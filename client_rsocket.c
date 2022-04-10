#include <linux/types.h>
#include <rdma/rsocket.h>
#include <stdio.h>
#include <arpa/inet.h>
// #include <sys/socket.h>
// #include <unistd.h>

#define REQEUST_SIZE    512

int main(int argc, char **argv) {
    struct sockaddr_in server_addr;
    int fetch_size;
    int rsockfd;
    char *send_buf, *recv_buf;

    if (argc < 4) {
        printf("Usage: ./client [IPv4] [port] [amount to request in bytes]\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    fetch_size = atoi(argv[3]);

    // Send buffer allocation and fill request string in
    send_buf = malloc(REQEUST_SIZE);
    if (!send_buf) {
        perror("malloc() for send_buf failed.");
        return -1;
    }
    memset(send_buf, 0, REQEUST_SIZE);
    sprintf(send_buf, "Request - give me %d bytes of data!", fetch_size);

    // Recv buffer allocation    
    recv_buf = malloc(fetch_size + 1);
    if (!recv_buf) {
        perror("malloc() for recv_buf failed.");
        return -1;
    }
    memset(recv_buf, 0, fetch_size);
    recv_buf[fetch_size] = '\0';


    rsockfd = rsocket(AF_INET, SOCK_STREAM, 0);
    if (rsockfd == -1) {
        perror("rsocket()");
        exit(-1);
    }

    if (rconnect(rsockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("rconnect()");
        exit(-1);
    }

    rwrite(rsockfd, send_buf, strlen(send_buf));

    int read_total = 0;
    while (read_total < fetch_size)
        read_total += rread(rsockfd, recv_buf, fetch_size - read_total);
    

    printf("Received data: %s\n", recv_buf);

    rclose(rsockfd);
    return 0;
}