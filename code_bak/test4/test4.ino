

#include "ads1299.h"

boolean deviceIDReturned = false;
boolean startedLogging = false;

int DRDY=42, CS=3; //pin numbers for "Data Ready" (DRDY) and "Chip Select" CS (Datasheet, pg. 26)
int outputCount;
float tCLKs = 0.000666;

const int ADS_START_PIN = 7; 
const int ADS_RESET_PIN = 8;
const int ADS_PWDN_PIN = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  SPI.begin(SCK,MISO,MOSI);
  SPI.setClockDivider(SPI_CLOCK_DIV16); //Divides 16MHz clock by 16 to set CLK speed to 1MHz
  SPI.setDataMode(SPI_MODE1);  //clock polarity = 0; clock phase = 1 (pg. 8)
  SPI.setBitOrder(MSBFIRST);  //data format is MSB (pg. 25)

  // SPI.begin(SCK,MISO,MOSI);
  // SPI.setBitOrder(MSBFIRST);
  // SPI.setDataMode(SPI_MODE1);
  // SPI.setClockDivider(SPI_CLOCK_DIV16); // 设置SPI时钟分频

  setup(DRDY, CS,outputCount);//初始化设备1
  delay(10);

  //初始化引脚START，RESET

  pinMode(ADS_START_PIN, OUTPUT);//高电平表示开始转换
  pinMode(ADS_RESET_PIN, OUTPUT);//拉低后表示复位
  pinMode(ADS_PWDN_PIN, OUTPUT);//拉低后表示断电

  digitalWrite(ADS_PWDN_PIN,HIGH);
  digitalWrite(ADS_RESET_PIN,HIGH);
  digitalWrite(ADS_START_PIN,LOW);
  delay(1000);



  RESET(CS);//复位
  delay(3000);

  SDATAC(CS);
  delay(500);

  WREG(CONFIG3,0xE0,CS);
  delay(3000);
  WREG(CONFIG1,0xb6,CS);
  delay(100);
  WREG(CONFIG2,0xd0,CS);
  delay(100);


  RREG(CONFIG1,CS);
  RREG(CONFIG2,CS);
  RREG(CONFIG3,CS);


  //设置噪声测试
  WREG(CH1SET,0x01,CS);
  delay(100);
  WREG(CH2SET,0x01,CS);
  delay(100);
  WREG(CH3SET,0x01,CS);
  delay(100);
  WREG(CH4SET,0x01,CS);
  delay(100);
  WREG(CH5SET,0x01,CS);
  delay(100);
  WREG(CH6SET,0x01,CS);
  delay(100);
  WREG(CH7SET,0x01,CS);
  delay(100);
  WREG(CH8SET,0x01,CS);
  delay(100);

  START(CS);
  digitalWrite(ADS_START_PIN,LOW);
  delay(100);
  RDATAC(CS);
  delay(100);
  //这里就可以等待DRDY发出信号了
  updateData(DRDY,CS,outputCount);  
  delay(1000);


  //设置内部测试信号
  SDATAC(CS);
  delay(100);
  WREG(CONFIG2,0xd0,CS);
  delay(100);

  WREG(CH1SET,0x55,CS);
  delay(100);
  WREG(CH2SET,0x55,CS);
  delay(100);
  WREG(CH3SET,0x55,CS);
  delay(100);
  WREG(CH4SET,0x55,CS);
  delay(100);
  WREG(CH5SET,0x55,CS);
  delay(100);
  WREG(CH6SET,0x55,CS);
  delay(100);
  WREG(CH7SET,0x55,CS);
  delay(100);
  WREG(CH8SET,0x55,CS);
  delay(100);

  RDATAC(CS);
  delay(100);
  //这里就可以等待DRDY发出信号了
  updateData(DRDY,CS,outputCount);  
  delay(1000);


}

void loop() {
  // put your main code here, to run repeatedly:
  updateData(DRDY,CS,outputCount); 
  delay(1000); 

}


