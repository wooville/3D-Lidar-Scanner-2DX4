
/* 
* This file is part of VL53L1 Platform 
* 
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved 
* 
* License terms: BSD 3-clause "New" or "Revised" License. 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer. 
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution. 
* 
* 3. Neither the name of the copyright holder nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
* 
*/

/*
		2DX4 Note
		The "platform" code provided by the manufacturer of the VL53L1X only specifies functions that
		must be implemented.  The following code was written for the students in McMaster Comp Eng 2DX4
		for their use in a final term project.  The code is suppled as-is for basic functionality of the 
		time of flight sensor -- modify at your own risk.
		
		Written by: T. Doyle
		Last update: March 15, 2020

*/

#include "tm4c1294ncpdt.h"
#include "vl53l1_platform_2dx4.h"
#include "onboardLEDs.h"
#include <string.h>
#include <time.h>
#include <math.h>

uint8_t _I2CBuffer[256];

int8_t beginTxI2C(uint8_t dev){
  I2C0_MSA_R = (dev)&0xFE;    // MSA[7:1] is slave address
  I2C0_MSA_R &= ~0x01;             // MSA[0] is 0 for send
	return 0;
}
	
int8_t writeRegisterIndex(uint16_t index) {
	uint8_t data1, data2;

	data1 = index>>8;
	data2 = index;
	
  I2C0_MDR_R = data1&0xFF;         // prepare first byte
  I2C0_MCS_R =  0x03&0xFF;				// See Reference Manual Chapter 19
	FlashI2CTx();	
  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C0_MCS_R = 0x04&0xFF;
		FlashI2CError(1);
                                          // return error bits if nonzero
    return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  I2C0_MDR_R = data2&0xFF;         // prepare second byte
  I2C0_MCS_R = 0x05&0xFF;
	FlashI2CTx();	
	
  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C0_MCS_R = 0x04&0xFF;
		FlashI2CError(1);
                                          // return error bits if nonzero
    return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }	return 0;
}

int8_t writeRegisterIndex_nostop(uint16_t index) {
	uint8_t data1, data2;

	data1 = index>>8;
	data2 = index;
	
  I2C0_MDR_R = data1&0xFF;         // prepare first byte
  I2C0_MCS_R =  0x03&0xFF;				// See Reference Manual Chapter 19
	FlashI2CTx();	
  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C0_MCS_R = 0x04&0xFF;
		FlashI2CError(1);
                                          // return error bits if nonzero
    return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }
  I2C0_MDR_R = data2&0xFF;         // prepare second byte
  I2C0_MCS_R = 0x01&0xFF;
	FlashI2CTx();	
	
  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
  if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
    I2C0_MCS_R = 0x04&0xFF;
		FlashI2CError(1);
                                          // return error bits if nonzero
    return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
  }	return 0;
}


//Write count bytes to dev
int8_t writeI2C(uint8_t *pdata, uint32_t count){

	for(int i = 0; i < count-1; i++){ 
		I2C0_MDR_R = (*pdata++)&0xFF;     // prepare byte

		if (i == 0) { I2C0_MCS_R =  0x03&0xFF; }				// See Reference Manual Chapter 19
		else { I2C0_MCS_R =  0x01&0xFF; }
		FlashI2CTx();	

		while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
		if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
			I2C0_MCS_R = 0x04&0xFF;
			FlashI2CError(1);
                                          // return error bits if nonzero
			return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
		}
	}

	I2C0_MDR_R = (*pdata)&0xFF;         // prepare last byte
	I2C0_MCS_R = 0x05&0xFF; 
	FlashI2CTx();	

  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
		if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
			I2C0_MCS_R = 0x04&0xFF;
			FlashI2CError(1);
                                          // return error bits if nonzero
			return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
		}
	return(0);
}
	

	
// Configure master to read data from dev
int8_t beginRxI2C(uint8_t dev){
		I2C0_MSA_R = (dev)&0xFE;    // MSA[7:1] is slave address
    I2C0_MSA_R |= 0x01;              // MSA[0] is 1 for receive

	return 0;
}



// Read 
//int8_t readI2C(int8_t *pdata, uint32_t count){
int8_t readI2C(uint8_t *pdata){	
	I2C0_MCS_R =  0x07&0xFF; 
	FlashI2CRx();	
  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
		
	*pdata = (I2C0_MDR_R&0xFF);       // read data
                                          // return error bits
  if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){

    I2C0_MCS_R = 0x04&0xFF;
		FlashI2CError(1);
																		// return error bits if nonzero
    return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));	
	}
	return(0);
/*	
	for(int i = 0; i < count-1; i++){ 
		if (i == 0) { I2C0_MCS_R =  0x0B&0xFF; }				// See Reference Manual Chapter 19
		else { I2C0_MCS_R =  0x09&0xFF; }

	  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
                                          // check error bits
		if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0){
			I2C0_MCS_R = 0x04&0xFF;
                                          // return error bits if nonzero
			return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));
		}			
		*pdata = (I2C0_MDR_R&0xFF);       // read data
	}	
	
  if (count == 1) { I2C0_MCS_R =  0x07&0xFF; }
	else { I2C0_MCS_R = 0x05&0xFF; }

  while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
		
	*pdata = (I2C0_MDR_R&0xFF);       // read data
                                          // return error bits
  return (I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR));	
*/
}








