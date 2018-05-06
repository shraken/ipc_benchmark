#ifndef  _COMMON_H_
#define  _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
typedef struct _msg_queue_item {
    uint16_t id;
    uint16_t type;
    char msgtext[16];
} msg_queue_item;
*/

typedef struct _msg_queue_entry {
    long mtype;
    char mtext[200];
} msg_queue_entry;

#endif