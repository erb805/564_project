// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

typedef struct __attribute__((__packed__)) {
    char eof;
    char fname[255];
    char payload[1024];
} Packet;


int main(int argc, char const* argv[])
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "192.168.1.155", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
 
    if ((client_fd
         = connect(sock, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    FILE* ptr = NULL;
    Packet packet;
    while(1){
        /* read a packet*/
        read(sock, (void*)&packet, (ssize_t)sizeof(packet));
        if (ptr == NULL){
            //open the file if it has not been opened already
            ptr = fopen(packet.fname, "wb");
            if (ptr == NULL){
                printf("File counld not be opened");
            }
        }
        //write out data to the file
        write(fileno(ptr), (void*)packet.payload,1024);
        if (packet.eof){
            break;
        }
    }

    fclose(ptr);


   // valread = read(new_socket, buffer, 1024);
 
    // closing the connected socket
    close(sock);
    // closing the listening socket
    shutdown(client_fd, SHUT_RDWR);
    return 0;

    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
 
    // closing the connected socket
    close(client_fd);
    return 0;
}
