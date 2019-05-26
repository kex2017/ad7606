/*
 * data_acquisition.h
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_NEW_DATA_ACQUISITION_H_
#define PARTS_CABLE_GROUND_NEW_DATA_ACQUISITION_H_

#include <stdint.h>

typedef struct _collector_info {
    float latitude;
    float longitude;
    float altitude;
    char collector_desc[64];
}collector_info_t ;

/**
 * @brief prepare to trigger sample data information
 *
 * @param[in] timestamp         start to sample curve data timestamp
 * @param[in] channel_count     total channel count to sample
 * @param[in] cfg       every channel configuration parameters
 *
 * @return                      0 on success, else failure
 **/
uint32_t data_acquisition_set_trigger_sammple_info(uint32_t timestamp, uint8_t channel_count, void* cfg);

/**
 * @brief read common curve data with the packet information
 *
 * @param[in] channel           channel Number which to read
 * @param[in] timestamp         curve data sample timestamp
 * @param[in] size              single packet length
 * @param[in] sn                serial number of current channel curve data
 * @param[out] data             destination to save curve data
 * @param[out] total_pkt_count  total packet number of current channel curve data
 *
 * @return                      0 < ret on success, else failure
 **/
int data_acquisition_read_curve_data(uint8_t channel, uint32_t timestamp, uint16_t size, uint16_t cur_pkt_num, void* data, uint16_t* total_pkt_count);

/**
 * @brief get curve data length
 * @param[in] channel           channel Number which to read
 * @param[in] timestamp         curve data sample timestamp
 *
 * @return                      0 on success, else failure
 * */
uint32_t data_acquisition_get_curve_data_len(uint8_t channel, uint32_t timestamp, uint32_t *len);

/**
 * @brief read power frequency current of A\B\C and total phase
 * @param[in] channel      channel Number which to read
 * @param[in] timestamp    start to sample current timestamp
 * @param[out] data        destination to save current data
 *
 * @return                 0 on success, else failure
 **/
uint32_t data_acquisition_read_pf_current(uint8_t channel, uint32_t timestamp, void* data);

/**
 * @brief read collector device channel:0~3 JF current
 * @param[in] channel            channel Number which to read
 *
 * @return                 0 on success, else failure
 **/
uint32_t data_acquisition_read_jf_current(uint8_t channel);
uint32_t data_acquisition_read_pf_all_info(uint8_t channel, uint32_t timestamp, void* data);

void data_acquisition_service_init(void);

void read_current_phase_harmonics_data(uint8_t channel);

#endif /* PARTS_CABLE_GROUND_NEW_DATA_ACQUISITION_H_ */
