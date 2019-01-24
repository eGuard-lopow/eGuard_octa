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

int xm1110_init(xm1110_t *dev, const xm1110_params_t *params) {
    // uint8_t tmp = 0;

    /* check parameters */
    assert(dev && params);

    /* initialize the device descriptor */
    memcpy(&dev->p, params, sizeof(xm1110_params_t));

    return XM1110_OK;
}

void xm1110_set_gps_active(const xm1110_t *dev) {
    assert(dev);
    char randomByte = '%';
    
    i2c_acquire(BUS);
    i2c_write_byte(BUS, ADDR, randomByte, 0);
    i2c_write_reg(BUS, ADDR, REG_W, randomByte, 0);
    i2c_release(BUS);

    printf("\n(GPS) active.");
}

void xm1110_set_gps_standby(const xm1110_t *dev) {
    assert(dev);
    char standby_command[21] = "$PMTK161,0*28\r\n";

    i2c_acquire(BUS);
    for(int i = 0; i<21; i++) {
        i2c_write_byte(BUS, ADDR, standby_command[i], 0);
    }
    i2c_release(BUS);
    printf("\n(GPS) standby.\n");
}

void xm1110_glp_mode(const xm1110_t *dev) {
    assert(dev);
    char glp_command[21] = "$PMTK262,3*2B\r\n";

    i2c_acquire(BUS);
    // i2c_write_regs(BUS, ADDR, REG_W, &glp_command, 21, 0);
    for(int i = 0; i<21; i++) {
        i2c_write_byte(BUS, ADDR, glp_command[i], 0);
    }
    i2c_release(BUS);

    printf("\n(GPS) GLP activated.\n");
}

int xm1110_read(const xm1110_t *dev, xm1110_data_t *xmdata) {

    assert(dev && xmdata);
    int res;
    i2c_acquire(BUS);
    for(int i = 0; i<255; i++) {
        res = i2c_read_byte(BUS, ADDR, &xmdata->data[i], 0);
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
    } else if(res != 0) {
        printf("\n\nError int: %d\n", res);
    }

    return 0;
}