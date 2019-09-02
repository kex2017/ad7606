#include "shell_commands.h"
#include "shell.h"
#include "board.h"
#include "data_transfer.h"
#include "heart_beat.h"
#include "daq.h"
#include "sc_daq.h"
#include "env_cfg.h"
#include "gps_sync.h"
#include "period_data.h"
#include "sc_device_cfg_test.h"
#include "upgrade_from_flash.h"
#include "data_send.h"
#include "over_current.h"

static const shell_command_t shell_commands[] = {
		{ "setenv", "set device cfg", set_device_cfg},
        { "daq", "daq ops", daq_command},
		{ "printenv", "print env", printenv_command },
    { NULL, NULL, NULL }
};

void ext_pm_init(void)
{
   ext_pm_ctrl_init();
   ext_pm_power_on_all();
}

int main(void)
{
	ext_pm_init();
	daq_init();

	download_fpga_image(FPGA_A_CS);
	delay_s(1);
	download_fpga_image(FPGA_B_CS);
	delay_s(1);
	download_fpga_image(FPGA_C_CS);
	delay_s(5);

	load_device_cfg();

	gps_service_init();

	data_transfer_init();
	frame_parser_data_init();

	data_send_serv_init();

	hf_pf_over_current_service_init();
	period_data_serv_init();

	send_heart_beat_thread_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;

}
