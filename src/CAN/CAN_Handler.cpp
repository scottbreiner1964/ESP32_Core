#include <GlobalDefinitions.h>
#ifdef CAN_HANDLER
#include <string.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <CAN_Receive_Config.h>
#include <CAN_Transmit_Config.h>

#define CAN_RECEIVE_MESSAGE_TASK_RATE 5 // task rate in milliseconds
#define CAN_TRANSMIT_TASK_RATE 5 // task rate in milliseconds
#define CAN_SPEED CAN_SPEED_500KBPS // CAN_SPEED_250KBPS
#define TX_MAX_QUEUE_SIZE 50 

void CAN_Driver_Setup_Init( void );
int CAN_Data_Request_API(enum receive_CAN_message_names CAN_Index, unsigned char *data_buffer, int *update_flag);
int CAN_Transmit_Manual_Transmit_Request_API (enum transmit_CAN_message_names CAN_Index);
int CAN_Transmit_Data_Load_API (enum transmit_CAN_message_names CAN_Index, unsigned char *data_buffer);
int CAN_QueueTransmitMessage(int index);

CAN_device_t CAN_cfg;               // CAN Config
unsigned long previousMillis = 0;   // will store last time a CAN Message was send
const int interval = 1000;          // interval at which send CAN Messages (milliseconds)
const int rx_queue_size = 100;       // Receive Queue size
const int tx_queue_size = 100;

void CAN_Driver_Setup_Init( void )
{
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_25;
 // CAN_cfg.rx_pin_id = GPIO_NUM_17;
  CAN_cfg.rx_pin_id = GPIO_NUM_26;
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
  CAN_cfg.tx_queue = xQueueCreate(tx_queue_size, sizeof(CAN_frame_t));
  // Init CAN Module
  ESP32Can.CANInit();
}

void CAN_Receive_Message_Task (void * parameter)
{
  CAN_frame_t rx_frame;
  int i;
  static char init_task_ran;

  while (1)
  {

    if (init_task_ran == FALSE)
    {
      CAN_Driver_Setup_Init();
      init_task_ran = TRUE;
    }

    while (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0))
    {
      for (i=0; i<CAN_Receive_Config_Array_Size; i++)
      {
        // No command byte message type
        if (Receive_CAN_Config[i].type == CAN_ID_ONLY)
        {
          if (rx_frame.MsgID == Receive_CAN_Config[i].CAN_ID)
          {
            vTaskSuspendAll();
            CAN_Receive_Data[i].updated_flag = TRUE;
            memcpy(&(CAN_Receive_Data[i].data_buffer),&(rx_frame.data.u8),8);
            xTaskResumeAll();
            break;
          }
        }

        // 1 command byte message type
        if (Receive_CAN_Config[i].type == CAN_ID_1_CONTROL_BYTE)
        {
          if ((rx_frame.MsgID == Receive_CAN_Config[i].CAN_ID) && (Receive_CAN_Config[i].command_byte_1 == rx_frame.data.u8[0]))
          {
            vTaskSuspendAll();
            CAN_Receive_Data[i].updated_flag = TRUE;
            memcpy(&(CAN_Receive_Data[i].data_buffer),&(rx_frame.data.u8),8);
            xTaskResumeAll();
            break;
          }
        }

        // 2 command byte message type
        if (Receive_CAN_Config[i].type == CAN_ID_2_CONTROL_BYTES)
        {
          if ((rx_frame.MsgID == Receive_CAN_Config[i].CAN_ID) && (Receive_CAN_Config[i].command_byte_1 == rx_frame.data.u8[0]) && (Receive_CAN_Config[i].command_byte_2 == rx_frame.data.u8[1]))
          {
            vTaskSuspendAll();
            CAN_Receive_Data[i].updated_flag = TRUE;
            memcpy(&(CAN_Receive_Data[i].data_buffer),&(rx_frame.data.u8),8);
            xTaskResumeAll();
            break;
          }
        }
      }
    }
    vTaskDelay(CAN_RECEIVE_MESSAGE_TASK_RATE/portTICK_PERIOD_MS);  
  }
}

int CAN_Data_Request_API(enum receive_CAN_message_names CAN_Index, unsigned char *data_buffer, int *update_flag)
{
  int rtn; 
  if (CAN_Index < CAN_Receive_Config_Array_Size)  // insure the CAN index is in bounds
  {
    vTaskSuspendAll();
    *update_flag = CAN_Receive_Data[CAN_Index].updated_flag;
    CAN_Receive_Data[CAN_Index].updated_flag = FALSE; // reset the flag
    memcpy(data_buffer, CAN_Receive_Data[CAN_Index].data_buffer,8);
    xTaskResumeAll();
    rtn = 1;
  }
  else
  {
    rtn = 0;
  }
  return (rtn);
}

