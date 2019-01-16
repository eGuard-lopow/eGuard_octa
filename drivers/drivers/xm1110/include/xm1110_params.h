/*
 * Copyright (C) 2018 Toon De Keyzer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_xm1110
 *
 * @{
 *
 * @file
 * @brief       Definitions for the XM1110 GPS AirPrime
 *
 * @author      Toon De Keyzer <toondk@gmail.com>
 */

#ifndef XM1110_PARAMS_H
#define XM1110_PARAMS_H

#include "board.h"
#include "xm1110.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters
 * @{
 */
#ifndef XM1110_PARAM_I2C
#define XM1110_PARAM_I2C            I2C_DEV(1)
#endif
#ifndef XM1110_PARAM_I2C_ADDR
#define XM1110_PARAM_I2C_ADDR       (0x10)
#endif
#ifndef XM1110_PARAM_R_ADDR
#define XM1110_PARAM_R_ADDR         (0x21)
#endif
#ifndef XM1110_PARAM_W_ADDR
#define XM1110_PARAM_W_ADDR         (0x20)
#endif

// #ifndef XM1110_PARAM_GPS_PIN
// #define XM1110_PARAM_GPS_PIN        (GPIO_PIN(0, 0))
// #endif


#ifndef XM1110_PARAMS
#define XM1110_PARAMS                   { .i2c_bus   = XM1110_PARAM_I2C,        \
                                          .i2c_addr  = XM1110_PARAM_I2C_ADDR,   \
                                          .r_addr    = XM1110_PARAM_R_ADDR,    \
                                          .w_addr    = XM1110_PARAM_W_ADDR }
#endif
#ifndef XM1110_SAUL_INFO
#define XM1110_SAUL_INFO                { .name = "xm1110" }
#endif
/**@}*/

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const xm1110_params_t xm1110_params[] =
{
    XM1110_PARAMS
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t xm1110_saul_info[] =
{
    XM1110_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* XM1110_PARAMS_H */
/** @} */
