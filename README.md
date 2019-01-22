# eGuard_octa

## About
The eGuard is a device which tracks the health of an electronic device by using sensors like an accelerometer, a light sensor, a temperature and humidity sensor, ...

While programming, we paid attention to the low power aspect which is needed in the world of IoT to let devices run several years on a single battery.

## Information

University of Antwerp

Course: I-IoT Low Power Embedded Systems

Professsor/teacher: Maarten Weyn, Michiel Aernouts, Mats De Meyer

Master Students: Thomas Verellen, Robin Janssens, Toon De Keyzer


## Setup

If the eGuard is inside (determined with DASH7 connection), it will use DASH7 to communicate and fingerprinting to locate itself in the class room. When outside, the eGuard will automatically switch to LoRaWAN to communicate with the backend and use the GPS module to locate itself.

> In the current version, the switch between DASH7 vs. LoRaWAN and GPS is done by pressing `BTN1` to illustrate the operation.

### Hardware eGuard

- Nucleo L496ZG
- Octa connect shield
- Octa connect Murata modem shield
- Octa connnect GPS shield

![Imgur](https://i.imgur.com/fKzjctM.jpg)

### Backend
  - MongoDB
  - Fingerprinting
  - MQTT broker
  - The Things Network
  - Thingsboard
  - Telegram Push Notifications

![Imgur](https://i.imgur.com/5m04SeV.png)

## Deploy

### Hardware

- Octa Board
  - Mount the Murata modem shield on P1
  - Mount the GPS shield on P2

### Keys
- Add your device's ABP keys to `template_keys.h` and rename this file to `keys.h`

### Drivers
- This repository uses newly made drivers. To install these, correctly place the 
	following files in the correct folders:
	- Move the .h files in `drivers/include/` to `RIOTBASE/drivers/include/`
	- Move the folders with .c files in `drivers/drivers/` to `RIOTBASE/drivers/`

- Edit the `RIOTBASE/drivers/Makefile.dep` file and add the following code:
```
ifneq (,$(filter lsm303agr,$(USEMODULE)))
  FEATURES_REQUIRED += periph_i2c
endif

ifneq (,$(filter xm1110, $(USEMODULE)))
  USEMODULE += xtimer
  FEATURES_REQUIRED += periph_i2c
endif

ifneq (,$(filter tcs34725,$(USEMODULE)))
  FEATURES_REQUIRED += periph_i2c
endif
```

- Edit the `RIOTBASE/drivers/Makefile.include` file and add the following code:
```
ifneq (,$(filter lsm303agr,$(USEMODULE)))
  USEMODULE_INCLUDES += $(RIOTBASE)/drivers/lsm303agr/include
endif

ifneq (,$(filter xm1110,$(USEMODULE)))
  USEMODULE_INCLUDES += $(RIOTBASE)/drivers/xm1110/include
endif

ifneq (,$(filter tcs34725,$(USEMODULE)))
  USEMODULE_INCLUDES += $(RIOTBASE)/drivers/tcs34725/include
endif
```

### I2C configuration
- Edit the `RIOTBASE/boards/octa/include/periph_conf.h` file en replace the I2C-config with the following:
```cpp
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C1EN,
        .irqn           = I2C1_EV_IRQn
    },
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_F, 0),
        .sda_pin        = GPIO_PIN(PORT_F, 1),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR1_I2C2EN,
        .irqn           = I2C2_EV_IRQn
    }
};
```

## Components/Techniques

### GPS

The global (outdoor) position of the eGuard is determined by two variables; latitude and longitude. These variables represent the position on the earth.

GPS data is presented in NMEA data. NMEA data can have different formats for different kinds of data. Basically, it consists of structured characters. Data is kept in the buffer (255 bytes) of the GPS module. We can read the data from the buffer sequentially through I2C.

When data is retrieved, it can be parsed to obtain the coordinates. We will use [minmea](https://github.com/kosma/minmea) because it was already integrated in RIOT OS. We chose to parse NMEA data in the RMC format (Recommended Minimum Data for gps), which looks like the following.

`$GNRMC,105824.000,A,5110.577055,N,00420.844651,E,0.42,285.58,080119,,,A*73`

We discard all the other NMEA data.

The data fiels are seperated using a comma. These are the fields:
- 1st field defines the format
- 2nd field defines the time the fix is taken (UTC time)
- 3rd field defines the status (Active or Void)
- 4th field defines latitude
- 5th field defines longitude
- 6th fiels defines speed (knots)
- 7th field defines track angle
- 8th field defines the date
- 9th field defines th magnetic variation in degrees
- 10th field defines the checksum data, always begins with *

After reading through I2C and parsing the data, we send the latitude and longitude via LoRaWAN to the backend.

### Fingerprinting

## Power Measurement

## Division Of Labour

Thomas
- Drivers sensors
- ISR
- Power Measuring

Robin
- Backend

Toon
- GPS
