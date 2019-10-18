# Default RIOT bootloader
APPLICATION = bootloader

# Default testing board
BOARD ?= kl-cg-ibox-new


FEATURES_REQUIRED += periph_timer
FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += periph_rtt
FEATURES_REQUIRED += periph_flashpage

#ROM_START_ADDR ?= 0x08000000

# Disable unused modules
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
DISABLE_MODULE += auto_init

USEMODULE += hashes
# Include riotboot flash partition functionality

# RIOT codebase
RIOTBASE ?= $(CURDIR)/../RIOT/

include $(RIOTBASE)/Makefile.include

# limit riotboot bootloader size
# TODO: Manage to set this variable for boards which already embed a
# bootloader, currently it will be overwritten
#ROM_LEN := $(RIOTBOOT_LEN)
