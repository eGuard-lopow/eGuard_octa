/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_LSM303AGR
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LSM303AGR 3D accelerometer/magnetometer.
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "LSM303AGR.h"
#include "LSM303AGR-internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define DEV_I2C         (dev->params.i2c)
#define DEV_ACC_ADDR    (dev->params.acc_addr)
#define DEV_ACC_PIN     (dev->params.acc_pin)
#define DEV_ACC_RATE    (dev->params.acc_rate)
#define DEV_ACC_SCALE   (dev->params.acc_scale)
#define DEV_MAG_ADDR    (dev->params.mag_addr)
#define DEV_MAG_PIN     (dev->params.mag_pin)
#define DEV_MAG_RATE    (dev->params.mag_rate)
#define DEV_MAG_GAIN    (dev->params.mag_gain)

int LSM303AGR_init(LSM303AGR_t *dev, const LSM303AGR_params_t *params)
{
    dev->params = *params;

    int res;
    uint8_t tmp;

    /* Acquire exclusive access to the bus. */
    i2c_acquire(DEV_I2C);

    DEBUG("LSM303AGR reboot...");
    res = i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_CTRL5_A, LSM303AGR_REG_CTRL5_A_BOOT, 0);
    /* Release the bus for other threads. */
    i2c_release(DEV_I2C);
    DEBUG("[OK]\n");

    /* configure accelerometer */
    /* enable all three axis and set sample rate */
    tmp = (LSM303AGR_CTRL1_A_XEN
          | LSM303AGR_CTRL1_A_YEN
          | LSM303AGR_CTRL1_A_ZEN
          | DEV_ACC_RATE);
    i2c_acquire(DEV_I2C);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303AGR_REG_CTRL1_A, tmp, 0);
    /* update on read, MSB @ low address, scale and high-resolution */
    tmp = (DEV_ACC_SCALE | LSM303AGR_CTRL4_A_HR);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303AGR_REG_CTRL4_A, tmp, 0);
    /* no interrupt generation */
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303AGR_REG_CTRL3_A, LSM303AGR_CTRL3_A_I1_NONE, 0);
    /* configure acc data ready pin */
    gpio_init(DEV_ACC_PIN, GPIO_IN);

    /* configure magnetometer and temperature */
    /* enable temperature output and set sample rate */
    tmp = LSM303AGR_TEMP_EN | DEV_MAG_RATE;
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                         LSM303AGR_REG_CRA_M, tmp, 0);
    /* configure z-axis gain */
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                         LSM303AGR_REG_CRB_M, DEV_MAG_GAIN, 0);
    /* set continuous mode */
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                         LSM303AGR_REG_MR_M, LSM303AGR_MAG_MODE_CONTINUOUS, 0);
    i2c_release(DEV_I2C);
    /* configure mag data ready pin */
    gpio_init(DEV_MAG_PIN, GPIO_IN);

    return (res < 0) ? -1 : 0;
}

int LSM303AGR_read_acc(const LSM303AGR_t *dev, LSM303AGR_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(DEV_I2C);
    i2c_read_reg(DEV_I2C, DEV_ACC_ADDR, LSM303AGR_REG_STATUS_A, &tmp, 0);
    DEBUG("LSM303AGR status: %x\n", tmp);
    DEBUG("LSM303AGR: wait for acc values ... ");

    res = i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                       LSM303AGR_REG_OUT_X_L_A, &tmp, 0);
    data->x_axis = tmp;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_OUT_X_H_A, &tmp, 0);
    data->x_axis |= tmp<<8;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                       LSM303AGR_REG_OUT_Y_L_A, &tmp, 0);
    data->y_axis = tmp;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_OUT_Y_H_A, &tmp, 0);
    data->y_axis |= tmp<<8;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                       LSM303AGR_REG_OUT_Z_L_A, &tmp, 0);
    data->z_axis = tmp;
    res += i2c_read_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_OUT_Z_H_A, &tmp, 0);
    data->z_axis |= tmp<<8;
    i2c_release(DEV_I2C);
    DEBUG("read ... ");

    data->x_axis = data->x_axis>>4;
    data->y_axis = data->y_axis>>4;
    data->z_axis = data->z_axis>>4;

    if (res < 0) {
        DEBUG("[!!failed!!]\n");
        return -1;
    }
    DEBUG("[done]\n");

    return 0;
}

int LSM303AGR_read_mag(const LSM303AGR_t *dev, LSM303AGR_3d_data_t *data)
{
    int res;

    DEBUG("LSM303AGR: wait for mag values... ");
    while (gpio_read(DEV_MAG_PIN) == 0){}

    DEBUG("read ... ");

    i2c_acquire(DEV_I2C);
    res = i2c_read_regs(DEV_I2C, DEV_MAG_ADDR,
                        LSM303AGR_REG_OUT_X_H_M, data, 6, 0);
    i2c_release(DEV_I2C);

    if (res < 0) {
        DEBUG("[!!failed!!]\n");
        return -1;
    }
    DEBUG("[done]\n");

    /* interchange y and z axis and fix endiness */
    int16_t tmp = data->y_axis;
    data->x_axis = ((data->x_axis<<8)|((data->x_axis>>8)&0xff));
    data->y_axis = ((data->z_axis<<8)|((data->z_axis>>8)&0xff));
    data->z_axis = ((tmp<<8)|((tmp>>8)&0xff));

    /* compensate z-axis sensitivity */
    /* gain is currently hardcoded to LSM303AGR_GAIN_5 */
    data->z_axis = ((data->z_axis * 400) / 355);

    return 0;
}

