/* 
 * File:   UART Protocol.h
 * Author: E105-POSTE2
 *
 * Created on 2 décembre 2015, 12:10
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

#define LED_PROTOCOL 0x0020
#define TELEMETRE_PROTOCOL 0x0030
#define CONSIGNE_PROTOCOL 0x0040
#define STATE_PROTOCOL 0x0050
#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052
#define SET_MOTOR_SPEED 0x0053
#define QEI_SET_POSITION 0x0062
#define LIDAR_PROTOCOL 0x0070
#define SET_LIDAR_SENSITIVITY 0x0071
#define MESSAGE_PROTOCOL 0x0080
#define BALISE_HISTOGRAM 0x0090
#define BALISE_ANGLES 0x0091
#define BALISE_POSITION 0x0092
#define SET_POSITION 0x00A0
#define ASSERVISSEMENT_POSITION_TELEMETRIE 0x00A1
#define ASSERVISSEMENT_VITESSE_TELEMETRIE 0x00A2
#define TELEMETRE_ULTRASONIC_DATA 0xB0

#define RECEPTION_WAIT 0
#define RECEPTION_FUNCTION_MSB 1
#define RECEPTION_FUNCTION_LSB 2
#define RECEPTION_PAYLOAD_LENGTH_MSB 3
#define RECEPTION_PAYLOAD_LENGTH_LSB 4
#define RECEPTION_PAYLOAD 5
#define RECEPTION_CHECKSUM 6


void Uart1EncodeAndSendMessage(int, int, unsigned char*);
void Uart1DecodeMessage(unsigned char);
void Uart2DecodeMessage(unsigned char c);
unsigned char UartCalculateChecksum(int, int, unsigned char*);
void UartProcessMessage(unsigned char, unsigned char, unsigned char*);

#endif	/* UART_PROTOCOL_H */

