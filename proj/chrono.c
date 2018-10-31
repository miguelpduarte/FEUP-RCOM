#include "chrono.h"
#include <time.h>
#include <stdio.h>

static int t_measure1, t_measure2;
static struct timespec start_time, end_time;

void start_clock() {
    t_measure1 = clock_gettime(CLOCK_REALTIME, &start_time);
}

void stop_clock() {
    t_measure2 = clock_gettime(CLOCK_REALTIME, &end_time);
}

void print_clock_diff() {
    if (t_measure1 == -1 || t_measure2 == -1) {
        fprintf(stderr, "Failed to measure elapsed time.");
    } else {
        double elapsed_time = ( end_time.tv_sec - start_time.tv_sec )
                            + ( end_time.tv_nsec - start_time.tv_nsec )
                            / 1e9;
        printf("Elapsed time: %lfs\n", elapsed_time);
    }
}