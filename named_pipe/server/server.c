#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <common.h>

static char *fifo_name = "/tmp/ipc_fifo";

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

uint8_t *allocate_buffer(void) {
    int i;
    int rand_value;
    int total_bytes;
    int total_attempts;
    uint8_t *buffer;

    srand(time(NULL));
    buffer = (uint8_t *) malloc(BYTES_TO_READ * 2);

    for (i = 0; i < BYTES_TO_READ; i++) {
        rand_value = rand() % 0xFF;
        *(buffer + i) = (uint8_t) rand_value;
    }

    return buffer;
}

int main(int argc, char *argv[]) {
    int fd;
    int n;
    int total_bytes;
    int total_attempts;
    uint8_t *fbuf;
    uint8_t *foo;

    printf("named_pipe IPC server test\n");

    printf("allocating random buffer of length %d bytes\n", BYTES_TO_READ);
    fbuf = allocate_buffer();

    if (!fbuf) {
        printf("error could not allocate memory\n");
        return -1;
    } else {
        printf("allocation finished\n");
    }

    fd = open(fifo_name, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return -1;
    }

    while (1) {
        n = write(fd, fbuf + total_bytes, BUFFER_BYTES_LENGTH);

        if (n < 0) {
            perror("write");
            break;
        } else if (n > 0) {
            total_bytes += n;
            total_attempts++;
        }

        if (total_bytes >= BYTES_TO_READ * 2) {
            break;
        }
    }

    //free(fbuf);
    close(fd);

    return 0;
}