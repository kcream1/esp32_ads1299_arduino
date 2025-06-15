#ifndef ____ADS1299_H__
#define ____ADS1299_H__

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define _WAKEUP 0x02 // Wake-up from standby mode
#define _STANDBY 0x04 // Enter Standby mode
#define _RESET 0x06 // Reset the device
#define _START 0x08 // Start and restart (synchronize) conversions
#define _STOP 0x0A // Stop conversion
#define _RDATAC 0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 0x11 // Stop Read Data Continuous mode
#define _RDATA 0x12 // Read data by command; supports multiple read back

#define _RREG 0x20 // (also = 00100000) is the first opcode that the address must be added to for RREG communication
#define _WREG 0x40 // 01000000 in binary (Datasheet, pg. 35)

#define ID 0x00
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 0x04
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C



void printRegisterName(byte _address);
void RREG(byte _address, int CS);
void WREG(byte _address, byte _value, int CS);
void RDATA(int CS);
void SDATAC(int CS);
void RDATAC(int CS);
void STOP(int CS);
void START(int CS);
void RESET(int CS);
void WAKEUP(int CS);
void STANDBY(int CS);


//------------------------------------------------
  // //设置噪声测试
  // WREG(CH1SET,0x01,CS4);
  // delay(100);
  // WREG(CH2SET,0x01,CS4);
  // delay(100);
  // WREG(CH3SET,0x01,CS4);
  // delay(100);
  // WREG(CH4SET,0x01,CS4);
  // delay(100);
  // WREG(CH5SET,0x01,CS4);
  // delay(100);
  // WREG(CH6SET,0x01,CS4);
  // delay(100);
  // WREG(CH7SET,0x01,CS4);
  // delay(100);
  // WREG(CH8SET,0x01,CS4);
  // delay(100);

  // START(CS4);

  // delay(100);
  // RDATAC(CS4);
  // delay(100);
  // //这里就可以等待DRDY发出信号了
  // updateData(DRDY4,CS4,outputCount);  
  // delay(1000);
//-------------------------------------------------------


#endif
