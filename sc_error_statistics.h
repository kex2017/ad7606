#ifndef SC_ERROR_STATISTICS_H_
#define SC_ERROR_STATISTICS_H_

#include <stdint.h>

void statistics_rcv_num(void);
void statistics_snd_num(void);
void statistics_rcv_cs_err_num(void);
void statistics_hf_current_jf_error_num(uint8_t channel, uint16_t num);
void statistics_dielectric_loss_error_num(uint8_t channel, uint16_t num);
void statistics_over_voltage_error_num(uint8_t channel, uint16_t num);
void statistics_partial_discharge_error_num(uint8_t channel, uint16_t num);

int statistics_command(int argc, char **argv);

#endif
