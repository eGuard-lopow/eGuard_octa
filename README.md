# eGuard_octa
Low power project

## Configure
- Add your device's ABP keys to `template_keys.h` and rename this file to `keys.h`

## Drivers
- This repository uses modified drivers. To install these correctly place the 
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
```

- Edit the `RIOTBASE/drivers/Makefile.include` file and add the following code:
```
ifneq (,$(filter lsm303agr,$(USEMODULE)))
  USEMODULE_INCLUDES += $(RIOTBASE)/drivers/lsm303agr/include
endif

ifneq (,$(filter xm1110,$(USEMODULE)))
  USEMODULE_INCLUDES += $(RIOTBASE)/drivers/xm1110/include
endif
```