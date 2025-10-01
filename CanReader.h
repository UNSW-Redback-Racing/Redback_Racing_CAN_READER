#ifndef CAN_READER_H
#define CAN_READER_H

#include <linux/can.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Opens a socket for the CAN bus.
 *  - If using_pipe_ is true, it will read from a pipe instead of the CAN bus.
 * @param interface The interface / pipe to open the socket on.
 * @throws std::runtime_error if the interface / pipe cannot be bound to the socket.
 */
int open_can_socket(char* interface, bool using_pipe);

/**
 * Reads a CAN frame from the socket.
 * @param frame the frame struct to write the data into
 * @return True if a frame was read, false otherwise.
 */
bool read_can_frame(int can_socket, struct can_frame* frame);

#endif