#include <linux/types.h>
#include <rdma/rsocket.h>
#include <stdio.h>
#include <arpa/inet.h>

#define REQEUST_SIZE    512

int main(int argc, char **argv) {
    struct sockaddr_in server_addr;
    int send_size;
    int rsockfd_listen, rsockfd_conn;
    char *send_buf, *recv_buf;

    if (argc < 4) {
        printf("Usage: ./server [IPv4] [port] [reply size]\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    send_size = atoi(argv[3]);

    // Send buffer allocation and fill request string in
    send_buf = malloc(send_size);
    if (!send_buf) {
        perror("malloc() for send_buf failed.");
        return -1;
    }
    for (int i=0; i < send_size; i++)
        send_buf[i] = 0x41 + (i % 26); // 0x41 = 'A'

    // Recv buffer allocation    
    recv_buf = malloc(REQEUST_SIZE);
    if (!recv_buf) {
        perror("malloc() for recv_buf failed.");
        return -1;
    }
    memset(recv_buf, 0, REQEUST_SIZE);
    recv_buf[REQEUST_SIZE-1] = '\0';


    rsockfd_listen = rsocket(AF_INET, SOCK_STREAM, 0);
    if (rsockfd_listen == -1) {
        perror("rsocket()");
        exit(-1);
    }

    if (rbind(rsockfd_listen, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("rbind()");
        exit(-1);
    }

    rlisten(rsockfd_listen, 1);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    rsockfd_conn = raccept(rsockfd_listen, (struct sockaddr *) &client_addr, &client_addr_size);
    if (rsockfd_conn == -1) {
        perror("raccept()");
        exit(-1);
    }

    rread(rsockfd_conn, recv_buf, REQEUST_SIZE);
    printf("Message from client: %s\n", recv_buf);

    // int sent_total = 0;
    rwrite(rsockfd_conn, send_buf, send_size);


    rclose(rsockfd_conn);
    rclose(rsockfd_listen);
    return 0;
}