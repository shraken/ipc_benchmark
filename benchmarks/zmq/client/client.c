#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
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
    double total_time;
    uint8_t tbuf[BUFFER_BYTES_LENGTH];
    struct timeval t_start, t_end;

    bool pretty_mode;
    int block_size;
    int total_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

    void *context = zmq_ctx_new ();
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    //int rc = zmq_connect (subscriber, "tcp://localhost:5556");
    int rc = zmq_connect(subscriber, "ipc:///tmp/ipctest");

    assert (rc == 0);

    char *filter = "";
    rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE,
                         filter, strlen (filter));
    assert (rc == 0);
    
    total_bytes = 0;
    total_attempts = 0;

    gettimeofday(&t_start, NULL);
    
    // read 
    while (1) {
        n = zmq_recv(subscriber, tbuf, block_size, 0);
        //printf("got %d bytes\n", n);

        if (n < 0) {
            perror("zmq_recv");
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

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return 0;
}