#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <common.h>

static char *fifo_name = "/tmp/ipc_fifo";

int main(int argc, char *argv[]) {
    int fd;
    int result;
    int n;
    int total_bytes;
    int total_attempts;
    double total_time;
    uint8_t tbuf[BUFFER_BYTES_LENGTH];
    struct timeval t_start, t_end;

    bool pretty_mode;
    int block_size;
    int total_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

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

    print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);

    return 0;
}