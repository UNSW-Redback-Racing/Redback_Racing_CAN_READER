#include <net/if.h>
#include <fcntl.h>
#include <stdbool.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "CanReader.h"

int open_can_socket(char* interface, bool using_pipe) {
  int can_socket = -1;
  if (using_pipe) {
    printf("Test Mode On: Using pipe: %s for CAN data\n", interface);
    can_socket = open(interface, O_RDWR);

    if (can_socket == -1) {
      printf("Error: Failed to open CAN pipe '%s'.\n", interface);
      exit(1);
    }

    return can_socket;
  } else {
    struct sockaddr_can addr;
    struct ifreq ifr;

    can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    ifr.ifr_ifindex = -1;

    strcpy(ifr.ifr_name, interface);
    ioctl(can_socket, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    fcntl(can_socket, F_SETFL, fcntl(can_socket, F_GETFL, 0) | O_NONBLOCK);

    int sockerr = bind(can_socket, (struct sockaddr*)&addr, sizeof(addr));

    if (sockerr < 0) {
      printf("FATAL: Failed to bind socket: %s\n", interface);
      exit(1);
    }
    printf("Successfully opened socket: %s\n", interface);

    return can_socket;
  }
  
}

bool read_can_frame(int can_socket, struct can_frame* frame)
{
  // polls the can_socket, will wait until there is data present
  // or 100ms before timing out
  struct pollfd fds[1];
  fds[0].fd = can_socket;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  int err = poll(fds, 1, 100);
  // if (err == -1) {
  //   throw std::runtime_error("FATAL: Failed to poll socket");
  // }

  if (fds[0].revents & POLLIN) {
    int num_bytes = read(can_socket, frame, sizeof(struct can_frame));
    return num_bytes == sizeof(struct can_frame);
  }
  else {
    return false;
  }
}
