#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd <= 0) {
        perror("Socket creation failed");
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY connections from any address
    address.sin_port = htons(8080);

    int bind_stat = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(bind_stat < 0) {
        perror("bind failed");
        exit(1);
    }

    int listen_stat = listen(server_fd, 4);
    if(listen_stat < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("server is listening in the port 8080...\n");
    // call accept() it rturns client_fd for further communication
    socklen_t addrlen = sizeof(address);
    int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if(client_fd < 0){
        perror("accept failed");
        exit(1);    
    }
    printf("connection accepted");

    close(client_fd);
    close(server_fd);
    return 0;
}