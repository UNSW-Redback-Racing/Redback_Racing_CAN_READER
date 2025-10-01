#include <net/if.h>
#include <fcntl.h>
#include <stdbool.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "CanReader.h"

bool send_can_frame(int can_socket, struct can_frame frame)
{
  if (write(can_socket, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
   perror("Write");
   return 0;
  }
  return 1;
}
