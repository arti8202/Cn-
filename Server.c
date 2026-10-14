
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define DATA_SIZE 4096
#define ID_SIZE 32
#define RESPONSE_SIZE 256

struct Query {
    char id[ID_SIZE];
    char data[DATA_SIZE];
};

ssize_t read_full(int fd, void *buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = read(fd, (char *)buffer + total, size - total);
        if (n == 0) return total == 0 ? 0 : -1;
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        total += n;
    }
    return total;
}

ssize_t write_full(int fd, const void *buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = write(fd, (const char *)buffer + total,
                          size - total);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        total += n;
    }
    return total;
}

void handle_client(int client_fd) {
    struct Query query;
    char response[RESPONSE_SIZE];
    char *saveptr;
    char *line;

    memset(&query, 0, sizeof(query));

    if (read_full(client_fd, &query, sizeof(query))
        != sizeof(query)) {
        close(client_fd);
        _exit(1);
    }

    query.id[ID_SIZE - 1] = '\0';
    query.data[DATA_SIZE - 1] = '\0';

    int found = 0;
    line = strtok_r(query.data, "\n", &saveptr);

    while (line != NULL) {
        int student_id;
        char name[100], branch[100];
        char extra;

        if (sscanf(line, "%d %99s %99s %c",
                   &student_id, name, branch, &extra) == 3) {
            char requested[ID_SIZE];
            snprintf(requested, sizeof(requested), "%d", student_id);

            if (strcmp(requested, query.id) == 0) {
                snprintf(response, sizeof(response),
                         "Record Found : %s %s %s",
                         requested, name, branch);
                found = 1;
                break;
            }
        }

        line = strtok_r(NULL, "\n", &saveptr);
    }

    if (!found) {
        snprintf(response, sizeof(response), "Record Not Found");
    }

    write_full(client_fd, response, sizeof(response));

    printf("Child %ld handled a client.\n", (long)getpid());

    close(client_fd);
    _exit(0);
}

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    signal(SIGCHLD, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd,
                           (struct sockaddr *)&address, &addrlen);

        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            close(client_fd);
        } else if (pid == 0) {
            close(server_fd);
            handle_client(client_fd);
        } else {
            close(client_fd);
        }

        addrlen = sizeof(address);
    }

    close(server_fd);
    return 0;
}
