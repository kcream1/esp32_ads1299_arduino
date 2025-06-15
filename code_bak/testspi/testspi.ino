#include <SPI.h>
#include "ads1299.h"

// 定义SPI引脚
#define PIN_MOSI 11
#define PIN_MISO 13
#define PIN_CLK 12
#define PIN_CS 10  // 片选引脚

int DRDY=39, CS=10; //pin numbers for "Data Ready" (DRDY) and "Chip Select" CS (Datasheet, pg. 26)
int outputCount;
float tCLKs = 0.000666;

const int ADS_START_PIN = 7;
const int ADS_RESET_PIN = 8;
const int ADS_PWDN_PIN = 1;

void setup() {
  // 初始化串口用于调试
  Serial.begin(115200);

  // 配置片选引脚
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH); // 片选引脚初始为高电平

  pinMode(ADS_START_PIN, OUTPUT);
  pinMode(ADS_RESET_PIN, OUTPUT);
  pinMode(ADS_PWDN_PIN, OUTPUT);

  digitalWrite(ADS_PWDN_PIN,HIGH);
  digitalWrite(ADS_RESET_PIN,HIGH);
  digitalWrite(ADS_START_PIN,LOW);

  // 初始化SPI
  SPI.begin(PIN_CLK, PIN_MISO, PIN_MOSI, PIN_CS);

  // 配置SPI设置
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(CS, LOW); //Low to communicated
  SPI.transfer(_RESET); 
  digitalWrite(CS, HIGH); //Low to communicated
  delay(100);
  digitalWrite(CS, LOW); //Low to communicated
  digitalWrite(ADS_START_PIN,HIGH);
  SPI.transfer(_START);
  digitalWrite(CS, HIGH); //Low to communicated
  delay(1000);

  
  // setup(DRDY, CS,outputCount);//初始化设备1
  // delay(10);

  //   //初始化引脚START，RESET
  // pinMode(ADS_START_PIN, OUTPUT);
  // pinMode(ADS_RESET_PIN, OUTPUT);
  // delay(100);
  // digitalWrite(ADS_START_PIN,HIGH);
  // //digitalWrite(ADS_RESET_PIN, HIGH);
  // delay(100);


  // digitalWrite(ADS_START_PIN,LOW);
  // delay(100);
  // SDATAC(CS);
  // delay(1000);

  // RESET(CS);//复位
  // delay(100);
  // //digitalWrite(ADS_RESET_PIN, LOW);
  // WAKEUP(CS,tCLKs);
  // delay(3000);

  // WREG(CONFIG2,0xd5,CS);
  // delay(100);
  // RREG(CONFIG2,CS);
  // delay(100);

  // digitalWrite(ADS_START_PIN,HIGH);
  // RDATA(CS);
  // delay(100);
  // START(CS);
  // digitalWrite(ADS_START_PIN,HIGH);
  // delay(1000);

}

void loop() {
  // 开始SPI传输

  digitalWrite(PIN_CS, LOW); // 拉低片选引脚
  SPI.transfer(_SDATAC);
  SPI.transfer(0x20);
  SPI.transfer(0x00);
  //byte dataToSend = 0x55;    // 示例数据
  byte temp = SPI.transfer(0x00);

  digitalWrite(PIN_CS, HIGH); // 传输结束后拉高片选引脚


  // digitalWrite(CS, LOW); //Low to communicated
  // SPI.transfer(SDATAC);
  // SPI.transfer(0x20); //RREG
  // SPI.transfer(0x00); //Asking for 1 byte (hopefully 0b???11110)
  // byte temp = SPI.transfer(0x00);
  // digitalWrite(CS, HIGH); //Low to communicated
  
  Serial.println(temp, BIN);




  delay(1000);
}
