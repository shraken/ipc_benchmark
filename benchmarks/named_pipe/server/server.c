#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

static char *fifo_name = "/tmp/ipc_fifo";

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
    int fd;
    int n;
    int total_bytes;
    int total_attempts;
    uint8_t *fbuf;
    struct timeval t_start, t_end;
    double total_time;

    if (argc < 3) {
        print_usage(argv);
        exit(0);
    }

    int block_size = atoi(argv[1]);
    int total_size = atoi(argv[2]);

    printf("named_pipe IPC server test\n");
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

    fd = open(fifo_name, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return -1;
    }

    while (1) {
        n = write(fd, fbuf + total_bytes, block_size);

        if (n < 0) {
            perror("write");
            break;
        } else if (n > 0) {
            total_bytes += n;
            total_attempts++;
        }

        if (total_bytes >= total_size * 2) {
            break;
        }
    }

    free(fbuf);
    close(fd);

    gettimeofday(&t_end, NULL);
 
    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(total_bytes, total_attempts, total_time);
    return 0;
}