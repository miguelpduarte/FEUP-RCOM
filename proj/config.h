#ifndef _INIT_H_
#define _INIT_H_

/**
 * @brief   Configures serial port for communication, storing the system configuration
 */
void set_config();

/**
 * @brief   Restores the system configuration
 */
void reset_config();

/**
 * @brief   Gets the serial port file descriptor
 * @return  serial port file descriptor
 */
int get_serial_port_fd();

#endif // _INIT_H_