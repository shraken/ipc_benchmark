#include <common.h>

void print_usage(char *argv[]) {
    printf("%s -a -b <block_size> -c <total_size>\n", argv[0]);
    return;
}

int parse_arguments(int argc, char *argv[], int *block_size, int *total_size, bool *pretty_mode) {
    int c;
    bool parse_pretty_mode = false;
    int parse_block_size;
    int parse_total_size;
    
    if (argc < 3) {
        print_usage(argv);
        exit(0);
    }

    opterr = 0;

    while ((c = getopt(argc, argv, "ab:c:")) != -1) {
        switch (c) {
            case 'a':
                parse_pretty_mode = true;
                break;

            case 'b':
                parse_block_size = atoi(optarg);
                break;

            case 'c':
                parse_total_size = atoi(optarg);
                break;

            case '?':
                if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                break;

            default:
                abort();
        }
    }
    
    printf("parse_arguments()\n");
    printf("parse_pretty_mode = %d\n", parse_pretty_mode);
    printf("parse_block_size = %d\n", parse_block_size);
    printf("parse_total_size = %d\n", parse_total_size);

    *pretty_mode = parse_pretty_mode;
    printf("A\n");

    *block_size = parse_block_size;
    printf("B\n");

    *total_size = parse_total_size;
    printf("C\n");

    return 0;
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