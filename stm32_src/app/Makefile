APPLICATION = cg-ibox-new

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT/

# If no BOARD is found in the environment, use this default:
BOARD ?= kl-cg-ibox-new

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP 
ROM_START_ADDR ?= 0x08010000
# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0
FEATURES_REQUIRED += periph_spi
FEATURES_REQUIRED += periph_timer
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += periph_rtt
FEATURES_REQUIRED += periph_flashpage
FEATURES_REQUIRED += periph_gpio
FEATURES_REQUIRED += periph_stm32f10x_std_periph

# Modules to include:
USEMODULE += xtimer
USEMODULE += kldaq_fpga_spi
USEMODULE += klfpga
USEMODULE += kldaq
USEMODULE += hashes
USEMODULE += periph_gpio_irq

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += schedstatistics

USEMODULE += printf_float
USEMODULE += frame_paser
USEMODULE += mtd
USEMODULE += mtd_spi_nor
USEMODULE += auto_init_storage
USEMODULE += fmt
USEMODULE += log
USEMODULE += log_printftimestamp
#USEMODULE += log_printfnoformat
#USEMODULE += uart_frame
#USEMODULE += checksum
#USEMODULE += printf_float

USEMODULE += kldaq_fpga_spi
USEMODULE += klfpga
USEMODULE += kldaq
USEMODULE += hashes

USEMODULE += fatfs_diskio_sdcard_spi
USEMODULE += sdcard_spi
USEPKG += fatfs
USEMODULE += mtd
USEMODULE += mtd_spi_nor
USEMODULE += auto_init_storage

GIT_VERSION := $(shell git --no-pager describe --tags --dirty --always)

CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\" \
		  -DAPPLICATION=\"$(APPLICATION)\"


include $(RIOTBASE)/Makefile.include