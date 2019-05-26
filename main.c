#include "thread.h"

#include "shell_commands.h"
#include "shell.h"
#include "cmd.h"

#include "board.h"
#include "data_storage.h"
#include "cfg.h"

#include "daq.h"
#include "gps_sync.h"
#include "x_delay.h"

#include "temperature.h"

#include "frame_handler.h"
#include "cable_ground_service.h"
#include "data_acquisition.h"
#include "partial_discharge.h"

#include "over_voltage.h"
#include "sc_data_acquisition.h"
#include "sc_nrsec3000.h"
#include "sc_env.h"
#include "sc_daq.h"
#include "sc_error_statistics.h"

#include "comm_packet.h"
#include "circular_task.h"
#include "sc_upgrade_kl_fpga.h"
#include "sc_user_cmd.h"

static const shell_command_t shell_commands[] = {
    { "send", "send packet to communication pipe", comm_send_packet_command },
    { "daq", "daq cmd", daq_command },
    { "demo", "user cmd", user_command},
//    { "sample", "sample", sample_command },
    { "sec", "run nrsec3000 spi test", nrsec30000_command },
    { "setenv", "set env", setenv_command },
    { "saveenv", "save modification env", saveenv_command },
    { "printenv", "print env", printenv_command },
	 { "stats", "print statistics", statistics_command },
	 { "upgrade", "FSMC FPGA upgrade", upgrade_kl_fpga_command},
    { NULL, NULL, NULL }
};


void ext_pm_init(void)
{
   ext_pm_ctrl_init();
   ext_pm_power_on_all();
}

int main(void)
{
   kernel_pid_t cable_groud_service_pid;
   kernel_pid_t cable_groud_sender_pid;
   data_storage_init();

   load_device_cfg();

   ext_pm_init();
   delay_s(5);//谢工建议等待FSMC的FPGA配置完后再访问(.big_data), 否则容易出意外

   gps_service_init();
//   temp_read_service_init();

   data_acquisition_service_init();

   cable_groud_sender_pid = comm_packet_sender_init();
   cable_groud_service_pid = cable_ground_service_init();

   comm_circular_sender_hook(cable_groud_sender_pid);
   comm_handler_sender_hook(cable_groud_sender_pid);
   comm_packet_receiver_hook(cable_groud_service_pid);

   comm_packet_receiver_init();
   IWDG_init();
   heartbeat_service_init();
	char line_buf[SHELL_DEFAULT_BUFSIZE];
	shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

	return 0;
}
