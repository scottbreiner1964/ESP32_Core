//

  #define MANUAL_TRANSMIT (65535)

  enum transmit_CAN_message_names
  {
    // BLUETOOTH_CAN_BRIDGE_MSG,

     TX_EXAMPLE1_MESSAGE,
          #if 0
     TX_EXAMPLE2_MESSAGE,
     TX_EXAMPLE3_MESSAGE,
     TX_EXAMPLE4_MESSAGE,
     TX_EXAMPLE5_MESSAGE,
     TX_EXAMPLE6_MESSAGE,
     TX_EXAMPLE7_MESSAGE,
     TX_EXAMPLE8_MESSAGE,
     TX_EXAMPLE9_MESSAGE,
     TX_EXAMPLE10_MESSAGE,
     TX_EXAMPLE11_MESSAGE,
     TX_EXAMPLE12_MESSAGE,
     TX_EXAMPLE13_MESSAGE,
     TX_EXAMPLE14_MESSAGE,
     TX_EXAMPLE15_MESSAGE,
     TX_EXAMPLE16_MESSAGE,
     TX_EXAMPLE17_MESSAGE,
     #endif
     TX_LAST_MESSAGE,
  };

  struct CAN_Transmit_Config_Struct
  {
    enum transmit_CAN_message_names index;
    unsigned long CAN_ID;
    enum message_type type;
    int command_byte_1;
    int command_byte_2;
    unsigned int transmit_rate; // ms transmit rate; MANUAL_TRANSMIT means the message is manually transmitted by the application task
  };

  struct CAN_Transmit_Data_Struct
  {
    unsigned char data[8];
    unsigned int transmit_timer; 
  };



const struct CAN_Transmit_Config_Struct Transmit_CAN_Config[] =  
{
    /*****************  The enumeration list transmit_CAN_message_names and the below structure need to be in the same order !!!!!!!!!! *************/
    /**************** Also, make sure to order your messages in terms of transmit rate ******************************************/
    
    /*  Index: This enumeration is used for a manual transmit and loading data into the transmit message
   *   29-bit ID: The 29-bit CAN identifier
   *   type: Type of message being transmitted (i.e. does it have control bytes )
   *   Control Byte 1,2: The control bytes if used, if not set the value to -1
   *   Transmit rate: transmit rate in ms.  Note, it needs to be a multiple of 5ms.
   */
 // {BLUETOOTH_CAN_BRIDGE_MSG,                0x18FFBD91ul,         CAN_ID_ONLY,           -1,         -1,       MANUAL_TRANSMIT},
 
  {TX_EXAMPLE1_MESSAGE,                0x18FFFC4Dul,         CAN_ID_ONLY,                      -1,           -1,         100},
     #if 0
  {TX_EXAMPLE2_MESSAGE,              0x18FFFC40ul,         CAN_ID_ONLY,                      -1,           -1,         100},
  {TX_EXAMPLE3_MESSAGE,              0x18FFFC43ul,         CAN_ID_ONLY,                      -1,           -1,         100},
  {TX_EXAMPLE4_MESSAGE,              0x18FFFC47ul,         CAN_ID_ONLY,                      -1,           -1,         100},
  {TX_EXAMPLE5_MESSAGE,              0x18FFFC4Aul,         CAN_ID_ONLY,                      -1,           -1,         100},
  {TX_EXAMPLE6_MESSAGE,              0x18FFFC4Ful,         CAN_ID_ONLY,                      -1,           -1,         100},
  {TX_EXAMPLE7_MESSAGE,                0x18FFFC4Eul,         CAN_ID_1_CONTROL_BYTE,            0x20,         -1,         100},
  {TX_EXAMPLE8_MESSAGE,                0x18FFFC41ul,         CAN_ID_1_CONTROL_BYTE,            0x20,         -1,         100},
  {TX_EXAMPLE9_MESSAGE,                0x18FFFC45ul,         CAN_ID_1_CONTROL_BYTE,            0x20,         -1,         100},
  {TX_EXAMPLE10_MESSAGE,                0x18FFFC48ul,         CAN_ID_1_CONTROL_BYTE,            0x20,         -1,         100},
  {TX_EXAMPLE11_MESSAGE,                0x18FFFC4Bul,         CAN_ID_1_CONTROL_BYTE,            0x20,         -1,         100},
  {TX_EXAMPLE12_MESSAGE,                0x18FFFC5Aul,         CAN_ID_1_CONTROL_BYTE,            0x20,         -1,         100},
  {TX_EXAMPLE13_MESSAGE,                0x18FFFC44ul,         CAN_ID_2_CONTROL_BYTES,           0x21,         0x22,       100},
  {TX_EXAMPLE14_MESSAGE,                0x18FFFC42ul,         CAN_ID_2_CONTROL_BYTES,           0x21,         0x22,       100},
  {TX_EXAMPLE15_MESSAGE,                0x18FFFC46ul,         CAN_ID_2_CONTROL_BYTES,           0x21,         0x22,       100},
  {TX_EXAMPLE16_MESSAGE,                0x18FFFC49ul,         CAN_ID_2_CONTROL_BYTES,           0x21,         0x22,       100},
  {TX_EXAMPLE17_MESSAGE,                0x18FFFC4Cul,         CAN_ID_2_CONTROL_BYTES,           0x21,         0x22,       100},
  #endif


};

const unsigned int CAN_Transmit_Config_Array_Size = sizeof(Transmit_CAN_Config) / sizeof(Transmit_CAN_Config[0]);

struct CAN_Transmit_Data_Struct transmit_data[CAN_Transmit_Config_Array_Size];


