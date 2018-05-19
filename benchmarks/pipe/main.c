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

int main(int argc, char *argv[]) {
    int fd[2];
    int n;
    int total_bytes;
    int total_attempts;
    double total_time;
    struct timeval t_start, t_end;
    pid_t child_pid;
    uint8_t *tbuf;
    uint8_t *fbuf;

    bool pretty_mode;
    int block_size;
    int total_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

    fbuf = allocate_buffer(total_size);
    
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    total_bytes = 0;

    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(1);
    } else if (child_pid == 0) {
        // child
        close(fd[0]);

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
        close(fd[1]);

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

        print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);
        free(tbuf);
    }

    free(fbuf);
    return 0;
}