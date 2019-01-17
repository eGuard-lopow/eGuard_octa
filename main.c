/*
This example shows how to use the modem API to interface with a serial OSS-7 modem.
An unsolicited message will be transmitted periodically using the DASH7 interface or the LoRaWAN interface (alternating)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "keys.h"

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "xtimer.h"
#include "errors.h"

#include "xm1110.h"
#include "xm1110_params.h"
#include "minmea.h"

#include "sensors/sensor_sht3x.h"
#include "sensors/sensor_lsm303agr.h"

#include "modem.h"

#define INTERVAL (20U * US_PER_SEC)

uint8_t localization = GPS;
uint8_t data[13];
int16_t temp;
int16_t hum;
bool tempAlert;
uint32_t start;
sht3x_dev_t dev_sht3x;
LSM303AGR_t lsm;
xm1110_t dev_xm1110;
xm1110_data_t xmdata;
uint8_t payload[8];

void on_modem_command_completed_callback(bool with_error)
{
  printf("modem command completed (success = %i)\n", !with_error);
}

void on_modem_return_file_data_callback(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t* output_buffer)
{
  printf("modem return file data file %i offset %li size %li buffer %p\n", file_id, offset, size, output_buffer);
}

void on_modem_write_file_data_callback(uint8_t file_id, uint32_t offset, uint32_t size, uint8_t* output_buffer)
{
  printf("modem write file data file %i offset %li size %li buffer %p\n", file_id, offset, size, output_buffer);
}

static d7ap_session_config_t d7_session_config = {
  .qos = {
    .qos_resp_mode = SESSION_RESP_MODE_ALL,
    .qos_retry_mode = SESSION_RETRY_MODE_NO
  },
  .dormant_timeout = 0,
  .addressee = {
    .ctrl = {
      .nls_method = AES_NONE,
      .id_type = ID_TYPE_NOID
    },
    .access_class = 0x01,
    .id = {0},
  },
};

static lorawan_session_config_abp_t lorawan_session_config = {
  .appSKey = LORAWAN_APP_SESSION_KEY,
  .nwkSKey = LORAWAN_NETW_SESSION_KEY,
  .devAddr = LORAWAN_DEV_ADDR,
  .request_ack = false,
  .network_id = LORAWAN_NETW_ID,
  .application_port = 1
};


void readGPS(xm1110_t* dev, xm1110_data_t* xmdata, uint8_t* payload) {
  // 
  char* token;
  struct minmea_sentence_rmc frame;

  float latitude_float = 0.0;
  float longitude_float = 0.0;

  uint32_t latitude_int;
  uint32_t longitude_int;

  int res = 0;


  // Read GPS coordinates and store them in payload
  for(int i = 0; i<5; i++) {
    res = xm1110_read(dev, xmdata);
    if ( res == 0 ) {
      for(int i = 0;i<255;i++){
        xmdata->data[i] = xmdata->data[i]%256;
        printf("%c", xmdata->data[i]);
      }
    }

    token = strtok (xmdata->data,"\n");
    while (token != NULL) {
      printf("\nGPS:   *Zin*:  %s\n",token);
  
      puts("GPS: START");

      switch (minmea_sentence_id(token, false)) {
        case MINMEA_SENTENCE_RMC: { //$GNRMC
          if (minmea_parse_rmc(&frame, token)) {
            printf("$RMC floating point degree coordinates and speed: (%f,%f) %f\n",
                  minmea_tocoord(&frame.latitude),
                  minmea_tocoord(&frame.longitude),
                  minmea_tofloat(&frame.speed));
            latitude_float = minmea_tofloat(&frame.latitude);
            longitude_float = minmea_tofloat(&frame.longitude);
          }
        } break;

        default: {
          //do nothing
        }
      }
      puts("GPS: STOP");
      
      token = strtok (NULL, "\n");
    }
  }

  // Convert floats in bytes
  // latitude_int = (uint32_t)(test_float1*1000000);
  // longitude_int = (uint32_t)(test_float2*1000000);
  
  latitude_int = (uint32_t)(latitude_float*1000000);
  longitude_int = (uint32_t)(longitude_float*1000000);

  payload[0] = (latitude_int & 0xFF000000) >> 24;
  payload[1] = (latitude_int & 0x00FF0000) >> 16;
  payload[2] = (latitude_int & 0x0000FF00) >> 8;
  payload[3] = (latitude_int & 0x000000FF);

  payload[4] = (longitude_int & 0xFF000000) >> 24;
  payload[5] = (longitude_int & 0x00FF0000) >> 16;
  payload[6] = (longitude_int & 0x0000FF00) >> 8;
  payload[7] = (longitude_int & 0x000000FF);
}

void exampleLightSensor() {
  tcs34725_t dev;
  tcs34725_data_t data;

  puts("TCS34725 RGBC Data; Sensor driver test application\r\n");
  printf("Initializing first configured TCS34725 sensor...\r\n");

  if (tcs34725_init(&dev, &tcs34725_params[0]) == TCS34725_OK) {
    puts("[OK]\r\n");
  }
  else {
    puts("[Failed]\r\n");
    return -1;
  }

  while (1) {
    tcs34725_read(&dev, &data);
    printf("R: %5"PRIu32" G: %5"PRIu32" B: %5"PRIu32" C: %5"PRIu32"\r\n",
        data.red, data.green, data.blue, data.clear);
    printf("CT : %5"PRIu32" Lux: %6"PRIu32" AGAIN: %2d ATIME %"PRIu32"\r\n",
        data.ct, data.lux, dev.again, dev.p.atime);

    xtimer_usleep(SLEEP);
  }

  return 0;
}



void measurementLoop(int loopCounter){
  // ------------------------------
  // Reset parameters/flags
  // ------------------------------
  data[0] = 0;
  tempAlert = false;
  printf("entered measurement loop, loopCounter = %d \n",loopCounter);



  // ------------------------------
  // Periodic temperature measurement
  // ------------------------------
  read_sht3x(&dev_sht3x, &temp, &hum);
  data[1] = temp & 0xFF;
  data[2] = temp >> 8;
  data[3] = hum & 0xFF;
  data[4] = hum >> 8;
  if(temp / 100 > 29){
    tempAlert = true;
    data[0] = data[0] | 5; 
    printf("TEMP ALERT\n");
  }
  if(loopCounter == 255){
    data[0] = data[0] | 3;
    printf("FALL ALLERT\n");
  }
    
  // ------------------------------
  // Perform Measurements
  // ------------------------------
  if(loopCounter == 4 || tempAlert || loopCounter == 255){
    if(localization == GPS){
      readGPS(&dev_xm1110, &xmdata, payload);
      data[5] = payload[0];
      data[6] = payload[1];
      data[7] = payload[2];
      data[8] = payload[3];
      data[9] = payload[4];
      data[10] = payload[5];
      data[11] = payload[6];
      data[12] = payload[7];
    }

    // ------------------------------
    // Transmit Data
    // ------------------------------
    if(localization == GPS){
      modem_status_t status = modem_send_unsolicited_response(0x40, 0, 13, &data[0], ALP_ITF_ID_LORAWAN_ABP, &lorawan_session_config);
      uint32_t duration_usec = xtimer_now_usec() - start;
      printf("Command completed in %li ms\n", duration_usec / 1000);
      if(status == MODEM_STATUS_COMMAND_COMPLETED_SUCCESS) {
        printf("Command completed successfully\n");
      } else if(status == MODEM_STATUS_COMMAND_COMPLETED_ERROR) {
        printf("Command completed with error\n");
      } else if(status == MODEM_STATUS_COMMAND_TIMEOUT) {
        printf("Command timed out\n");
      }
    } else {
      modem_status_t status = modem_send_unsolicited_response(0x40, 0, 5, &data[0], ALP_ITF_ID_D7ASP, &d7_session_config);
      uint32_t duration_usec = xtimer_now_usec() - start;
      printf("Command completed in %li ms\n", duration_usec / 1000);
      if(status == MODEM_STATUS_COMMAND_COMPLETED_SUCCESS) {
        printf("Command completed successfully\n");
      } else if(status == MODEM_STATUS_COMMAND_COMPLETED_ERROR) {
        printf("Command completed with error\n");
      } else if(status == MODEM_STATUS_COMMAND_TIMEOUT) {
        printf("Command timed out\n");
      }
    }
  }
}

void cb_lsm303agr(void *arg)
{
    if (arg != NULL) {
    }

    printf("Fall Detected\n");
    measurementLoop(255);
}

void Configure_Interrupt_lsm303agr(void) {
    gpio_init_int(GPIO_PIN(PORT_B, 13),GPIO_IN,GPIO_RISING, cb_lsm303agr, (void*) 0); //INT_1 from lsm303agr
    gpio_irq_enable(GPIO_PIN(PORT_B, 13));
}

int main(void)
{
  printf("+------------Initializing------------+\n");
  // ------------------------------
  // Initialize SHT3x
  // Initialize LSM303AGR
  // Initialize GPS
  // ------------------------------
  init_sht3x(&dev_sht3x); 
  init_lsm303agr(&lsm, 1);
  Configure_Interrupt_lsm303agr();
  int res;
  if ((res = xm1110_init(&dev_xm1110, &xm1110_params[0])) != XM1110_OK) {
      puts("GPS:Initialization failed\n");
      return XM1110_NO_DEV;
  }
  else {
      puts("GPS:Initialization successful\n");
  }


  // ------------------------------
  // LoRa / D7 initialization
  // ------------------------------
    modem_callbacks_t modem_callbacks = {
      .command_completed_callback = &on_modem_command_completed_callback,
      .return_file_data_callback = &on_modem_return_file_data_callback,
      .write_file_data_callback = &on_modem_write_file_data_callback,
    };

    modem_init(UART_DEV(1), &modem_callbacks);

    uint8_t uid[D7A_FILE_UID_SIZE];
    modem_read_file(D7A_FILE_UID_FILE_ID, 0, D7A_FILE_UID_SIZE, uid);
    printf("modem UID: %02X%02X%02X%02X%02X%02X%02X%02X\n", uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6], uid[7]);

    xtimer_ticks32_t last_wakeup = xtimer_now();
    uint8_t loopCounter = 0;
      // ------------------------------
      // Main loop
      // ------------------------------
    
    while(1) {
      start = xtimer_now_usec();
      measurementLoop(loopCounter);
      loopCounter++;
      if(loopCounter == 5){
        loopCounter = 0;
      }
      xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
    }
  return 0;
}



