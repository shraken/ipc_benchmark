#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_BUFFER_LENGTH 16384

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

uint8_t *allocate_buffer(int total_size) {
    int i;
    int rand_value;
    int total_bytes;
    int total_attempts;
    uint8_t *buffer;

    srand(time(NULL));
    buffer = (uint8_t *) malloc(total_size * 2);

    for (i = 0; i < total_size; i++) {
        rand_value = rand() % 0xFF;
        *(buffer + i) = (uint8_t) rand_value;
    }

    return buffer;
}

void print_usage(char *argv[]) {
    printf("%s <block_size> <total_size>\n", argv[0]);
    return;
}

int main(int argc, char *argv[]) {
    int fd[2];
    int n;
    int64_t total_bytes;
    int64_t total_attempts;
    double total_time;
    struct timeval t_start, t_end;
    pid_t child_pid;
    uint8_t *tbuf;
    uint8_t *fbuf;

    if (argc < 3) {
        print_usage(argv);
        exit(1);
    }

    int block_size = atoi(argv[1]);
    int total_size = atoi(argv[2]);

    if (block_size > MAX_BUFFER_LENGTH) {
        printf("block_size parameter invalid\n");
        exit(1);
    }

    printf("named_pipe IPC client test\n");
    printf("using block_size = %d\n", block_size);
    printf("using total_size = %d\n", total_size);

    fbuf = allocate_buffer(total_size);

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1) {
        perror("socketpair");
        exit(1);
    }

    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(1);
    } else if (child_pid == 0) {
        // child

        while (1) {
            n = write(fd[1], fbuf + total_bytes, block_size);

            if (n < 0) {
                perror("write");
                break;
            } else if (n > 0) {
                total_bytes += n;
                total_attempts++;
            }

            if (total_bytes >= total_size) {
                break;
            }
        }
    } else {
        // parent

        tbuf = (uint8_t *) malloc(block_size);
        gettimeofday(&t_start, NULL);

        // read 
        while (1) {
            n = read(fd[0], tbuf, block_size);

            if (n < 0) {
                perror("read");
                break;
            } else if (n > 0) {
                total_bytes += n;
                total_attempts++;
            }

            if (total_bytes >= total_size) {
                break;
            }
        }

        gettimeofday(&t_end, NULL);

        total_time = (double) (t_end.tv_sec - t_start.tv_sec);
        total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

        print_runtime_stats(total_bytes, total_attempts, total_time);
        free(tbuf);
    }

    free(fbuf);
    return 0;
}