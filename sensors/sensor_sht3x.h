#ifndef SENSOR_SHT3X_H
#define SENSOR_SHT3X_H

#include <stdio.h>
#include "shell.h"
#include <stdlib.h>

#include "../config.h"
#include "sht3x_params.h"
#include "periph/gpio.h"

int init_sht3x(sht3x_dev_t* dev);
int read_sht3x(sht3x_dev_t* dev, int16_t* temp, int16_t* hum);
int read_alert_sht3x(sht3x_dev_t* dev, int limit);
int set_alert_sht3x(sht3x_dev_t* dev, int limit, int humidity, int temperature);
void configure_PB15(void);

#endif