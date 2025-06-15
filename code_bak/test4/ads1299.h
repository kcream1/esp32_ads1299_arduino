
#ifndef ____ADS1299_H__
#define ____ADS1299_H__

#include <stdio.h>
#include <Arduino.h>
#include <SPI.h>
#include "Definitions.h"





void setup(int DRDY, int CS,int outputCount);

//ADS1299 SPI Command Definitions (Datasheet, Pg. 35)
//System Commands
void WAKEUP(int CS,float tCLKs);
void STANDBY(int CS);
void RESET(int CS);
void START(int CS);
void STOP(int CS);

//Data Read Commands
void RDATAC(int CS);
void SDATAC(int CS);
void RDATA(int CS);

//Register Read/Write Commands
void getDeviceID(int CS);
void RREG(byte _address,int CS);
void RREGS(byte _address, byte _numRegistersMinusOne,int CS); //to read multiple consecutive registers (Datasheet, pg. 38)

void printRegisterName(byte _address);

void WREG(byte _address, byte _value,int CS); //
//void WREGS(byte _address, byte _value, byte _numRegistersMinusOne); //

void updateData(int DRDY,int CS,int outputCount);

//SPI Arduino Library Stuff
//byte transfer(byte _data);




    

#endif
