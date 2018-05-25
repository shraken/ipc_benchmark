#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <czmq.h>
#include <common.h>

#define BUFFER_BYTES_LENGTH 8192

int main(int argc, char *argv[]) {
    int n;
    int total_bytes;
    int total_attempts;
    uint8_t *fbuf;
    struct timeval t_start, t_end;
    double total_time;

    bool pretty_mode;
    int block_size;
    int total_size;

    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    //int rc = zmq_bind(publisher, "tcp://*:5556");
    int rc = zmq_bind(publisher, "ipc:///tmp/ipctest");

    assert (rc == 0);
    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

    fbuf = allocate_buffer(total_size);

    if (!fbuf) {
        printf("error could not allocate memory\n");
        return -1;
    } else {
        printf("allocation finished\n");
    }

    total_bytes = 0;
    while (1) {
        n = zmq_send(publisher, fbuf + total_bytes, block_size, 0);
        //printf("wrote %d bytes\n", n);

        if (n < 0) {
            perror("zmq_send");
            break;
        } else if (n > 0) {
            total_bytes += n;
            total_attempts++;
        }

        if (total_bytes >= total_size) {
            total_bytes = (total_bytes % total_size);
        }
    }

    free(fbuf);
    gettimeofday(&t_end, NULL);
 
    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);
    
    zmq_close(publisher);
    zmq_ctx_destroy(context);

    return 0;
}