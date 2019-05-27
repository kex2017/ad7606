#include "shell_commands.h"
#include "shell.h"
#include "board.h"
#include "heart_beat.h"

#include "periph/gpio.h"

static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

void ext_pm_init(void)
{
   ext_pm_ctrl_init();
   ext_pm_power_on_all();
}

#define  FPGA_INT1         GPIO_PIN(PORT_C, 0)

void on_int(void *arg)
{
    (void)arg;
    printf("int ----------\r\n");
}


int main(void)
{
//    ext_pm_init();
//
//    daq_init();
//    sd_card_set_app_to_run();

//	frame_parser_data_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}

