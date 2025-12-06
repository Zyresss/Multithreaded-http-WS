#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void *handle_client(void *client_socket_ptr)
{
    int client_fd = *(int *)client_socket_ptr;

    char buffer[30000] = {0};
    long data = read(client_fd, buffer, 30000);
    char method[10] = {0};
    char url[100] = {0};
    sscanf(buffer, "%s %s", method, url); // sscanf
    printf("Method: %s, URL: %s\n", method, url);

    char *file_name;
    if(strcmp(url, "/") == 0) {
        file_name = "index.html";

    } else {
        file_name = url + 1; // + for skiping the '/'
    }
    printf("%s\n", file_name);

    // printf("-------client request--------\n");
    // printf("the data is : %s\n", buffer);z
    // printf("-----------------------------\n");

    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        printf("File not found: %s\n", file_name);
        char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
        write(client_fd, not_found_response, strlen(not_found_response));
        close(client_fd);
        free(client_socket_ptr);
        return NULL;
    }
    char response_data[30000] = {0};
    fread(response_data, 30000, 1, fp);
    fclose(fp);

    char* content_type;
    if(strstr(file_name, ".css")) {
        content_type = "text/css";

    } else if(strstr(file_name, ".html")) {
        content_type = "text/html";
    } else if(strstr(file_name, ".js")){
        content_type = "application/javascript";
    } else if(strstr(file_name, ".jpg") || strstr(file_name, ".jpeg")) {
        content_type = "image/jpeg";
    } else if(strstr(file_name, ".png")) {
        content_type = "image/png";
    } else {
        content_type = "text/plain";
    }

    char header[2048] = {0};
    sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type); // sprintf it formats and stores the string in header from the library <string.h>
    write(client_fd, header, strlen(header));
    write(client_fd, response_data, strlen(response_data));
    printf("Response sent to browser!\n");

    free(client_socket_ptr);
    close(client_fd);
    return NULL;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd <= 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY connections from any address
    address.sin_port = htons(8080);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int bind_stat = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if (bind_stat < 0)
    {
        perror("bind failed");
        exit(1);
    }

    int listen_stat = listen(server_fd, 10);
    if (listen_stat < 0)
    {
        perror("listen failed");
        exit(1);
    }

    printf("server is listening in the port 8080...\n");
    // call accept() it rturns client_fd for further communication
    socklen_t addrlen = sizeof(address);
    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0)
        {
            perror("accept failed");
            exit(1);
        }
        printf("connection accepted\n");

        int *pclient = malloc(sizeof(int));
        *pclient = client_fd;
        pthread_t thread_id;
        int thread_stat = pthread_create(&thread_id, NULL, handle_client, pclient);
        if(thread_stat != 0) {
            perror("failed to create thread!");
            exit(1);
        }
        pthread_detach(thread_id); //



        // char buffer[30000] = {0};
        // long data = read(client_fd, buffer, 30000);

        // // printf("-------client request--------\n");
        // // printf("the data is : %s\n", buffer);
        // // printf("-----------------------------\n");

        // FILE* fp;
        // fp = fopen("index.html", "r");
        // if (fp == NULL) {
        //     perror("Failed to open file");
        //     close(client_fd);
        //     continue;
        // }
        // char response_data[30000] = {0};
        // fread(response_data, 30000, 1, fp);
        // fclose(fp);

        // char header[2048] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        // write(client_fd, header, strlen(header));
        // write(client_fd, response_data, strlen(response_data));
        // printf("Response sent to browser!\n");
        // close(client_fd);
    }

    close(server_fd);
    return 0;
}