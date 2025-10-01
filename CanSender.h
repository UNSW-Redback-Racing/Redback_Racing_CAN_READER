#ifndef CAN_SENDER_H
#define CAN_SENDER_H

#include <linux/can.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Sends a CAN frame from the socket.
 * @param frame the frame struct to write the data into
 * @return True if a frame was sent, false otherwise.
 */
bool send_can_frame(int can_socket, struct can_frame frame);

#endif