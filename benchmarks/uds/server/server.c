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

#define SOCK_PATH "/tmp/ipc_uds"

int main(int argc, char *argv[]) {
    int s, len;
    int n;
    double total_time;
    struct sockaddr_un remote;
    int total_bytes;
    int total_attempts;
    uint8_t *fbuf;
    struct timeval t_start, t_end;

    bool pretty_mode;
    int block_size;
    int total_size;

    int write_size;
    
    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

    fbuf = allocate_buffer(total_size);

    if (!fbuf) {
        printf("error could not allocate memory\n");
        return -1;
    }

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family) + 1;

    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

    total_bytes = 0;
    while (1) {
        if ((total_bytes + block_size) < total_size) {
            write_size = block_size;
        } else {
            write_size = (total_size - total_bytes);
        }

        if ((n = send(s,  fbuf + total_bytes, write_size, 0)) == -1) {
            perror("send");
            free(fbuf);
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
    close(s);

    gettimeofday(&t_end, NULL);
 
    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);

    return 0;
}