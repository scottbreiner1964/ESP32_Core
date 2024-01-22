#define FALSE 0 
#define TRUE 1

//#define CAN_MESSAGE_TIMESTAMP // if time stamps for received CAN messages are needed enable this;  most likely only needed for logging CAN messages or displaying

#include <Arduino.h>

// define the features to enable
//#define CAN_HANDLER // allows for transmitting and receiving of CAN messages; setup of messages to be received and transmitted are in CAN_receive_config and CAN_transmit_config

// Wireless CAN bridge implementation using ESPNOW
#define CAN_BRIDGE_WIRELESS 