/* Write Multiple Bytes to VL53L1X
		Transmit in master mode an amount of data in blocking mode using
			dev			Device Handle (address, default 0x29)
			index		The register index inside the device
			pdata		Pointer to uint8_t buffer containing the data to be written (simply, this ia an array of type uint8_t)
			count		Number of bytes in the supplied byte buffer
*/
int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {

// send address, index msb, index lsb, loop send pdata-1 (no stop), send last pdata & stop
	beginTxI2C(dev);
	writeRegisterIndex(index);
 	
	//pdata
	writeI2C(pdata, count);
	
	return 0; 
}



/* Read Multiple Bytes from VL53L1X
		Read in master mode an amount of data in blocking mode using
			dev			Device Handle (address, default 0x29)
			index		The register index inside the device
			pdata		Pointer to uint8_t buffer containing the data to be read (simply, this ia an array of type uint8_t)
			count		Number of bytes in the supplied byte buffer
*/
int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	
// send address, send index, receive pdata-1, receive last pdata & stop  

	beginTxI2C(dev);
	writeRegisterIndex(index);	

	beginRxI2C(dev);
	for(int i = 0; i < count; i++) readI2C(pdata++);
	
	return 0;
}	



/* Write One Byte (8 bits) to VL53L1X
		Transmit in master mode an amount of data in blocking mode using
			dev			Device Handle (address, default 0x29)
			index		The register index inside the device
			data		Variable containing the data to be written
*/
int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
// send address, index msb, index lsb, send data & stop
	beginTxI2C(dev);
	writeRegisterIndex_nostop(index);
	
 	writeI2C(&data, 1);

	return 0; 
}

/* Write Word (16-bits) to VL53L1X
		Transmit in master mode an amount of data in blocking mode using
			dev			Device Handle (address, default 0x29)
			index		The register index inside the device
			data		variable containing the 16-bit data to be written
*/
int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {

	uint8_t d1, d2;
	
// send address, index msb, index lsb, send data msb, data lsb & stop
	beginTxI2C(dev);
	writeRegisterIndex(index);
	
	// Example Word 0x03EB
	//Little Endian
	//
	//		Address-1	EB	LSB
	//		Address		03	MSB
	
	d1 = data>>8&0xFF; 
	d2 = data&0xFF;
	
	writeI2C(&d2, 1);
	writeI2C(&d1, 1);

	return 0;
}

/* Write Double Word (32-bits) to VL53L1X
		Transmit in master mode an amount of data in blocking mode using
			dev			Device Handle (address, default 0x29)
			index		The register index inside the device
			data		variable containing the 32-bit data to be written
*/
int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {

// send address, index msb, index lsb, send data msb, data lsb & stop
	beginTxI2C(dev);
	writeRegisterIndex(index);
 		
	//data
	writeI2C(&data, 4);	
	return 0;}

/* Read One Byte (8-bits) from VL53L1X
		Read in master mode an amount of data in blocking mode using
			dev			Device Handle (address, default 0x29)
			index		The register index inside the device
			pdata		Pointer to uint8_t buffer containing the data to be read (simply, this ia an array of type uint8_t)
			count		Number of bytes in the supplied byte buffer
*/
//int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
//	return 0;//
//}
int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {

// send address, send index, receive data & stop  

	beginTxI2C(dev);
	writeRegisterIndex(index);	
	beginRxI2C(dev);
	readI2C(data);

	return 0;
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
//Cortex M microcontrollers use the little endian format.   Thus a 16-bit number is stored as
//
//		Address - 1			LSB
//		Address					MSB
//
	
// send address, send index, receive data & stop  

	uint8_t d1, d2;

	beginTxI2C(dev);
	writeRegisterIndex(index);
	
	beginRxI2C(dev);
		
	readI2C(&d1);
	readI2C(&d2);
	
	*data = d1<<8 | d2; 
	
	
	return 0;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
//Cortex M microcontrollers use the little endian format.   Thus a 32-bit number is stored as
//
//		Address - 3			LSB
//		Address - 2			MSB-2
//		Address - 1			MSB-1
//		Address					MSB
//

	// send address, send index, receive data & stop  
	uint8_t d1, d2, d3, d4;
	
	beginTxI2C(dev);
	writeRegisterIndex(index);	
	beginRxI2C(dev);

	readI2C(&d1);
	readI2C(&d2);
	readI2C(&d3);
	readI2C(&d4);
	*data = d1<<24 | d2<<16 | d3<<8 | d4; 
	return 0;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	//assumes 120MHz clock
  uint32_t i;
  for(i=0; i<wait_ms; i++){
    SysTick_Wait(120000);  // wait 10ms (assumes 120 MHz clock)
  }

	return 0;
}
