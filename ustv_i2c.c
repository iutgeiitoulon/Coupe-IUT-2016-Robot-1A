#include "ustv_i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include "p33Fxxxx.h"
#include <libpic30.h>

#define I2C_BLOCK_LIMIT 1000
unsigned int I2CAntiBlockCounter=0;


void InitI2C1(void)
{
	I2C1BRG = 393; //Voir page 15 du ref manual I2C	
    PMD1bits.I2C1MD = 0;						// 1 : peripheral disabled
    											// 0 : peripheral active    
    
    I2C1CONbits.I2CEN = 0;          // 1 : Enables the I2Cx module and configures the SDAx and SCLx pins as serial port pins
                                    // 0 : Disables the I2Cx module; all I2C pins are controlled by port functions

    //Routine de déplantage éventuel du BUS I2C
    _TRISB5 = 0;
    _TRISB6 = 0;
    _LATB5 = 0;
    _LATB6 = 0;

    __delay32(1000);
    _LATB5 = 1;
    _LATB6 = 1;
    
    I2C1CONbits.I2CSIDL = 1; 					// 1 : stop in idle mode
    											// 0 : continue in idle mode
    											
    I2C1CONbits.SCLREL = 1; 					// 1 : Release SCLx clock
    											// 0 : Hold SCLx clock low (clock stretch)
    											
    I2C1CONbits.IPMIEN = 0; 					// 1 : IPMI Support mode is enabled
    											// 0 : IPMI Support mode disabled (forced in master mode)
    											
    I2C1CONbits.A10M = 0; 						// 1 : I2CxADD register is a 10-bit slave address
    											// 0 : I2CxADD register is a 7-bit slave address
    											
    I2C1CONbits.DISSLW = 0; 					// 1 : Slew rate control disabled, en 100kHz
    											// 0 : Slew rate control enabled, en 400kHz
    											
    I2C1CONbits.SMEN = 0; 						// 1 : Enable I/O pin thresholds compliant with SMBus specification
    											// 0 : Disable SMBus input thresholds
    											
    I2C1CONbits.GCEN = 0;	 					// 1 : Enable interrupt when a general call address is received in the I2CxRSR register (module in reception)
    											// 0 : General call address disabled
    											
    I2C1CONbits.STREN = 0; 						// 1 : Enable user software or receive clock stretching
    											// 0 : Disable user software or receive clock stretching
    											
    I2C1CONbits.ACKDT = 0; 						// 1 : Send NACK during Acknowledge
    											// 0 : Send not ACK during Acknowledge
    											   
    I2C1CONbits.I2CEN = 1; 						// 1 : Enables the I2Cx module and configures the SDAx and SCLx pins as serial port pins
    											// 0 : Disables the I2Cx module; all I2C pins are controlled by port functions
    
    //Désactivation des interruptions I2C
    IEC1bits.MI2C1IE = 0;
    IEC1bits.SI2C1IE = 0;
    IdleI2C1Antiblock();         	//wait for idle condition, lower 5 bits must be 0
} 


//****************************************************************************** 
//* Function Name: I2CByteRead * 
//* Return Value: error condition status * 
//* Parameters: EE memory device control, address and data * 
//* bytes. * 
//* Description: Write single data byte to I2C EE memory * 
//* device. This routine can be used for any I2C * 
//* EE memory device, which uses 2 bytes of * 
//* address data as in the 24LC32A/64A/256A. * 
//* * 
//****************************************************************************** 

