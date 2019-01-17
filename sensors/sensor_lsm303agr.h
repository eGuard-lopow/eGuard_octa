#ifndef SENSOR_LSM303AGR_H
#define SENSOR_LSM303AGR_H

#include <stdio.h>
#include "shell.h"
#include <stdlib.h>

#include "../config.h"
#include "lsm303agr_params.h"
#include "periph/gpio.h"

int init_lsm303agr(LSM303AGR_t* dev, int cm);
int read_lsm303agr(LSM303AGR_t* dev);

#endif