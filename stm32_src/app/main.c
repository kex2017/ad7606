#include "shell_commands.h"
#include "shell.h"
#include "board.h"
#include "vc_temp_bat_vol_sample.h"
#include "data_processor.h"
#include "dev_cfg.h"
#include "sc_cfg.h"
#include "heart_beat.h"
#include "history_data_send.h"
#include "send_command.h"

#include "data_transfer.h"
#include "daq.h"
#include "sc_daq.h"
#include "flash_fpga_from_sd_card.h"


static const shell_command_t shell_commands[] = {
        {   "daq", "daq ops", daq_command},
        {   "cfg", "cfg ops", cfg_command },
		{	"send", "send test",frame_command},
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
    sd_card_set_app_to_run();
    init_dev_cfg();
	data_transfer_init();
	frame_parser_data_init();
    vc_temp_bat_sample_serv_init();
	data_processor_thread_init();
	send_heart_beat_thread_init();
	history_data_send_init();
	watch_dog_service_init();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}

