#include <GlobalDefinitions.h>
#ifdef CAN_BRIDGE_WIRELESS

#include <ESP32CAN.h>
#include <CAN_config.h>
#include <esp_now.h>

#define CAN_RECEIVE_MESSAGE_TASK_RATE 5 // task rate in milliseconds
#define CAN_SPEED CAN_SPEED_500KBPS            // CAN_SPEED_250KBPS
#define MAX_QUEUE_SIZE 50                      // assuming a 5ms task rate for receive and transmit at 500KBPS a maximum of 20 messages can be received and/or transmitted in a 5ms window.
#define MAX_NUM_MESSAGE_PER_ESPNOW_TRANSFER 15 // to stay under 250 bytes only 15 messages max can be bridged per espnow transfer call...each message is 16-byte (at 5 ms task rate the bridge can handle ~75% bus load)
#define SEND_KICKED_OFF 25                     // arbitrary number other than 0 and 1 which are designated as success and fail by the esp-idf

CAN_device_t CAN_cfg; // CAN Config
uint8_t esp_transfer_array[250];
int msg_count;

extern uint8_t espTranferComplete;


void CAN_Driver_Setup_Init( void )
{
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_25;
  CAN_cfg.rx_pin_id = GPIO_NUM_26;
  CAN_cfg.tx_queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(CAN_frame_t));
  CAN_cfg.rx_queue = xQueueCreate(MAX_QUEUE_SIZE, sizeof(CAN_frame_t));
  // Init CAN Module
  ESP32Can.CANInit();
  espTranferComplete = TRUE;
}

void CAN_Wireless_Bridge_Receive_Message_Task (void * parameter)
{
  CAN_frame_t rx_frame;
  static char init_task_ran;
  uint8_t i, num_queue_items;
 
  while (1)
  {
    if (init_task_ran == FALSE)
    {
      CAN_Driver_Setup_Init();
      init_task_ran = TRUE;
    }

    if (1)
    {
      espTranferComplete = FALSE;
    
      num_queue_items = MAX_QUEUE_SIZE - uxQueueSpacesAvailable(CAN_cfg.rx_queue);  // total items in the queue
    // Serial.println(num_queue_items);

      if (num_queue_items != 0) // there are CAN messages in the receive queue so start transmitting over ESP_NOW
      {
        msg_count = 0;
        if (num_queue_items <= MAX_NUM_MESSAGE_PER_ESPNOW_TRANSFER)
        {
          // the number of items in the CAN receive queue can be transferred in a single ESP_NOW message packet
          for (i = 0; i < num_queue_items; i++)
          {
            xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0);
            memcpy(&(esp_transfer_array[msg_count * sizeof(rx_frame)]), (uint8_t *)&rx_frame, sizeof(rx_frame));
            msg_count++;
          }
          esp_now_send(0, (uint8_t *)&esp_transfer_array, msg_count * sizeof(rx_frame));
        }
        else
        {
          // the number of items in the CAN receive queue cannot be transferred in a single ESP_NOW message packet
          for (i = 0; i < MAX_NUM_MESSAGE_PER_ESPNOW_TRANSFER; i++)
          {
            xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0);
            memcpy(&(esp_transfer_array[msg_count * sizeof(rx_frame)]), (uint8_t *)&rx_frame, sizeof(rx_frame));
            msg_count++;
          }
          esp_now_send(0, (uint8_t *)&esp_transfer_array, msg_count * sizeof(rx_frame));
        }
      }
    }
    vTaskDelay(CAN_RECEIVE_MESSAGE_TASK_RATE/portTICK_PERIOD_MS);  
  }
}
#endif

