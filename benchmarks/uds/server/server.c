#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <common.h>

uint8_t *allocate_buffer(int total_size) {
    int i;
    int rand_value;
    uint8_t *buffer;

    srand(time(NULL));
    buffer = (uint8_t *) malloc(total_size * 2);

    for (i = 0; i < total_size; i++) {
        rand_value = rand() % 0xFF;
        *(buffer + i) = (uint8_t) rand_value;
    }

    return buffer;
}

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
    int s, len;
    int n;
    double total_time;
    struct sockaddr_un remote;
    int total_bytes;
    int total_attempts;
    uint8_t *fbuf;
    struct timeval t_start, t_end;

    if (argc < 3) {
        print_usage(argv);
        exit(0);
    }

    int block_size = atoi(argv[1]);
    int total_size = atoi(argv[2]);

    printf("uds IPC server test\n");
    printf("using block_size = %d\n", block_size);
    printf("using total_size = %d\n", total_size);

    printf("allocating random buffer of length %d bytes\n", total_size);
    fbuf = allocate_buffer(total_size);

    if (!fbuf) {
        printf("error could not allocate memory\n");
        return -1;
    } else {
        printf("allocation finished\n");
    }

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("Connecting to %s\n", SOCK_PATH);

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected to %s\n", SOCK_PATH);

    while (1) {
        if (n = send(s,  fbuf + total_bytes, block_size, 0) == -1) {
            perror("send");
            exit(1);
        }

        if (n > 0) {
            total_bytes += n;
            total_attempts++;
        }

        if (total_bytes >= total_size * 2) {
            break;
        }
    }

    free(fbuf);
    close(s);

    gettimeofday(&t_end, NULL);
 
    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(total_bytes, total_attempts, total_time);
    return 0;
}