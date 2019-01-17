/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
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
 * @brief       Register definitions for the TCS34725 driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 */

#ifndef TCS34725_INTERNAL_H
#define TCS34725_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TCS34725_AG_THRESHOLD_LOW
#define TCS34725_AG_THRESHOLD_LOW       200
#endif

#ifndef TCS34725_AG_THRESHOLD_HIGH
#define TCS34725_AG_THRESHOLD_HIGH      (65535 - TCS34725_AG_THRESHOLD_LOW)
#endif

/**
 * @name    Register Map
 *
 * @note    All setting register are defined for repeated byte protocol transaction.
 * @{
 */
#define TCS34725_ENABLE             0x00 /**< Enables states and interrupts */
#define TCS34725_ATIME              0x01 /**< RGBC time */
// #define TCS34725_PTIME              0x82 /**< Proximity time */
#define TCS34725_WTIME              0x03 /**< Wait time */
#define TCS34725_AILTL              0x04 /**< Clear interrupt low threshold low byte */
#define TCS34725_AILTH              0x05 /**< Clear interrupt low threshold high byte */
#define TCS34725_AIHTL              0x06 /**< Clear interrupt high threshold low byte */
#define TCS34725_AIHTH              0x07 /**< Clear interrupt high threshold high byte */
// #define TCS34725_PILTL              0x08 /**< Proximity interrupt low threshold low byte */
// #define TCS34725_PILTH              0x09 /**< Proximity interrupt low threshold high byte */
// #define TCS34725_PIHTL              0x0A /**< Proximity interrupt high threshold low byte */
// #define TCS34725_PIHTH              0x0B /**< Proximity interrupt high threshold high byte */
#define TCS34725_PERS               0x0C /**< Interrupt persistence filters */
#define TCS34725_CONFIG             0x0D /**< Configuration */
// #define TCS34725_PPULSE             0x8E /**< Proximity pulse count */
#define TCS34725_CONTROL            0x0F /**< Gain control register */
#define TCS34725_ID                 0x12 /**< Device IDID */
#define TCS34725_STATUS             0x13 /**< Device status */
#define TCS34725_CDATA              0x14 /**< Clear ADC data low byte */
#define TCS34725_CDATAH             0x15 /**< Clear ADC data high byte */
#define TCS34725_RDATA              0x16 /**< Red ADC data low byte */
#define TCS34725_RDATAH             0x17 /**< Red ADC data high byte */
#define TCS34725_GDATA              0x18 /**< Green ADC data low byte */
#define TCS34725_GDATAH             0x19 /**< Green ADC data high byte */
#define TCS34725_BDATA              0x1A /**< Blue ADC data low byte */
#define TCS34725_BDATAH             0x1B /**< Blue ADC data high byte */
// #define TCS34725_PDATA              0x1C /**< Proximity ADC data low byte */
// #define TCS34725_PDATAH             0x1D /**< Proximity ADC data high byte */

// ORIGINELE WAARDEN
// #define TCS34725_ENABLE             0x80 /**< Enables states and interrupts */
// #define TCS34725_ATIME              0x81 /**< RGBC time */
// #define TCS34725_PTIME              0x82 /**< Proximity time */
// #define TCS34725_WTIME              0x83 /**< Wait time */
// #define TCS34725_AILTL              0x04 /**< Clear interrupt low threshold low byte */
// #define TCS34725_AILTH              0x05 /**< Clear interrupt low threshold high byte */
// #define TCS34725_AIHTL              0x06 /**< Clear interrupt high threshold low byte */
// #define TCS34725_AIHTH              0x07 /**< Clear interrupt high threshold high byte */
// #define TCS34725_PILTL              0x08 /**< Proximity interrupt low threshold low byte */
// #define TCS34725_PILTH              0x09 /**< Proximity interrupt low threshold high byte */
// #define TCS34725_PIHTL              0x0A /**< Proximity interrupt high threshold low byte */
// #define TCS34725_PIHTH              0x0B /**< Proximity interrupt high threshold high byte */
// #define TCS34725_PERS               0x8C /**< Interrupt persistence filters */
// #define TCS34725_CONFIG             0x8D /**< Configuration */
// #define TCS34725_PPULSE             0x8E /**< Proximity pulse count */
// #define TCS34725_CONTROL            0x8F /**< Gain control register */
// #define TCS34725_ID                 0x92 /**< Device IDID */
// #define TCS34725_STATUS             0x93 /**< Device status */
// #define TCS34725_CDATA              0x14 /**< Clear ADC data low byte */
// #define TCS34725_CDATAH             0x15 /**< Clear ADC data high byte */
// #define TCS34725_RDATA              0x16 /**< Red ADC data low byte */
// #define TCS34725_RDATAH             0x17 /**< Red ADC data high byte */
// #define TCS34725_GDATA              0x18 /**< Green ADC data low byte */
// #define TCS34725_GDATAH             0x19 /**< Green ADC data high byte */
// #define TCS34725_BDATA              0x1A /**< Blue ADC data low byte */
// #define TCS34725_BDATAH             0x1B /**< Blue ADC data high byte */
// #define TCS34725_PDATA              0x1C /**< Proximity ADC data low byte */
// #define TCS34725_PDATAH             0x1D /**< Proximity ADC data high byte */
/** @} */