void I2C1ReadN( unsigned char slaveAddress, unsigned char registerAddress, unsigned char* data, unsigned int length ) 
{ 
	I2C1CONbits.I2CEN = 1; 					
	IdleI2C1Antiblock();  //wait for idle condition, lower 5 bits must be 0
	
	I2C1CONbits.SEN = 1; // Transmit a Start condition

	//while(I2C1CONbits.SEN ); // Wait till Start sequence is completed     
        ResetI2CAntiBlockCounter();
        while ((I2C1CONbits.SEN) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();

	// Write Slave address and set master for transmission (R/W bit should be 0) 
	putI2C1(slaveAddress & 0xFE);

	// Write word address for register 
	putI2C1(registerAddress);  // write byte

	I2C1CONbits.RSEN = 1;		// Lance le restart

	//while(I2C1CONbits.RSEN);    // Wait till Start sequence is completed 
        ResetI2CAntiBlockCounter();
        while ((I2C1CONbits.RSEN) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();

	// Write Slave address and set master for transmission (R/W bit should be 1) 
	putI2C1(slaveAddress | 0x01);  // write byte

	int i;
	for (i=0;i<length;i++)
	{
		if(i!=length-1)
			*data = getI2C1(I2C_ACK); // Envoi de Ack sur toutes les lectures sauf la dernière
		else
			*data = getI2C1(I2C_NAK); // Envoi de Nack sur le dernière lecture
	 	data++;
	}	
	

	I2C1CONbits.PEN=1; // send STOP condition

	//while(I2C1CONbits.PEN); // Wait till Stop sequence is completed 
	ResetI2CAntiBlockCounter();
        while ((I2C1CONbits.PEN) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();
        
	IdleI2C1Antiblock();         	//wait for idle condition, lower 5 bits must be 0
	
	
}

unsigned char I2C1Write1( unsigned char slaveAddress, unsigned char registerAddress, unsigned char data) 
{ 	
	I2C1CONbits.I2CEN = 1; 					
	IdleI2C1Antiblock();         	//wait for idle condition, lower 5 bits must be 0
	
	I2C1CONbits.SEN = 1; 				// Transmit a Start condition

	//while(I2C1CONbits.SEN ); 			// Wait till Start sequence is completed 
	ResetI2CAntiBlockCounter();
        while ((I2C1CONbits.SEN) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();
        
	// Write Slave address and set master for transmission (R/W bit should be 0) 
	putI2C1(slaveAddress & 0xFE); 
	
	// Write Register Address
	putI2C1(registerAddress); 
	
	// Write Slave address and set master for transmission (R/W bit should be 0) 
	putI2C1(data); 
		 
	I2C1CONbits.PEN=1; 					// send STOP condition

	//while(I2C1CONbits.PEN); 			// Wait till Stop sequence is completed 
        ResetI2CAntiBlockCounter();
        while ((I2C1CONbits.PEN) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();
	
	IdleI2C1Antiblock();         	//wait for idle condition, lower 5 bits must be 0
	
	return 0; // return with no error 
}   


/**
Operation: Send one byte (\em u8_val), if NAK is returned use reportError() function to save error and do software reset.
\param u8_val byte to send
*/
void putI2C1(unsigned char value) 
{
	I2C1TRN = value;  					// write byte

 	//while (I2C1STATbits.TBF); 			// wait for 8bits+ ack bit to finish
        ResetI2CAntiBlockCounter();
        while ((I2C1STATbits.TBF) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();

	//while (I2C1STATbits.TRSTAT); 		// wait for 8bits+ ack bit to finish	
        ResetI2CAntiBlockCounter();
        while ((I2C1STATbits.TRSTAT) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();

}

/**
Operation: Wait for a byte byte on the I2C bus, send \em u8_ack2Send as the acknowledgement bit to send back to the slave.
\param u8_ack2Send ack bit to send back to slave after byte is read
\return byte read from slave
*/
unsigned char getI2C1(unsigned int ackToSend) 
{
	unsigned char wdtState;
	unsigned char inByte;
	
	wdtState = _SWDTEN;             	//save WDT state
	IdleI2C1Antiblock();         	//wait for idle condition, lower 5 bits must be 0
	I2C1CONbits.RCEN = 1;           	//enable receive
	
        ResetI2CAntiBlockCounter();
        while ((!I2C1STATbits.RBF) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));      	//wait for receive byte
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();
        
	inByte = I2C1RCV;            		//read byte;
	
	//wait for idle condition before attempting ACK
	IdleI2C1Antiblock();         	//wait for idle condition, lower 5 bits must be 0
	I2C1CONbits.ACKDT = ackToSend;		//Send NACK during Acknowledge
	I2C1CONbits.ACKEN = 1;          	//enable ACKbit transmition

        ResetI2CAntiBlockCounter();
	while ((I2C1CONbits.ACKEN) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));     	//wait for completion
        if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
            SetI2CCrash();
	return(inByte);                  	//return the value
}

void IncrementI2CAntiBlockCounter(void)
{
    I2CAntiBlockCounter+=1;
}

void ResetI2CAntiBlockCounter(void)
{
    I2CAntiBlockCounter=0;
}


unsigned char I2CCrash=0;
void SetI2CCrash(void)
{
    I2CCrash = 1;
}

void ResetI2CCrash(void)
{
    I2CCrash = 0;
}

unsigned char IsI2CCrashed(void)
{
    return I2CCrash;
}

void IdleI2C1Antiblock(void)
{
    ResetI2CAntiBlockCounter();
    while((I2C1CONbits.SEN || I2C1CONbits.PEN || I2C1CONbits.RCEN ||
            I2C1CONbits.RSEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) && (I2CAntiBlockCounter<I2C_BLOCK_LIMIT));
    if(I2CAntiBlockCounter>=I2C_BLOCK_LIMIT)
        SetI2CCrash();
}
