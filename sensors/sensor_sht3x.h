#ifndef SENSOR_SHT3X_H
#define SENSOR_SHT3X_H

#include <stdio.h>
#include "shell.h"
#include "sht3x.h"
#include <stdlib.h>

#ifndef SHT3X_PARAM_MODE
#define SHT3X_PARAM_MODE        (sht3x_periodic_05mps)
#endif
#ifndef SHT3X_PARAM_REPEAT
#define SHT3X_PARAM_REPEAT      (sht3x_low)
#endif

#include "sht3x_params.h"
#include "periph/gpio.h"

int init_sht3x(sht3x_dev_t* dev);
int read_sht3x(sht3x_dev_t* dev, int16_t* temp, int16_t* hum);
int read_alert_sht3x(sht3x_dev_t* dev, int limit);
int set_alert_sht3x(sht3x_dev_t* dev, int limit, int humidity, int temperature);
void configure_PB15(void);

#endif