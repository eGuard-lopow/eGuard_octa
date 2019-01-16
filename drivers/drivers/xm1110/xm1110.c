#include "xm1110.h"
#include "xm1110_internal.h"
#include "xm1110_params.h"

#include "log.h"
#include "assert.h"
#include "stdlib.h"

#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

#include "debug.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "xtimer.h"

#define BUS         (dev->p.i2c_bus)
#define ADDR        (dev->p.i2c_addr)
#define REG_R       (dev->p.r_addr)
#define REG_W       (dev->p.w_addr)

// #define BUS         I2C_DEV(1)
// #define ADDR        (0x10)
// #define REG_R       (0x21)

int xm1110_init(xm1110_t *dev, const xm1110_params_t *params) {
    // uint8_t tmp = 0;

    /* check parameters */
    assert(dev && params);

    /* initialize the device descriptor */
    memcpy(&dev->p, params, sizeof(xm1110_params_t));

    // /* setup the I2C bus */
    // i2c_acquire(BUS);

    // i2c_read_reg(BUS, ADR, TCS37727_ID, &tmp, 0);
    // if (tmp == 0) {
    //     i2c_release(BUS);
    //     LOG_ERROR("[tcs37727] init: error while reading ID register\n");
    //     return XM1110_NO_DEV;
    // }

    return XM1110_OK;
}

void xm1110_set_gps_active(const xm1110_t *dev);

void xm1110_set_gps_standby(const xm1110_t *dev);

int xm1110_read(const xm1110_t *dev, xm1110_data_t *xmdata) {

    assert(dev && xmdata);
    int res;
    i2c_acquire(BUS);
    for(int i = 0; i<255; i++) {

        // res = i2c_read_reg(BUS, ADDR, REG_R, &xmdata->data[i], 0);
        res = i2c_read_byte(BUS, ADDR, &xmdata->data[i], 0);


        // printf("%d\n", xmdata->data[i]);

        if( res != 0) {
            printf("\n\nError int: %d\n", res);
        }
    }
    i2c_release(BUS);

    // check error message
    if( res==-EIO ){
        printf("EIO \n");
    } else if( res==-ENXIO ) {
        printf("ENXIO \n");
    } else if( res==-ETIMEDOUT ) {
        printf("ETIMEDOUT \n");
    } else if( res==-EINVAL ) {
        printf("EINVAL \n");
    } else if( res==-EOPNOTSUPP ) {
        printf("EOPNOTSUPP \n");
    } else if( res==-EAGAIN ) {
        printf("EAGAIN \n");
    }

    return 0;
}

bool check_xm1110_ready(const xm1110_t *dev) {
    assert(dev);
    int dataCheck;
    int res;
    i2c_acquire(BUS);
    res = i2c_read_reg(BUS, ADDR, REG_R, &dataCheck, 0);
    i2c_release(BUS);

    if( res != 0) {
        printf("\n\nError int: %d\n", res);
    }

    if(dataCheck==XM1110_NO_DATA) {
        return false;
    } else {
        return true;
    }
}