void CAN_Transmit_Message_Task( void * parameter )
{ 
  CAN_frame_t tx_frame;
  unsigned int i;
  int transmit_flag;
  
  while(1)
  { 

    for (i=0; i<CAN_Transmit_Config_Array_Size; i++)
    {
      if (Transmit_CAN_Config[i].transmit_rate != MANUAL_TRANSMIT)
      {
        if (transmit_data[i].transmit_timer <= Transmit_CAN_Config[i].transmit_rate)
        {
          transmit_data[i].transmit_timer += 5; 
        }
      }

      if (transmit_data[i].transmit_timer >= Transmit_CAN_Config[i].transmit_rate) 
      {
        if (uxQueueMessagesWaiting(CAN_cfg.tx_queue) < TX_MAX_QUEUE_SIZE )
        {
          transmit_flag = TRUE; 
        } 
        else
        {
          transmit_flag = FALSE; 
        }
      }
      else
      {
        transmit_flag = FALSE;
      }
      
      if (transmit_flag == TRUE)
      {
        CAN_QueueTransmitMessage(i);
      }
    }
    
    if (uxQueueMessagesWaiting(CAN_cfg.tx_queue))
    {
      if (xQueueReceive(CAN_cfg.tx_queue,&tx_frame,0))
      {
        ESP32Can.CANWriteFrame(&tx_frame);  // note only 1 call to this function is needed because I modified the CAN library such that continous CAN transmits will occur because the ISR routine will
                                            // check the transmit queue and if there are messages to be transmitted it just keep going until its empty in the background of the application code
      }
    }
    
    vTaskDelay(CAN_TRANSMIT_TASK_RATE/portTICK_PERIOD_MS);
  }
}

int CAN_Transmit_Manual_Transmit_Request_API (enum transmit_CAN_message_names CAN_Index)
{
  int rtn;

  if (CAN_Index < CAN_Transmit_Config_Array_Size)
  { 
    transmit_data[CAN_Index].transmit_timer = MANUAL_TRANSMIT;
    rtn = 1;
  } 
  else
  {
    rtn = 0;
  }
  return(rtn);
}

int CAN_Transmit_Data_Load_API (enum transmit_CAN_message_names CAN_Index, unsigned char *data_buffer)
{
  int rtn;

  if (CAN_Index < CAN_Transmit_Config_Array_Size)
  {
    vTaskSuspendAll();
    memcpy(transmit_data[CAN_Index].data,data_buffer,8);  
    xTaskResumeAll();
    rtn = 1;
  } 
  else
  {
    rtn = 0;
  }
  return (rtn);
}

int CAN_QueueTransmitMessage(int index)
{
    CAN_frame_t tx_frame;
    int rtn = 0;
    
    // No command byte message type
    if (Transmit_CAN_Config[index].type == CAN_ID_ONLY)
    {
      tx_frame.FIR.B.FF = CAN_frame_ext;
      tx_frame.MsgID = Transmit_CAN_Config[index].CAN_ID;
      tx_frame.FIR.B.DLC = 8;
      tx_frame.data.u8[0] = transmit_data[index].data[0];
      tx_frame.data.u8[1] = transmit_data[index].data[1];
      tx_frame.data.u8[2] = transmit_data[index].data[2];
      tx_frame.data.u8[3] = transmit_data[index].data[3];
      tx_frame.data.u8[4] = transmit_data[index].data[4];
      tx_frame.data.u8[5] = transmit_data[index].data[5];
      tx_frame.data.u8[6] = transmit_data[index].data[6];
      tx_frame.data.u8[7] = transmit_data[index].data[7];
      if (xQueueSendToBack(CAN_cfg.tx_queue, &tx_frame, 0))
      {
        transmit_data[index].transmit_timer = 0;    
        rtn = 1;
      }
      else
      {
        rtn = 0;
      }
    }

    // 1 command byte message type
    if (Transmit_CAN_Config[index].type == CAN_ID_1_CONTROL_BYTE)
    {
      tx_frame.FIR.B.FF = CAN_frame_ext;
      tx_frame.MsgID = Transmit_CAN_Config[index].CAN_ID;
      tx_frame.FIR.B.DLC = 8;
      tx_frame.data.u8[0] = Transmit_CAN_Config[index].command_byte_1;
      tx_frame.data.u8[1] = transmit_data[index].data[1];
      tx_frame.data.u8[2] = transmit_data[index].data[2];
      tx_frame.data.u8[3] = transmit_data[index].data[3];
      tx_frame.data.u8[4] = transmit_data[index].data[4];
      tx_frame.data.u8[5] = transmit_data[index].data[5];
      tx_frame.data.u8[6] = transmit_data[index].data[6];
      tx_frame.data.u8[7] = transmit_data[index].data[7];
      if (xQueueSendToBack(CAN_cfg.tx_queue, &tx_frame, 0))
      {
        transmit_data[index].transmit_timer = 0;    
        rtn = 1;
      }
      else
      {
        rtn = 0;
      }
    }

    // 2 command byte message type
    if (Transmit_CAN_Config[index].type == CAN_ID_2_CONTROL_BYTES)
    {
      tx_frame.FIR.B.FF = CAN_frame_ext;
      tx_frame.MsgID = Transmit_CAN_Config[index].CAN_ID;
      tx_frame.FIR.B.DLC = 8;
      tx_frame.data.u8[0] = Transmit_CAN_Config[index].command_byte_1;
      tx_frame.data.u8[1] = Transmit_CAN_Config[index].command_byte_2;
      tx_frame.data.u8[2] = transmit_data[index].data[2];
      tx_frame.data.u8[3] = transmit_data[index].data[3];
      tx_frame.data.u8[4] = transmit_data[index].data[4];
      tx_frame.data.u8[5] = transmit_data[index].data[5];
      tx_frame.data.u8[6] = transmit_data[index].data[6];
      tx_frame.data.u8[7] = transmit_data[index].data[7];
      if (xQueueSendToBack(CAN_cfg.tx_queue, &tx_frame, 0))
      {
        transmit_data[index].transmit_timer = 0;    
        rtn = 1;
      }
      else
      {
        rtn = 0;
      }
    }

    return (rtn);
}
#endif

