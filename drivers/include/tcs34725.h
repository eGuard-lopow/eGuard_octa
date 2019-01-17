/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tcs34725 TCS34725 RGB Light Sensor
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the AMS TCS34725 Color Light-To-Digital Converter
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Interface definition for the TCS34725 sensor driver.
 *
 * @author      Felix Siebel <f.siebel@phytec.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef TCS34725_H
#define TCS34725_H

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TCS34725_I2C_ADDRESS
#define TCS34725_I2C_ADDRESS    0x29    /**< Default Device Address */
#endif

#ifndef TCS34725_ATIME_DEFAULT
#define TCS34725_ATIME_DEFAULT  200000  /**< Default RGBC integration time */
#endif

/**
 * @brief   Struct for storing TCS34725 sensor data
 */
typedef struct {
    uint32_t red;           /**< IR compensated channels red */
    uint32_t green;         /**< IR compensated channels green */
    uint32_t blue;          /**< IR compensated channels blue */
    uint32_t clear;         /**< channels clear */
    uint32_t lux;           /**< Lux */
    uint32_t ct;            /**< Color temperature */
} tcs34725_data_t;

/**
 * @brief   TCS34725 configuration parameters
 */
typedef struct {
    i2c_t i2c;              /**< I2C bus the sensor is connected to */
    uint8_t addr;           /**< the sensors address on the I2C bus */
    uint32_t atime;         /**< conversion time in microseconds */
} tcs34725_params_t;

/**
 * @brief   Device descriptor for TCS34725 sensors
 */
typedef struct {
    tcs34725_params_t p;    /**< device configuration */
    int again;              /**< amount of gain */
} tcs34725_t;

/**
 * @brief   Possible TCS27737 return values
 */
enum {
    TCS34725_OK     =  0,   /**< everything worked as expected */
    TCS34725_NOBUS  = -1,   /**< access to the configured I2C bus failed */
    TCS34725_NODEV  = -2    /**< no TCS34725 device found on the bus */
};

/**
 * @brief   Initialize the given TCS34725 sensor
 *
 * The sensor is initialized in RGBC only mode with proximity detection turned
 * off.
 *
 * The gain will be initially set to 4x, but it will be adjusted
 *
 * The gain value will be initially set to 4x, but it will be automatically
 * adjusted during runtime.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       static configuration parameters
 *
 * @return                  TCS27737_OK on success
 * @return                  TCS34725_NOBUS if initialization of I2C bus fails
 * @return                  TCS34725_NODEV if no sensor can be found
 */
int tcs34725_init(tcs34725_t *dev, const tcs34725_params_t *params);

/**
 * @brief   Set RGBC enable, this activates periodic RGBC measurements.
 *
 * @param[out] dev          device descriptor of sensor
 */
void tcs34725_set_rgbc_active(const tcs34725_t *dev);

/**
 * @brief   Set RGBC disable, this deactivates periodic RGBC measurements
 *
 * Also turns off the sensor when proximity measurement is disabled.
 *
 * @param[in]  dev          device descriptor of sensor
 */
void tcs34725_set_rgbc_standby(const tcs34725_t *dev);

/**
 * @brief   Read sensor's data
 *
 * Besides an Autogain routine is called. If a maximum or minimum threshold
 * value of the channel clear is reached, then the gain will be changed
 * correspond to max or min threshold.
 *
 * @param[in]  dev         device descriptor of sensor
 * @param[out] data        device sensor data, MUST not be NULL
 */
void tcs34725_read(const tcs34725_t *dev, tcs34725_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* TCS34725_H */
/** @} */
