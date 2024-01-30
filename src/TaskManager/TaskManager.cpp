#include <GlobalDefinitions.h>

void TasklInitiator(void);

#ifdef CAN_HANDLER
  extern void CAN_Receive_Message_Task( void * parameter );
  extern void CAN_Transmit_Message_Task( void * parameter );
#endif
#ifdef CAN_BRIDGE_WIRELESS
  extern void ESP_NOW_Task( void * parameter );
  extern void CAN_Wireless_Bridge_Receive_Message_Task (void * parameter);
#endif 

void TaskInitiator (void)
{
//                          Function                            Name of the task                    stack size              task input          priority (1-7(high))    Task Handle         Core (0 or 1)                             
   #ifdef CAN_HANDLER                       
    xTaskCreatePinnedToCore(CAN_Receive_Message_Task,           "CAN_Receive_Message_Task",         1000,                   NULL,               7,                      NULL,               0);                 
    xTaskCreatePinnedToCore(CAN_Transmit_Message_Task,          "CAN_Transmit_Message_Task",        1000,                   NULL,               7,                      NULL,               0);                            
  #endif 
  #ifdef CAN_BRIDGE_WIRELESS
    xTaskCreatePinnedToCore(ESP_NOW_Task,                       "ESP_NOW_Task",                     3000,                   NULL,               7,                      NULL,               0);                 
    xTaskCreatePinnedToCore(CAN_Wireless_Bridge_Receive_Message_Task,                "CAN_Wireless_Bridge_Receive_Message_Task",                     3000,                   NULL,               5,                      NULL,              0);                 
  #endif
}