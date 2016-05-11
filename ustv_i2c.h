#ifndef ESPHI_I2C_H
#define ESPHI_I2C_H

#define I2C_ACK 0
#define I2C_NAK 1

	
void InitI2C1(void);
//unsigned char I2C1WriteN( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* chaine, unsigned int length );
unsigned char I2C1Write1( unsigned char slaveAddress, unsigned char registerAddress, unsigned char data) ;
void I2C1ReadN( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* data, unsigned int length );
//void I2C1Read1( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* data );
void putI2C1(unsigned char value);
unsigned char getI2C1(unsigned int ackToSend);
void IdleI2C1Antiblock(void);

void IncrementI2CAntiBlockCounter(void);
void ResetI2CAntiBlockCounter(void);
void SetI2CCrash(void);
void ResetI2CCrash(void);
unsigned char IsI2CCrashed(void);


#endif
