/*
 * cable_ground_threads.h
 *
 *  Created on: Aug 27, 2018
 *      Author: chenzy
 */

#ifndef PARTS_CABLE_GROUND_CABLE_GROUND_THREADS_H_
#define PARTS_CABLE_GROUND_CABLE_GROUND_THREADS_H_


#include "thread.h"

/*************************************************************
 * thread for GPS event
 *************************************************************/
#define GPS_THREAD_PRIORITY     (0)
extern kernel_pid_t gps_service_pid;
extern char gps_service_thread_stack[THREAD_STACKSIZE_MAIN * 8] __attribute__((section(".big_data")));

/*************************************************************
 * thread accept data from 4G client
 *************************************************************/
#define COMM_PACKET_RECEIVE_PRIORITY        (1)
extern kernel_pid_t receiver_pid;
extern char comm_packet_receiver_stack[THREAD_STACKSIZE_MAIN * 8] __attribute__((section(".big_data")));

/*************************************************************
 * thread for cable ground frame handler
 *************************************************************/
#define CABLE_GROUND_PRIORITY       (2)
extern kernel_pid_t cable_ground_service_pid;
extern char cable_ground_service_thread_stack[THREAD_STACKSIZE_MAIN * 8] __attribute__((section(".big_data")));

/*************************************************************
 * thread for PRPS, PRPD curve data
 *************************************************************/
#define DATA_ACQUISITION_THREAD_PRIORITY        (3)
extern kernel_pid_t data_acquisition_service_pid;
extern char data_acquisition_service_thread_stack[THREAD_STACKSIZE_MAIN * 14] __attribute__((section(".big_data")));

/*************************************************************
 * thread for heartbeat
 *************************************************************/
#define HEARTBEAT_THREAD_PRIORITY   (4)
extern kernel_pid_t heartbeat_thread_pid;
extern char heartbeat_thread_stack[THREAD_STACKSIZE_MAIN * 2] __attribute__((section(".big_data")));

/*************************************************************
 * thread from protocol data to frame
 *************************************************************/
#define COMM_PACKET_SENDER_PRIORITY     (5)
extern kernel_pid_t comm_packet_sender_pid;
extern char comm_packet_sender_stack[THREAD_STACKSIZE_MAIN * 8] __attribute__((section(".big_data")));

/*************************************************************
 * thread for OVER VOLTAGE event
 *************************************************************/
//#define OVER_VOLTAGE_PRIORITY              (6)
//extern kernel_pid_t over_voltage_pid;
//extern char over_voltage_stack[THREAD_STACKSIZE_MAIN * 8];

/*************************************************************
 * thread for PD event
 *************************************************************/
//#define PARTIAL_DISCHARGE_PRIORITY               (7)
//extern kernel_pid_t partial_discharge_pid;
//extern char pd_service_stack[THREAD_STACKSIZE_MAIN * 8];

/*************************************************************
 * thread curve data transfer
 *************************************************************/
#define CIRCULAR_TASK_THREAD_PRIORITY   (8)
extern kernel_pid_t circular_task_service_pid;
extern char circular_task_thread_stack[THREAD_STACKSIZE_MAIN * 28] __attribute__((section(".big_data")));

/*************************************************************
 * thread for temperature data
 *************************************************************/
#define TEMP_THREAD_PRIORITY    (9)
extern kernel_pid_t temp_service_pid;
extern char temp_service_thread_stack[THREAD_STACKSIZE_MAIN] __attribute__((section(".big_data")));



#endif /* PARTS_CABLE_GROUND_CABLE_GROUND_THREADS_H_ */
