#include "cpu.h"
#include "panic.h"
#include "upgrade.h"
#include "periph/pm.h"

void kernel_init(void)
{
	printf("*********************************************************************\r\n");
	printf("===============================BOOTLOADER============================\r\n");
	printf("*********************************************************************\r\n");
	if(do_upgrade())
	{

		pm_reboot();
	}
	cpu_jump_to_image(0x08010000);
	return;
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
