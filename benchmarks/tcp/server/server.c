#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <common.h>

#define SERVER_PORT "5000"
#define SERVER_HOST "localhost"

// based off Beej's Network example code
// https://beej.us/guide/bgnet/html/multi/clientserver.html#simpleserver

#define SERVER_BACKLOG 10

// @todo common code with client, fix
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    int new_fd;
    int sockfd;
    int rv;
    int yes=1;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    int n;
    double total_time;
    int total_bytes;
    int total_attempts;
    uint8_t *fbuf;
    struct timeval t_start, t_end;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];

    bool pretty_mode;
    int block_size;
    int total_size;

    int write_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

    fbuf = allocate_buffer(total_size);

    if (!fbuf) {
        printf("error could not allocate memory\n");
        return -1;
    } else {
        printf("allocation finished\n");
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, SERVER_BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    sin_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
        return 1;
    }

    inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),
        s, sizeof s);
    //printf("server: got connection from %s\n", s);

    while (1) {
        if ((total_bytes + block_size) < total_size) {
            write_size = block_size;
        } else {
            write_size = (total_size - total_bytes);
        }

        if ((n = send(new_fd,  fbuf + total_bytes, write_size, 0)) == -1) {
            perror("send");
            exit(1);
        }

        if (n > 0) {
            total_bytes += n;
            total_attempts++;
        }

        if (total_bytes >= total_size) {
            break;
        }
    }

    free(fbuf);
    close(new_fd);
    close(sockfd);

    gettimeofday(&t_end, NULL);
 
    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);

    return 0;
}