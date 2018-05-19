#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <common.h>

#define BUFFER_BYTES_LENGTH 8192
#define SOCK_PATH "/tmp/ipc_uds"

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
    int n;
    int s;

    bool pretty_mode;
    int block_size;
    int total_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

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

    gettimeofday(&t_start, NULL);

    if ((sockfd = accept(s, (struct sockaddr *) &remote, &t)) == -1) {
        perror("accept");
        exit(1);
    }

    //printf("Connected!\n");

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

    print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);

    return 0;
}