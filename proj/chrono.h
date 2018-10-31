#ifndef _CHRONO_H_
#define _CHRONO_H_

/**
 * @brief   Starts the internal clock, by saving the starting time in an internal variable
 */
void start_clock();

/**
 * @brief   Stops the internal clock, by saving the end time in an internal variable
 */
void stop_clock();

/**
 * @brief   Prints the difference between the start and end times
 */
void print_clock_diff();

#endif // _CHRONO_H_