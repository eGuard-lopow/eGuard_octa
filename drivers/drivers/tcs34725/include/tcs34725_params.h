/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tcs37727
 *
 * @{
 * @file
 * @brief       Default configuration for TCS34725 devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef TCS34725_PARAMS_H
#define TCS34725_PARAMS_H

#include "board.h"
#include "tcs34725.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Set default configuration parameters for TCS34725 devices
 * @{
 */
#ifndef TCS34725_PARAM_I2C
#define TCS34725_PARAM_I2C          I2C_DEV(0)
#endif
#ifndef TCS34725_PARAM_ADDR
#define TCS34725_PARAM_ADDR         (TCS34725_I2C_ADDRESS)
#endif
#ifndef TCS34725_PARAM_ATIME
#define TCS34725_PARAM_ATIME        (TCS34725_ATIME_DEFAULT)
#endif

#ifndef TCS34725_PARAMS
#define TCS34725_PARAMS             { .i2c   = TCS34725_PARAM_I2C,  \
                                      .addr  = TCS34725_PARAM_ADDR, \
                                      .atime = TCS34725_PARAM_ATIME }
#endif
#ifndef TCS34725_SAUL_INFO
#define TCS34725_SAUL_INFO          { .name = "tcs34725" }
#endif
/**@}*/

/**
 * @brief   TCS34725 configuration
 */
static const tcs34725_params_t tcs34725_params[] =
{
    TCS34725_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t tcs34725_saul_info[] =
{
    TCS34725_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* TCS34725_PARAMS_H */
/** @} */
