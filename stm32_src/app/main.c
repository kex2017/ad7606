#include "shell_commands.h"
#include "shell.h"
#include "board.h"
#include "data_transfer.h"
#include "heart_beat.h"
#include "daq.h"
#include "sc_daq.h"
#include "gps_sync.h"
#include "over_current.h"

static const shell_command_t shell_commands[] = {
        {   "daq", "daq ops", daq_command},
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

	gps_service_init();
	data_transfer_init();
	frame_parser_data_init();
	over_current_service_init();

	char line_buf[SHELL_DEFAULT_BUFSIZE];
	shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
	return 0;
}
