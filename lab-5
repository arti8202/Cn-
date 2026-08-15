#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int count_vowel(char word[])
{
    int vowel = 0;

    for (int i = 9; word[i] != '\0'; i++)
    {
        if (word[i] == 'a' || word[i] == 'e' || word[i] == 'i' || word[i] == 'o' || word[i] == 'u' || word[i] == 'A' || word[i] == 'E' || word[i] == 'I' || word[i] == 'O' || word[i] == 'U')
        {
            vowel++;
        }

       
    }
    return vowel;
}
int count_words(char word[])
{
    int space = 0;
    for (int i = 9; word[i] != '\0'; i++)
    {

        if (word[i] == ' ')
        {
            space++;
        }
    }
    return space;
}
int count_chars(char word[])
{
    int chars = 0;
    for (int i = 9; word[i] != '\0'; i++)
    {
        chars++;
    }
    return chars;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Define address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // 3. Bind socket
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // 4. Listen for connections
    listen(server_fd, 3);

    printf("Server waiting on port %d...\n", PORT);

    // 5. Accept connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int n = read(new_socket, buffer, sizeof(buffer) - 1);

        if (n <= 0)
            break;

        buffer[n] = '\0';

        if (strcmp(buffer, "bye") == 0)
        {
            printf("Thank u for using our app..");
            break;
        }
        printf("Client: %s\n", buffer);
        int vowel = 0;
        int chars = 0;
        int words = 0;
        if (buffer[0] == 'A' && buffer[1] == 'N' && buffer[2] == 'A' && buffer[3] == 'L' && buffer[4] == 'Y' && buffer[5] == 'Z' && buffer[6] == 'E' && buffer[7] == ' ' && buffer[8] == '|')
        {
            vowel = count_vowel(buffer);
            chars = count_chars(buffer);
            words = count_words(buffer);
        }

        memset(buffer, 0, sizeof(buffer));
        char buffer[1024];
        sprintf(buffer, "Vowel: %d\n CHARS: %d\n Words: %d\n", vowel, chars, words);
        printf("Result: %s\n",buffer);
        send(new_socket, buffer, strlen(buffer), 0);
    }

    close(new_socket);
    close(server_fd);

    return 0;
}
