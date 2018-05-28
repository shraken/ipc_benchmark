#ifndef  _COMMON_H_
#define  _COMMON_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>

#define BUFFER_BYTES_LENGTH 65536

uint8_t *allocate_buffer(uint64_t total_size);
void print_usage(char *argv[]);
int parse_arguments(int argc, char *argv[], int *block_size, int *total_size, bool *pretty_mode);
void print_runtime_stats(bool pretty_mode, int total_bytes, int total_attempts, double total_time);

#endif