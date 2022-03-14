//


  struct CAN_Receive_Data_Struct
  {
    unsigned char updated_flag;
    unsigned char data_buffer[8];
  };

  enum receive_CAN_message_names
  {
     RCV_PAYLOAD_MESSAGE,
     RCV_LAST_MESSAGE,
  };
  
  enum message_type
  {
    CAN_ID_ONLY,
    CAN_ID_1_CONTROL_BYTE,
    CAN_ID_2_CONTROL_BYTES,
  };
  
  struct CAN_Receive_Config_Struct
  {
    enum receive_CAN_message_names index;
    unsigned long CAN_ID;
    enum message_type type;
    int command_byte_1;
    int command_byte_2;
  };



/* To add a message:
 *  1. Add the CAN message to the enum can_message_names list see above
 *  2. Add the relevant configuration information to the Receive_CAN_Config array
 *  
 *  NOTE: the enum list and config array need to be in the same order!!!!  If you remove a message make sure it is removed in both spots otherwise the request data API's will be messed up.
 *  
 */

const CAN_Receive_Config_Struct Receive_CAN_Config[] =  
{
  /*  Index: This enumeration is used in the CAN receive data API
   *   29-bit ID: The 29-bit CAN identifier
   *   type: Type of message being received (i.e. does it have control bytes for additional filtering
   *   Control Byte 1,2: The control bytes if used, if not set the value to -1
   */
//  Index                       29-bit ID            type,                      Control Byte 1          Control Byte 2
  { RCV_PAYLOAD_MESSAGE,           0x18efff21ul,        CAN_ID_ONLY,               0xFF,                     0xFF},            
};

const unsigned int CAN_Receive_Config_Array_Size = sizeof(Receive_CAN_Config) / sizeof(Receive_CAN_Config[0]);

struct CAN_Receive_Data_Struct CAN_Receive_Data[CAN_Receive_Config_Array_Size];

