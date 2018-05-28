#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <common.h>

#define SERVER_PORT "5000"
#define SERVER_HOST "localhost"

// based off Beej's Network example code
// https://beej.us/guide/bgnet/html/multi/clientserver.html#simpleserver

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    int total_bytes;
    int total_attempts;
    int sockfd;
    double total_time;
    uint8_t tbuf[BUFFER_BYTES_LENGTH];
    struct timeval t_start, t_end;
    int rv;
    int n;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo, *p;

    bool pretty_mode;
    int block_size;
    int total_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(SERVER_HOST, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    total_bytes = 0;
    total_attempts = 0;

    gettimeofday(&t_start, NULL);

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