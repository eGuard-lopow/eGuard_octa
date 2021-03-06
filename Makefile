# name of your application
APPLICATION = eGuard_octa

# If no BOARD is found in the environment, use this default:
BOARD ?= octa

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#USEMODULE += schedstatistics

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += sht3x  # temperature and humidity sensor
USEMODULE += lsm303agr
USEMODULE += tcs34725

# include and auto-initialize all available sensors
# USEMODULE += saul_default
USEMODULE += xtimer
USEMODULE += periph_gpio_irq

# GPS packages
USEMODULE += xm1110
USEPKG += minmea

# Modem
EXTERNAL_MODULE_DIRS += $(RIOTBASE)/../riot-oss7-modem/drivers/oss7_modem
USEMODULE += oss7_modem

INCLUDES += -I$(RIOTBASE)/../riot-oss7-modem//drivers/oss7_modem/include

include $(RIOTBASE)/Makefile.include

# add sensorfolder:
USEMODULE += sensors
INCLUDES += -I$(CURDIR)/sensors
DIRS += $(CURDIR)/sensors

CFLAGS += -DDEBUG_ASSERT_VERBOSE
