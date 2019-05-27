/*
 * gps.h
 *
 *  Created on: May 27, 2019
 *      Author: xuke
 */

#ifndef APP_GPS_H_
#define APP_GPS_H_

void gps_init(void);
int gps_read(char* buffer, int count);
void gps_enable(void);

#endif /* APP_GPS_H_ */
