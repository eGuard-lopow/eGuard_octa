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

#include "sensors/sensor_sht3x.h"
#include "sensors/sensor_lsm303agr.h"

#include "modem.h"

#define INTERVAL (20U * US_PER_SEC)

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

int main(void)
{
   printf("+------------Initializing------------+\n");
  // ------------------------------
  // Initialize SHT3x
  // ------------------------------
  sht3x_dev_t dev;
  init_sht3x(&dev); 

  // ------------------------------
  // Initialize LSM303AGR
  // ------------------------------
  LSM303AGR_t lsm;
  init_lsm303agr(&lsm, 35);
  Configure_Interrupt_lsm303agr();


  // ------------------------------
  // LoRa / D7 example
  // ------------------------------
    puts("Welcome to RIOT!");

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
    alp_itf_id_t current_interface_id = ALP_ITF_ID_D7ASP;
    void* current_interface_config = (void*)&d7_session_config;
    uint8_t counter = 0;
    uint8_t data[4];
    while(1) {
      // ------------------------------
      // Change communication protocol
      // ------------------------------
      // counter = 1; // only use dash-7
      uint32_t start = xtimer_now_usec();
      if(counter % 5 == 0) {
        printf("\n");
        if(current_interface_id == ALP_ITF_ID_D7ASP) {
          printf("Switching to LoRaWAN\n");
          current_interface_id = ALP_ITF_ID_LORAWAN_ABP;
          current_interface_config = &lorawan_session_config;
        } else {
          //printf("Switching to D7AP\n");
          //current_interface_id = ALP_ITF_ID_D7ASP;
          //current_interface_config = &d7_session_config;
        }
      }
      // counter = 1; // only use lora

      // ------------------------------
      // Perform Measurement
      // ------------------------------
      int16_t temp;
      int16_t hum;
      read_sht3x(&dev, &temp, &hum);
      data[0] = temp & 0xFF;
      data[1] = temp >> 8;
      data[2] = hum & 0xFF;
      data[3] = hum >> 8;

      // ------------------------------
      // Transmit Data
      // ------------------------------
      //while(true) { //this loop is for testing
      printf("Sending msg with data [ %i, %i, %i, %i ]\n", data[0], data[1], data[2], data[3]);
      modem_status_t status = modem_send_unsolicited_response(0x40, 0, 4, &data[0], current_interface_id, current_interface_config);
      uint32_t duration_usec = xtimer_now_usec() - start;
      printf("Command completed in %li ms\n", duration_usec / 1000);
      if(status == MODEM_STATUS_COMMAND_COMPLETED_SUCCESS) {
        printf("Command completed successfully\n");
      } else if(status == MODEM_STATUS_COMMAND_COMPLETED_ERROR) {
        printf("Command completed with error\n");
      } else if(status == MODEM_STATUS_COMMAND_TIMEOUT) {
        printf("Command timed out\n");
      }

      counter++;
      xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
      printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
      printf("------------------------------\n");
    
    }

    return 0;
}
