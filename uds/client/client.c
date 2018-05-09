#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <common.h>

void print_runtime_stats(int total_bytes, int total_attempts, double total_time) {
    double average_throughput;

    average_throughput = (double) total_bytes / total_time;
    
    printf("\n=========================================\n");
    printf("Average Throughput: %f MBytes/sec\n", average_throughput / 1e6);
    printf("Test took: %f secs with %d bytes\n", total_time, total_bytes);
    printf("Number of read calls on named pipe: %d\n", total_attempts);
    printf("=========================================\n");
    
    return;
}

void print_usage(char *argv[]) {
    printf("%s <block_size> <total_size>\n", argv[0]);
    return;
}

int main(int argc, char *argv[]) {
    int total_bytes;
    int total_attempts;
    int sockfd;
    int t;
    int len;
    double total_time;
    uint8_t tbuf[BUFFER_BYTES_LENGTH];
    struct sockaddr_un local, remote;
    struct timeval t_start, t_end;
    int rv;
    int n;
    int s;
    fd_set set;
    struct timeval timeout;
    bool connected = false;

    if (argc < 3) {
        print_usage(argv);
        exit(0);
    }

    int block_size = atoi(argv[1]);
    int total_size = atoi(argv[2]);

    printf("uds IPC client test\n");
    printf("using block_size = %d\n", block_size);
    printf("using total_size = %d\n", total_size);

    if (block_size > BUFFER_BYTES_LENGTH) {
        printf("error: block_size out of range\n");
        exit(1);
    }

    total_bytes = 0;
    total_attempts = 0;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);

    if (bind(s, (struct sockaddr *) &local, len) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(s, 5) == -1) {
        perror("listen");
        exit(1);
    }

    FD_ZERO(&set);
    FD_SET(s, &set);

    timeout.tv_sec = 0;
    timeout.tv_usec = 100;

    gettimeofday(&t_start, NULL);

    if ((sockfd = accept(s, (struct sockaddr *) &remote, &t)) == -1) {
        perror("accept");
        exit(1);
    }

    printf("Connected!\n");

    while (1) {
        n = recv(sockfd, tbuf, block_size, 0);

        if (n <= 0) {
            perror("recv");
            exit(1);
        } else {
            total_bytes += n;
            total_attempts++;
        }

        if (total_bytes >= total_size) {
            break;
        }
    }

    gettimeofday(&t_end, NULL);
    close(sockfd);

    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(total_bytes, total_attempts, total_time);

    return 0;
}