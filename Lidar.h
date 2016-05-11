/* 
 * File:   Lidar.h
 * Author: Valentin
 *
 * Created on 9 mars 2016, 23:09
 */

#ifndef LIDAR_H
#define	LIDAR_H

#define QUALITY_BYTE 1
#define ANGLE_LSB_BYTE 2
#define ANGLE_MSB_BYTE 3
#define DISTANCE_LSB_BYTE 4
#define DISTANCE_MSB_BYTE 5
#define ANGLE_MAX 165
#define ANGLE_MIN 15

extern unsigned char flagLidarFrameReady;
extern unsigned char DistanceTableToSend[ANGLE_MAX-ANGLE_MIN];

//unsigned char Min (unsigned char a,unsigned char b);
void DecodeIrRangeFinder(unsigned char c);
//void RplidarPostProcessing(void);

#endif	/* LIDAR_H */

