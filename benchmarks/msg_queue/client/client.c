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

static char *msgqueue_name = "../server/server.c";

int main(int argc, char *argv[]) {
    int n;
    int total_bytes;
    int total_attempts;
    double total_time;
    int msqid;
    key_t key;
    msg_queue_entry entry;
    struct timeval t_start, t_end;

    bool pretty_mode;
    int block_size;
    int total_size;

    parse_arguments(argc, argv, &block_size, &total_size, &pretty_mode);
    
    /*
    printf("msg_queue IPC client test\n");
    printf("using block_size = %d\n", block_size);
    printf("using total_size = %d\n", total_size);
    */
   
    //remove(msgqueue_name);
    if ((key = ftok(msgqueue_name, 'B')) == -1) {
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }

    total_bytes = 0;
    total_attempts = 0;
    
    gettimeofday(&t_start, NULL);
    
    while (1) {
        if ((n = msgrcv(msqid, &entry, sizeof(entry.mtext), 0, 0)) == -1) {
            perror("msgrcv");
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

    gettimeofday(&t_end, NULL);

    total_time = (double) (t_end.tv_sec - t_start.tv_sec);
    total_time += (double) (t_end.tv_usec - t_start.tv_usec) / 1000000;

    print_runtime_stats(pretty_mode, total_bytes, total_attempts, total_time);

    return 0;
}