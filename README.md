# eGuard_octa
Low power project

## Configure
- Add your device's ABP keys to `template_keys.h` and rename this file to `keys.h`

## Drivers
- This repository uses modified drivers. To install these correctly place the 
	following files in the correct folders:
	- Move the .h files in `drivers/include/` to `RIOTBASE/drivers/include/`
	- Move the .c files in `drivers/drivers/` to `RIOTBASE/drivers/DRIVERX/`
		with DRIVERX being the name of the corresponding driver
	- Move the param files in `drivers/drivers/` to `RIOTBASE/drivers/DRIVERX/include/`
		with DRIVERX being the name of the corresponding driver
