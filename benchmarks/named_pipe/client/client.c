#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_BYTES_LENGTH 8192

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

void print_usage(char *argv[]) {
    printf("%s <block_size> <total_size>\n", argv[0]);
    return;
}

int main(int argc, char *argv[]) {
    int fd;
    int result;
    int n;
    int total_bytes;
    int total_attempts;
    double total_time;
    uint8_t tbuf[BUFFER_BYTES_LENGTH];
    struct timeval t_start, t_end;

    if (argc < 3) {
        print_usage(argv);
        exit(0);
    }

    int block_size = atoi(argv[1]);
    int total_size = atoi(argv[2]);

    printf("named_pipe IPC client test\n");
    printf("using block_size = %d\n", block_size);
    printf("using total_size = %d\n", total_size);

    remove(fifo_name);
    result = mkfifo(fifo_name, 0666);
    if (result < 0) {
        perror("mkfifo");
        exit(-1);
    }

    total_bytes = 0;
    total_attempts = 0;

    fd = open(fifo_name, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return -1;
    }
    
    gettimeofday(&t_start, NULL);
    
    // read 
    while (1) {
        n = read(fd, tbuf, block_size);

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
    close(fd);

    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(total_bytes, total_attempts, total_time);

    return 0;
}