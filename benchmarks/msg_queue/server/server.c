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

typedef struct _msg_queue_entry {
    long mtype;
    char mtext[200];
} msg_queue_entry;

static char *msgqueue_name = "server.c";

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

    bool pretty_mode;
    int block_size;
    int total_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);

    // printf("allocating random buffer of length %d bytes\n", total_size);
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
    total_bytes = 0;
    
    while (1) {
        memcpy(entry.mtext, fbuf + (total_bytes % total_size), sizeof(entry.mtext));

        if ((n = msgsnd(msqid, &entry, sizeof(entry.mtext), 0)) == -1) {
            perror("msgsend");
        }
    }

    free(fbuf);

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    gettimeofday(&t_end, NULL);
 
    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);
    
    return 0;
}