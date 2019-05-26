APPLICATION = cable_ground

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/RIOT

# If no BOARD is found in the environment, use this default:
#BOARD ?= kl-stm32-a1
#BOARD ?= native

BOARD ?= kl-stm32-cg
#CFLAGS += -DUSE_SPI_SDCARD

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP 
ROM_START_ADDR ?=0x08010000
# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

FEATURES_REQUIRED += periph_timer
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += periph_rtt
FEATURES_REQUIRED += periph_flashpage

# Modules to include:
USEMODULE += uart_stdio
USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += log
USEMODULE += log_printftimestamp
USEMODULE += fmt
USEMODULE += mtd
USEMODULE += mtd_spi_nor
USEMODULE += printf_float
USEMODULE += kldaq_fpga_spi
USEMODULE += sdcard_spi
USEMODULE += auto_init_storage
USEMODULE += fatfs_diskio_sdcard_spi
USEMODULE += hashes
USEPKG += fatfs
USEPKG += heatshrink

GIT_VERSION := $(shell git --no-pager describe --tags --always)
GIT_COMMIT  := $(shell git rev-parse --verify HEAD)
#GIT_DATE    := $(firstword $(shell git --no-pager show --date=iso-strict --format="%ad" --name-only))
#BUILD_DATE  := $(shell date --iso=seconds)

CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\" \
		  -DGIT_COMMIT=\"$(GIT_COMMIT)\" \
		  -DAPPLICATION=\"$(APPLICATION)\"

include $(RIOTBASE)/Makefile.include