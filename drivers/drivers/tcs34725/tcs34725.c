/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_tcs34725
 * @{
 *
 * @file
 * @brief       Driver for the AMS TCS34725 Color Light-To-Digital Converter
 *
 * @author      Felix Siebel <f.siebel@phytec.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "log.h"
#include "assert.h"

#include "tcs34725.h"
#include "tcs34725-internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define BUS             (dev->p.i2c)
#define ADR             (dev->p.addr)

int tcs34725_init(tcs34725_t *dev, const tcs34725_params_t *params)
{
    uint8_t tmp;

    /* check parameters */
    assert(dev && params);

    /* initialize the device descriptor */
    memcpy(&dev->p, params, sizeof(tcs34725_params_t));

    /* setup the I2C bus */
    i2c_acquire(BUS);

    /* check if we can communicate with the device */
    i2c_read_reg(BUS, ADR, TCS34725_ID, &tmp, 0);
    if (tmp != TCS34725_ID_VALUE) {
        i2c_release(BUS);
        LOG_ERROR("[tcs34725] init: error while reading ID register\n");
        return TCS34725_NODEV;
    }

    /* configure gain and conversion time */
    i2c_write_reg(BUS, ADR, TCS34725_ATIME,
                  TCS34725_ATIME_TO_REG(dev->p.atime), 0);
    i2c_write_reg(BUS, ADR, TCS34725_CONTROL, TCS34725_CONTROL_AGAIN_4, 0);
    dev->again = 4;

    /* enable the device */
    tmp = (TCS34725_ENABLE_AEN | TCS34725_ENABLE_PON);
    i2c_write_reg(BUS, ADR, TCS34725_ENABLE, tmp, 0);

    i2c_release(BUS);

    return TCS34725_OK;
}

void tcs34725_set_rgbc_active(const tcs34725_t *dev)
{
    uint8_t reg;

    assert(dev);

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADR, TCS34725_ENABLE, &reg, 0);
    reg |= (TCS34725_ENABLE_AEN | TCS34725_ENABLE_PON);
    i2c_write_reg(BUS, ADR, TCS34725_ENABLE, reg, 0);
    i2c_release(BUS);
}

void tcs34725_set_rgbc_standby(const tcs34725_t *dev)
{
    uint8_t reg;

    assert(dev);

    i2c_acquire(BUS);
    i2c_read_reg(BUS, ADR, TCS34725_ENABLE, &reg, 0);
    reg &= ~TCS34725_ENABLE_AEN;
    if (!(reg & TCS34725_ENABLE_PEN)) {
        reg &= ~TCS34725_ENABLE_PON;
    }
    i2c_write_reg(BUS, ADR, TCS34725_ENABLE, reg, 0);
    i2c_release(BUS);
}

static uint8_t tcs34725_trim_gain(tcs34725_t *dev, int32_t rawc)
{
    uint8_t reg_again = 0;
    int val_again = dev->again;

    if (rawc < TCS34725_AG_THRESHOLD_LOW) {
        switch (val_again) {
            case 1:
                reg_again = TCS34725_CONTROL_AGAIN_4;
                val_again = 4;
                break;

            case 4:
                reg_again = TCS34725_CONTROL_AGAIN_16;
                val_again = 16;
                break;

            case 16:
                reg_again = TCS34725_CONTROL_AGAIN_60;
                val_again = 60;
                break;

            case 60:
            default:
                return -1;
        }
    }
    else if (rawc > TCS34725_AG_THRESHOLD_HIGH) {
        switch (val_again) {
            case 60:
                reg_again = TCS34725_CONTROL_AGAIN_16;
                val_again = 16;
                break;

            case 16:
                reg_again = TCS34725_CONTROL_AGAIN_4;
                val_again = 4;
                break;

            case 4:
                reg_again = TCS34725_CONTROL_AGAIN_1;
                val_again = 1;
                break;

            case 1:
            default:
                return -1;
        }
    }
    else {
        return 0;
    }

    i2c_acquire(BUS);
    uint8_t reg = 0;
    if (i2c_read_reg(BUS, ADR, TCS34725_CONTROL, &reg, 0) < 0) {
        i2c_release(BUS);
        return -2;
    }
    reg &= ~TCS34725_CONTROL_AGAIN_MASK;
    reg |= reg_again;
    if (i2c_write_reg(BUS, ADR, TCS34725_CONTROL, reg, 0) < 0) {
        i2c_release(BUS);
        return -2;
    }
    i2c_release(BUS);
    dev->again = val_again;

    return 0;
}

void tcs34725_read(const tcs34725_t *dev, tcs34725_data_t *data)
{
    uint8_t buf[8];

    assert(dev && data);

    i2c_acquire(BUS);
    i2c_read_regs(BUS, ADR, (TCS34725_INC_TRANS | TCS34725_CDATA), buf, 8, 0);
    i2c_release(BUS);

    int32_t tmpc = ((uint16_t)buf[1] << 8) | buf[0];
    int32_t tmpr = ((uint16_t)buf[3] << 8) | buf[2];
    int32_t tmpg = ((uint16_t)buf[5] << 8) | buf[4];
    int32_t tmpb = ((uint16_t)buf[7] << 8) | buf[6];
    DEBUG("rawr: %"PRIi32" rawg %"PRIi32" rawb %"PRIi32" rawc %"PRIi32"\n",
          tmpr, tmpg, tmpb, tmpc);

    /* Remove IR component as described in the DN40.  */
    int32_t ir = (tmpr + tmpg + tmpb - tmpc) >> 1;
    tmpr -= ir;
    tmpg -= ir;
    tmpb -= ir;

    /* Color temperature calculation as described in the DN40. */
    int32_t ct = (CT_COEF_IF * tmpb) / tmpr + CT_OFFSET_IF;

    /* Lux calculation as described in the DN40.  */
    int32_t gi = R_COEF_IF * tmpr + G_COEF_IF * tmpg + B_COEF_IF * tmpb;
    /* TODO: add Glass Attenuation Factor GA compensation */
    int32_t cpl = (dev->p.atime * dev->again) / DGF_IF;
    int32_t lux = gi / cpl;

    /* Autogain */
    tcs34725_trim_gain((tcs34725_t *)dev, tmpc);

    data->red = (tmpr < 0) ? 0 : (tmpr * 1000) / cpl;
    data->green = (tmpg < 0) ? 0 : (tmpg * 1000) / cpl;
    data->blue = (tmpb < 0) ? 0 : (tmpb * 1000) / cpl;
    data->clear = (tmpb < 0) ? 0 : (tmpc * 1000) / cpl;
    data->lux = (lux < 0) ? 0 : lux;
    data->ct = (ct < 0) ? 0 : ct;
}
