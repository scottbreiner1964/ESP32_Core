#define FALSE 0 
#define TRUE 1

#include <Arduino.h>

// define the features to enable
#define CAN_HANDLER // allows for transmitting and receiving of CAN messages; setup of messages to be received and transmitted are in CAN_receive_config and CAN_transmit_config