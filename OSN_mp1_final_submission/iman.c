#include "iman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80
#define INITIAL_BUFFER_SIZE 4096

void url_encode(const char *src, char *dest, int max_dest_size);

void fetch_man_page(const char *command) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[INITIAL_BUFFER_SIZE];
    char *request;
    char encoded_command[INITIAL_BUFFER_SIZE];
    size_t request_size;
    
    // URL encode the command
    url_encode(command, encoded_command, sizeof(encoded_command));
    printf("Encoded command: %s\n", encoded_command); // Debug print

    // Determine the size of the request buffer
    request_size = snprintf(NULL, 0, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\nConnection: close\r\n\r\n", encoded_command);
    
    // Allocate memory for the request buffer
    request = (char *)malloc(request_size + 1);
    if (request == NULL) {
        perror("Error allocating memory");
        return;
    }

    // Prepare the HTTP GET request
    snprintf(request, request_size + 1,
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: man.he.net\r\n"
             "Connection: close\r\n"
             "\r\n", encoded_command);

    printf("Request:\n%s", request); // Debug print

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        free(request);
        return;
    }

    // Resolve the hostname
    server = gethostbyname("man.he.net");
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        close(sockfd);
        free(request);
        return;
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        close(sockfd);
        free(request);
        return;
    }

    // Send the request
    if (send(sockfd, request, request_size, 0) < 0) {
        perror("Error sending request");
        close(sockfd);
        free(request);
        return;
    }

    // Receive the response
    int received;
    int header_end = 0;
    while ((received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[received] = '\0';
        printf("Received data:\n%s", buffer); // Debug print

        // Skip the HTTP header
        if (!header_end) {
            char *header_end_pos = strstr(buffer, "\r\n\r\n");
            if (header_end_pos) {
                header_end = 1;
                printf("%s", header_end_pos + 4);  // Print after the header
            }
        } else {
            printf("%s", buffer);
        }
    }

    if (received < 0) {
        perror("Error receiving response");
    }

    // Close the socket and free allocated memory
    close(sockfd);
    free(request);
}

void url_encode(const char *src, char *dest, int max_dest_size) {
    const char *hex = "0123456789ABCDEF";
    while (*src && max_dest_size > 3) {
        if (('a' <= *src && *src <= 'z') || ('A' <= *src && *src <= 'Z') || 
            ('0' <= *src && *src <= '9') || (*src == '-' || *src == '_' || *src == '.' || *src == '~')) {
            *dest++ = *src;
            max_dest_size--;
        } else {
            *dest++ = '%';
            *dest++ = hex[*src >> 4];
            *dest++ = hex[*src & 15];
            max_dest_size -= 3;
        }
        src++;
    }
    *dest = '\0';
}