/**
 * @name    Command Register
 * @{
 */
#define TCS34725_BYTE_TRANS         0x80 /**< Repeated byte protocol transaction */
#define TCS34725_INC_TRANS          0xA0 /**< Auto-increment protocol transaction */
#define TCS34725_SF_PICLR           0xE5 /**< Proximity interrupt clear */
#define TCS34725_SF_CICLR           0xE6 /**< Clear channel interrupt clear */
#define TCS34725_SF_PCICLR          0xE7 /**< Proximity and Clear channel interrupt clear */
/** @} */

/**
 * @name    Enable Register
 * @{
 */
#define TCS34725_ENABLE_PIEN        (1 << 5) /**< Proximity interrupt enable */
#define TCS34725_ENABLE_AIEN        (1 << 4) /**< Clear channel interrupt enable */
#define TCS34725_ENABLE_WEN         (1 << 3) /**< Wait enable, activates the wait feature */
#define TCS34725_ENABLE_PEN         (1 << 2) /**< Proximity enable, activates the proximity function */
#define TCS34725_ENABLE_AEN         (1 << 1) /**< RGBC enable, actives the two-channel ADC */
#define TCS34725_ENABLE_PON         (1 << 0) /**< Power ON */
/** @} */

/**
 * @name    Control Register
 * @{
 */
#define TCS34725_CONTROL_PDRIVE_100     0x00 /**< 100 mA LED Drive Strength */
#define TCS34725_CONTROL_PDRIVE_50      0x04 /**< 50 mA LED Drive Strength */
#define TCS34725_CONTROL_PDRIVE_25      0x08 /**< 25 mA LED Drive Strength */
#define TCS34725_CONTROL_PDRIVE_12      0x0C /**< 12.5 mA LED Drive Strength */
#define TCS34725_CONTROL_PDRIVE_MASK    0x0C /**< PDRIVE Mask */
#define TCS34725_CONTROL_AGAIN_1        0x00 /**<  1x gain RGBC Gain Value */
#define TCS34725_CONTROL_AGAIN_4        0x01 /**<  4x gain RGBC Gain Value */
#define TCS34725_CONTROL_AGAIN_16       0x02 /**< 16x gain RGBC Gain Value */
#define TCS34725_CONTROL_AGAIN_60       0x03 /**< 60x gain RGBC Gain Value */
#define TCS34725_CONTROL_AGAIN_MASK     0x03 /**< AGAIN Mask */
/** @} */

/**
 * @name    Device ID
 * @{
 */
#define TCS34725_ID_VALUE           0x12
// #define TCS34725_ID_VALUE           0x49
/** @} */

/**
 * @name    Predefined ATIME register values.
 * @{
 */
#define TCS34725_ATIME_MIN          2400    /* 2.4ms integration time, max count 1024 */
#define TCS34725_ATIME_MAX          614000  /* 614ms integration time, max count 0xffff */

#define TCS34725_ATIME_TO_REG(val)  (256 - (uint8_t)((val) / 2400))
#define TCS34725_ATIME_TO_US(reg)   ((256 - (uint8_t)(reg)) * 2400)
/** @} */

/**
 * @name    Coefficients for Lux and CT Equations (DN40)
 *
 * @note    Coefficients in integer format, multiplied by 1000.
 * @{
 */
#define DGF_IF                      310
#define R_COEF_IF                   136
#define G_COEF_IF                   1000
#define B_COEF_IF                   -444
#define CT_COEF_IF                  3810
#define CT_OFFSET_IF                1391
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TCS34725_INTERNAL_H */
/** @} */
