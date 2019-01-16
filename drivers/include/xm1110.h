#ifndef XM1110_H
#define XM1110_H

#include <stdint.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef XM1110_I2C_ADDRESS
#define XM1110_I2C_ADDRESS      0x10    /**< Default Device Address */
#endif

#ifndef XM1110_REG_ADDRESS_R
#define XM1110_REG_ADDRESS_R    0x21    /**< Register address include Read bit */
#endif

#ifndef XM1110_REG_ADDRESS_W
#define XM1110_REG_ADDRESS_W    0x20    /**< Register address include Write bit */
#endif

#ifndef XM1110_OK
#define XM1110_OK               0       /**< return value */
#endif

#ifndef XM1110_NO_DEV
#define XM1110_NO_DEV           1       /**< return value */
#endif

#ifndef XM1110_NO_DATA
#define XM1110_NO_DATA          0x0A    /**< Value when I2C buffer has no data */
#endif

typedef struct {
    char data[255];
} xm1110_data_t;

typedef struct {
    i2c_t i2c_bus;              /**< I2C bus the sensor is connected to     */
    uint8_t i2c_addr;           /**< slave address */
    uint8_t r_addr;             /**< the sensors register address on the I2C bus     */
    uint8_t w_addr;             /**< the sensors register address on the I2C bus     */
} xm1110_params_t;

typedef struct {
    xm1110_params_t p;    /**< device configuration */
    int again;              /**< amount of gain */
} xm1110_t;



int xm1110_init(xm1110_t *dev, const xm1110_params_t *params);

void xm1110_set_gps_active(const xm1110_t *dev);

void xm1110_set_gps_standby(const xm1110_t *dev);

int xm1110_read(const xm1110_t *dev, xm1110_data_t *xmdata);

bool check_xm1110_ready(const xm1110_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* XM1110_H */
/** @} */
