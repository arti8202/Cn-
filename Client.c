
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
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

int main(void) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct Query query;
    char response[RESPONSE_SIZE];

    memset(&query, 0, sizeof(query));

    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        perror("Cannot open data.txt");
        return 1;
    }

    size_t bytes = fread(query.data, 1, DATA_SIZE - 1, file);

    if (ferror(file)) {
        perror("Error reading data.txt");
        fclose(file);
        return 1;
    }

    fclose(file);

    if (bytes == DATA_SIZE - 1) {
        fprintf(stderr, "data.txt is too large for this program.\n");
        return 1;
    }

    printf("Enter Student ID to search: ");

    if (fgets(query.id, sizeof(query.id), stdin) == NULL) {
        fprintf(stderr, "Could not read Student ID.\n");
        return 1;
    }

    query.id[strcspn(query.id, "\r\n")] = '\0';

    if (query.id[0] == '\0' ||
        strspn(query.id, "0123456789") != strlen(query.id)) {
        fprintf(stderr, "Please enter a numeric Student ID.\n");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET,"10.85.16.94",&server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    if (write_full(sockfd, &query, sizeof(query))
        != sizeof(query)) {
        perror("Sending query failed");
        close(sockfd);
        return 1;
    }

    memset(response, 0, sizeof(response));

    if (read_full(sockfd, response, sizeof(response))
        != sizeof(response)) {
        fprintf(stderr, "Could not receive complete response.\n");
        close(sockfd);
        return 1;
    }

    response[RESPONSE_SIZE - 1] = '\0';
    printf("Server Response: %s\n", response);

    close(sockfd);
    return 0;
}
