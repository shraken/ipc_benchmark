#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <common.h>

static char *msgqueue_name = "server.c";

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
    int n;
    int total_bytes;
    int total_attempts;
    msg_queue_entry entry;
    uint8_t *fbuf;
    key_t key;
    int msqid;
    double total_time;
    struct timeval t_start, t_end;

    if (argc < 3) {
        print_usage(argv);
        exit(0);
    }

    int block_size = atoi(argv[1]);
    int total_size = atoi(argv[2]);

    printf("msg_queue IPC server test\n");
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

    if ((key = ftok(msgqueue_name, 'B')) == -1) {
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644)) == -1) {
        perror("msgget");
        exit(1);
    }

    entry.mtype = 1;

    while (1) {
        memcpy(entry.mtext, fbuf + (total_bytes % total_size), sizeof(entry.mtext));

        if ((n = msgsnd(msqid, &entry, sizeof(entry.mtext), 0)) == -1) {
            perror("msgsend");
        }
    }

    printf("freeing memory buffer A\n");
    free(fbuf);
    printf("freeing memory buffer B\n");

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    gettimeofday(&t_end, NULL);
 
    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(total_bytes, total_attempts, total_time);
    
    return 0;
}