#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "proto/can_frame.pb.h"

#include "CanReader.h"
#include "CanSender.h"

//struct for holding information
struct arg_struct {
  int can_socket;
};

//Gets current time of the device
uint64_t get_current_time()
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000000000ULL + spec.tv_nsec;
}

//For printing a pretty visual of the Can Frame
void print_can_frame(uint32_t can_id, pb_byte_t* can_data, char* can_bus)
{
  printf("[CAN Frame]\n id: %u \n", can_id);
  printf(" data: ");
  for (int i = 0; i < 8; i++) {
    printf("%x ", can_data[i]);
  }
  printf("\n");
  //prints which bus it comes from
  printf(" interface: %s\n", can_bus);
}

void* send_heartbeat(void *arguments)
{
  //Decoding our arguments we were given in a struct
  struct arg_struct *args = arguments;
  int can_socket = args->can_socket;

  //Creating Can Frame for can
  struct can_frame can_message;

  //This is based on your DBC file or if your building it from scratch, whatever info you want
  can_message.can_id = 0x29A;

  //Specifies a 1 byte of info being sent
  can_message.can_dlc = 1;

  //The first and only byte being sent has the value 1
  pb_byte_t can_data[8] = {1,0,0,0,0,0,0,0};

  //assigning data can message
  for (int i = 0; i < 8; i++) {
    can_message.data[i] = can_data[i];
  }

  struct timespec seconds, nanoseconds = { 0, 500000000 }; 
  //Waits half a second before it sends a new can frame to the can bus
  nanosleep(&nanoseconds, &seconds); 
  if(!send_can_frame(can_socket, can_message)) {
    printf("\nError no heart beat sent.\n");
  }
}


//Heres our main function
int main(int argc, char* argv[]) {
    //We need to keep track of our can interfaces, we do this by assigning memory as these values are super important
    //The number of can interfaces is deterimined by the starting command
    char** can_interfaces = malloc(sizeof(char*) * (argc - 1));
    int* can_sockets = malloc(sizeof(int) * (argc - 1));
    
    //If you haven't put in the information correctly it prompts you and exits  
    if (argc <= 1) {
        printf("Usage: %s <CAN interfaces>\n", argv[0]);
        exit(1);
    }

    //We determine the amount of CAN buses we have, since the first part of the starting command "./program.exe counts as 1 we have to specificy -1"
    int num_can_interfaces = argc - 1;

    //This opens every can interface so its ready to recieve can packets
    for (int i = 0; i < num_can_interfaces; i++) {
      can_interfaces[i] = argv[1 + i];
      can_sockets[i] = open_can_socket(can_interfaces[i], false);
    }

    //Sets the sending canbus to canbus 0
    struct arg_struct args;
    args.can_socket = can_sockets[0];

    //And heres our final recieving loop, this loop will read all the incoming can frames and print them out for us
    //This can be edited to do a lot of things are later examples will show
    while (1) {
      //Search through each can bus individually
      for (int i = 0; i < num_can_interfaces; i++) {
        //this custom struct is defined in the proto folder
        struct can_frame frame;

        //If theres nothing to read it skips it
        if (!read_can_frame(can_sockets[i], &frame)) {
          continue;
        }
        // Here we can get all the data from the frame, the two things we get are the frame ID and the data
        uint32_t can_id = frame.can_id;
        uint64_t timestamp = get_current_time();

        //Finally print out the data
        print_can_frame(can_id, frame.data, can_interfaces[i]);
      }
      send_heartbeat(&args);
    }
}