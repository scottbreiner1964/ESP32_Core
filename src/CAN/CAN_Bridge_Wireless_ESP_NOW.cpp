#include <GlobalDefinitions.h>
#ifdef CAN_BRIDGE_WIRELESS

#include <esp_now.h>
//#include <esp_wifi_internal.h>
#include "esp_private/wifi.h"
#include <WiFi.h>
#include <ESP32CAN.h>

#define ESP_NOW_TASK_RATE 5 // task rate in milliseconds
#define CHANNEL 9
#define DATARATE WIFI_PHY_RATE_5M_L

 //#define RICHOCHET

uint8_t espTranferComplete;
// Set the broadcastAddress to the node your transmitting to
uint8_t broadcastAddress1[] = {0x58, 0xBF, 0x25, 0x92, 0xF5, 0x98};  
uint8_t broadcastAddress2[] = {0x08, 0x3a, 0xf2, 0x7d, 0x2c, 0x54}; 

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  espTranferComplete = TRUE;
}

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  uint8_t i;
  CAN_frame_t tx_frame;
  CAN_frame_t *msg_ptr;
  int msg_count;
  //richochet the data right back upon reception FOR TESTING ONLY
 
  #ifdef RICHOCHET
  int transmit_length;
  uint8_t transmit_data[250];
  
  memcpy(&(transmit_data), incomingData, len);
  transmit_length = len;
  esp_now_send(0, (uint8_t *) &transmit_data, transmit_length);
  
  #else
  msg_count = len / sizeof(tx_frame);
  msg_ptr = (CAN_frame_t *)incomingData;
  for (i = 0; i < msg_count; i++, msg_ptr++)
  {
    memcpy(&tx_frame, msg_ptr, sizeof(tx_frame));

    if (xQueueSendToBack(CAN_cfg.tx_queue, &tx_frame, 0))
    {
      // still space in queue
    }
    else
    {
      break;
    }
  }

  xQueueReceive(CAN_cfg.tx_queue,&tx_frame,0);
  ESP32Can.CANWriteFrame(&tx_frame);  // note only 1 call to this function is needed because I modified the CAN library such that continous CAN transmits will occur because the ISR routine will
    // check the transmit queue and if there are messages to be transmitted it just keep going until its empty in the background of the application code 
    #endif
}

void ESP_NOW_Init(void)
{
  //Serial.print("ESP Board MAC Address:  ");
  //Serial.println(WiFi.macAddress());

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  /*Stop wifi to change config parameters*/
  esp_wifi_stop();
  esp_wifi_deinit();

  /*Disabling AMPDU is necessary to set a fix rate*/
  wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT(); //We use the default config ...
  my_config.ampdu_tx_enable = 0;                             //... and modify only what we want.
  esp_wifi_init(&my_config);                                 //set the new config

  esp_wifi_start();

  /*You'll see that in the next subsection ;)*/
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);

  /*set the rate*/
  esp_wifi_internal_set_fix_rate(WIFI_IF_STA, true, DATARATE);

  esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR);

  if (esp_now_init() != ESP_OK)
  {
   // Serial.println ("Init Not OK");
  }
  else
  {

  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // register first peer
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  //if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    //
  }
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    //
  }
}

void ESP_NOW_Task( void * parameter )
{
  static char init_task_ran;

  while (1)
  {
    if (init_task_ran == FALSE)
    {
      ESP_NOW_Init();
      init_task_ran = TRUE;
    }
    
    vTaskDelete(NULL);  // we shouldn't need this task anymore so blow it away
  }
}


#endif
