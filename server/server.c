// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>

#define PORT 8080

typedef struct __attribute__((__packed__)){
    char eof;
    char fileName[255];
    char packetBuffer[1024];
} Packet;

int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";
    double sleeptime = 0.5;
    //bytes/second
    long transfer_upper = 1024;
    long transfer_lower = 500;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    FILE* ptr;
    char ch;
    char fileName[255] = "hello.txt";
    ptr = fopen("hello.txt", "rb");
    if (ptr == NULL){
        printf("File counld not be opened");
    }
    Packet p;
    p.eof = 0;
    strcpy(p.fileName, fileName);
    int symbol;
    int i = 0;
    struct timeval start_time;
    struct timeval previous_end_time;
    struct timeval end_time;
    //get transmission start time
    gettimeofday(&start_time, NULL);

    int packet_count = 0;
    long avg_trans_rate;
    long current_trans_rate = 0;

    while(1){
        packet_count += 1;
        //we send 1024 bytes at a time
        while((symbol = getc(ptr)) != EOF && i < 1024)
        {
            p.packetBuffer[i] = (char)symbol;
            i++;
        }
        if ((int)symbol == EOF){
            p.eof = 1;
        }

        send(new_socket, (void *)&p, sizeof(p), 0);
        //we just sent 1024 bytes, calculate avg transmission time
        gettimeofday(&end_time, NULL);
        if (packet_count == 0){
            previous_end_time = start_time;
        }

        long total_time = ((end_time.tv_sec - start_time.tv_sec)*1000000L +end_time.tv_usec - start_time.tv_usec);

        avg_trans_rate = (int)((packet_count * 1024 * 1000000L)/total_time);
        // printf("Transmitted a packet, transmission rate: %d\n", avg_trans_rate);

        current_trans_rate = (1024 * 1000000L)/((end_time.tv_sec - previous_end_time.tv_sec) * 1000000L + end_time.tv_usec - previous_end_time.tv_usec);
        //see if our transfer rate is within threshold, and increase/decrease sleeptime accordingly
        if (current_trans_rate > transfer_upper){
            //we increase our sleeptime
            sleeptime *= 1.05;
        }
        if (current_trans_rate < transfer_lower){
            sleeptime *= 0.95;
        }

        //sleep by sleeptime
        printf("Current transfer rate: %d\n", current_trans_rate);
        printf("Sleeptime: %f\n", sleeptime);
        printf("SYMBOL: %s\n", &symbol);
        printf("SYMBOL IN HEX: %d\n", symbol);
        printf("Symbol is EOF: %d\n", (symbol == EOF));
        previous_end_time = end_time;
        sleep(sleeptime);
        if ((int)symbol == EOF){
            break;
        }
        
        memset(p.packetBuffer, 0, sizeof p.packetBuffer);
        i = 0;
    }

    fclose(ptr);


   // valread = read(new_socket, buffer, 1024);
 
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}