int LSM303AGR_read_temp(const LSM303AGR_t *dev, int16_t *value)
{
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_read_regs(DEV_I2C, DEV_MAG_ADDR, LSM303AGR_REG_TEMP_OUT_H,
                        value, 2, 0);
    i2c_release(DEV_I2C);

    if (res < 0) {
        return -1;
    }

    *value = (((*value) >> 8) & 0xff) | (*value << 8);

    DEBUG("LSM303AGR: raw temp: %i\n", *value);

    return 0;
}

int LSM303AGR_enable_interrupt(const LSM303AGR_t *dev, int cm)
{
	int res = 0;
	uint8_t tmp;

	
	//Default scale = 4G
	//Default ODR = 10 Hz: But place in 1Hz, and use activity threshold
	//page 55 in manual for registers
    i2c_acquire(DEV_I2C);
	
	
	 tmp = (LSM303AGR_CTRL1_A_XEN
          | LSM303AGR_CTRL1_A_YEN
          | LSM303AGR_CTRL1_A_ZEN
		  | LSM303AGR_CTRL1_A_10HZ);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303AGR_REG_CTRL1_A, tmp, 0);
						 /*
	res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303AGR_REG_ACT_THS_A, 0x0F, 0); // *16mg for 2G
	res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                         LSM303AGR_REG_ACT_DUR_A, 0x01, 0); // *8 + 1 for 1Hz */
						 
	//FREE FALL DETECTION
	res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_CTRL2_A ,LSM303AGR_CTRL2_A_HIGHPASS_DIS, 0); //high pass filter disabled
	res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_CTRL3_A ,LSM303AGR_CTRL3_A_I1_AOI1, 0); //Interrupt driven to INT1 pad
	res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_CTRL4_A ,LSM303AGR_CTRL4_A_SCALE_2G, 0); //FS = 2G
	//res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
    //                    LSM303AGR_REG_CTRL5_A ,0x08, 0); //Interrupt latched
	res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_INT1_THS_A ,0x16, 0); //Set free fall threshold = 350mg (*16mg for 2G)
	switch (cm){
		case 40:
		res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_INT1_DURATION_A ,0x02, 0); //Set minimum event duration (/ODR (default 10Hz)) max = 0x7F
		break;
		case 90:
		res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_INT1_DURATION_A ,0x03, 0); 
		break;
		default:
		res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_INT1_DURATION_A ,0x01, 0); 
		break;
	}
	tmp = (LSM303AGR_INT1_XLIE
				  | LSM303AGR_INT1_YLIE
				  | LSM303AGR_INT1_ZLIE
				  | LSM303AGR_INT1_AOI);
	res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_INT1_CFG_A ,tmp, 0); //Configure Free fall recognition
	i2c_release(DEV_I2C);
	 
						
	return (res < 0) ? -1 : 0;
}

int LSM303AGR_clear_int(const LSM303AGR_t *dev, int8_t *value)
{
	int res;

    i2c_acquire(DEV_I2C);
    res = i2c_read_regs(DEV_I2C, DEV_MAG_ADDR, LSM303AGR_REG_INT1_SRC_A,
                        value, 1, 0);
    i2c_release(DEV_I2C);
	
	return res;
}

int LSM303AGR_disable(const LSM303AGR_t *dev)
{
    int res;

    i2c_acquire(DEV_I2C);
    res = i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_CTRL1_A, LSM303AGR_CTRL1_A_POWEROFF, 0);
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                        LSM303AGR_REG_MR_M, LSM303AGR_MAG_MODE_SLEEP, 0);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR,
                        LSM303AGR_REG_CRA_M, LSM303AGR_TEMP_DIS, 0);
    i2c_release(DEV_I2C);

    return (res < 0) ? -1 : 0;
}

int LSM303AGR_enable(const LSM303AGR_t *dev)
{
    int res;
    uint8_t tmp = (LSM303AGR_CTRL1_A_XEN
                  | LSM303AGR_CTRL1_A_YEN
                  | LSM303AGR_CTRL1_A_ZEN
                  | LSM303AGR_CTRL1_A_N1344HZ_L5376HZ);
    i2c_acquire(DEV_I2C);
    res = i2c_write_reg(DEV_I2C, DEV_ACC_ADDR, LSM303AGR_REG_CTRL1_A, tmp, 0);

    tmp = (LSM303AGR_CTRL4_A_BDU| LSM303AGR_CTRL4_A_SCALE_2G | LSM303AGR_CTRL4_A_HR);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR, LSM303AGR_REG_CTRL4_A, tmp, 0);
    res += i2c_write_reg(DEV_I2C, DEV_ACC_ADDR, LSM303AGR_REG_CTRL3_A,
                         LSM303AGR_CTRL3_A_I1_DRDY1, 0);
    gpio_init(DEV_ACC_PIN, GPIO_IN);

    tmp = LSM303AGR_TEMP_EN | LSM303AGR_TEMP_SAMPLE_75HZ;
    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR, LSM303AGR_REG_CRA_M, tmp, 0);

    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                        LSM303AGR_REG_CRB_M, LSM303AGR_GAIN_5, 0);

    res += i2c_write_reg(DEV_I2C, DEV_MAG_ADDR,
                        LSM303AGR_REG_MR_M, LSM303AGR_MAG_MODE_CONTINUOUS, 0);
    i2c_release(DEV_I2C);

    gpio_init(DEV_MAG_PIN, GPIO_IN);

    return (res < 0) ? -1 : 0;
}
