#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>


// 定义WiFi凭据
const char* ssid = "FAST_DDD9"; // 替换为你的WiFi SSID
const char* password = "pystzw123456"; // 替换为你的WiFi密码

// 定义TCP服务器地址和端口
const char* host = "192.168.1.104"; // 替换为你想要连接的服务器地址
const int port = 8888; // 替换为你想要连接的服务器的端口

//  ADS1299 控制引脚
const int ADS_CS[] = {10, 9, 46, 3};  // ADS1299片选信号引脚
const int ADS_START_PIN = 7; 
const int ADS_RESET_PIN = 8;
const int ADS_PWDN_PIN = 1;
const int NTAG_FD_PIN = 35;

//ADS1299 数据引脚
const int ADS_DRDY[] = {39, 40, 41, 42}; // ADS1299的DRDY引脚

// 定义SPI的MOSI、SCK和MISO引脚
const int MOSI2 = 11;
const int SCK2 = 12;
const int MISO2 = 13;


	/*寄存器读取命令*/
#define	RREG 0x20	//读取001r rrrr 000n nnnn  这里定义的只有高八位，低8位在发送命令时设置
#define WREG 0x40	//写入010r rrrr 000n nnnn
	/*	r rrrr=要读、写的寄存器地址
			n nnnn=要读、写的数据*/
/*ADS1292内部寄存器地址定义*/
#define ID				0X00	//ID控制寄存器
#define CONFIG1		0X01	//配置寄存器1
#define CONFIG2		0X02	//配置寄存器2
#define CONFIG3 	0X03	//配置寄存器3			PD_REFBUF,1,1,BIAS_MEAS,BIASREF_INT,PD_BIAS,BIAS_LOFF_SENS,BIAS_STAT
#define	MISC1				0X15
#define	MISC2				0X16

#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C

//SPI Command Definitions (pg. 35)
const byte WAKEUP  = 0b00000010;     // Wake-up from standby mode
const byte STANDBY = 0b00000100;   // Enter Standby mode
const byte RESET   = 0b00000110;   // Reset the device
const byte START   = 0b00001000;   // Start and restart (synchronize) conversions
const byte STOP    = 0b00001010;   // Stop conversion
const byte RDATAC  = 0b00010000;   // Enable Read Data Continuous mode (default mode at power-up) 
const byte SDATAC  = 0b00010001;   // Stop Read Data Continuous mode
const byte RDATA   = 0b00010010;   // Read data by command; supports multiple read back

//Register Read Commands
// const byte RREG = 0b00000000;
// const byte WRET = 0b00000000;
const float tCLK = 0.000666;


int outputCount;
byte regData [24];

WiFiClient client;
void initWifiSta()
{
  // 连接WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

}

void setup() {
  // 初始化串行通信
  Serial.begin(9600);
  // 建立WiFi连接
  initWifiSta();
  // 建立TCP连接
  if (client.connect(host, port)) {
    Serial.println("Connected to TCP server");
    // 发送HTTP GET请求（示例）
    client.print("Hello, server!");
  } else {
    Serial.println("Failed to connect to TCP server");
  }


    // 初始化spi
  SPI.begin(MOSI2, SCK2, MISO2);
  SPI.setClockDivider(SPI_CLOCK_DIV16); //Divides 16MHz clock by 16 to set CLK speed to 1MHz
  SPI.setDataMode(SPI_MODE1);  //clock polarity = 0; clock phase = 1 (pg. 8)
  SPI.setBitOrder(MSBFIRST);  //data format is MSB (pg. 25)

  // 拉高ADS1299片选引脚，取消选择从机设备
  for (int i = 0; i < 4; i++) {
    pinMode(ADS_CS[i], OUTPUT);
    digitalWrite(ADS_CS[i], HIGH); 
  }

  pinMode(ADS_START_PIN, OUTPUT);
  pinMode(ADS_RESET_PIN, OUTPUT);
  pinMode(ADS_PWDN_PIN, OUTPUT);
  pinMode(NTAG_FD_PIN, OUTPUT);

  digitalWrite(ADS_START_PIN,HIGH);
  digitalWrite(ADS_RESET_PIN, HIGH);

  // 设置DRDY输入模式
  for (int i = 0; i < 4; i++) {
    pinMode(ADS_DRDY[i], INPUT);
  }

  // 初始化 ADS1299 
  for (int i = 0; i < 4; i++) {
    initializeADS1299(i);
  }

}

void loop() {

  //读取ADS1299数据
  for (int i = 0; i < 4; i++) {
    //if (digitalRead(ADS_DRDY[i]) == LOW) { 
    //int data = readADS1299Data(i);
    //sendDataToServer(i,data);
    // Serial.println(data);
    client.print("pys");
    //RREG(i,0x03);


    WREGS(CONFIG1,0b11010110,i);
    delay(1000);
    updateData(i);
    // readADS1299Data(i);
    delay(300);
    //}
  }

  // 等待1秒
  delay(1000);
}

void WREGS(byte _address, byte _value,int chipIndex) {
    byte opcode1 = WREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(ADS_CS[chipIndex], LOW); //Low to communicated
    SPI.transfer(SDATAC); //SDATAC
    SPI.transfer(opcode1);
    SPI.transfer(0x00);
    SPI.transfer(_value);
    SPI.transfer(RDATAC);
    digitalWrite(ADS_CS[chipIndex], HIGH); //Low to communicated
    Serial.print("Register 0x");
    Serial.print(_address, HEX);
    Serial.println(" modified.");
}

void updateData(int chipIndex){
    Serial.print("DRDY........");
    if(digitalRead(ADS_DRDY[chipIndex]) == LOW){
        Serial.print("DRDYLOW");
        digitalWrite(ADS_CS[chipIndex], LOW);
        long output[9];
        long dataPacket;
        for(int i = 0; i<9; i++){
            for(int j = 0; j<3; j++){
                byte dataByte = SPI.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte;
            }
//            output[outputCount][i] = dataPacket;
            output[i] = dataPacket;
            dataPacket = 0;
        }
        digitalWrite(ADS_CS[chipIndex], HIGH);
        Serial.print(outputCount);
        Serial.print(", ");
        for (int i=0;i<9; i++) {
            Serial.print(output[i], HEX);
            if(i!=8) Serial.print(", ");
            
        }
        Serial.println();
        outputCount++;
    }
}

void initializeADS1299(int chipIndex) {
  digitalWrite(ADS_CS[chipIndex], LOW); // 选择一个ADS1299
  delay(1); // 稳定性延迟

  digitalWrite(ADS_START_PIN,LOW);
  delay(100);
  SPI.transfer(SDATAC); 
  delay(1000);

  digitalWrite(ADS_RESET_PIN, HIGH);
  delay(3000);
  digitalWrite(ADS_RESET_PIN, LOW);
  delay(100);
  SPI.transfer(SDATAC); 
  delay(1000);
  digitalWrite(ADS_RESET_PIN, HIGH);



  Serial.println("data");
  delay(10);
  unsigned char data = RD_REG(RREG,1);
  Serial.println(data);
  delay(1000);




  // SPI.transfer(SDATAC); 
  // delay(1000);

  // getDeviceID(chipIndex);
  // delay(100);
  // RREGS(0x00);
  // delay(100);




  digitalWrite(ADS_CS[chipIndex], HIGH); // 取消选择ADS1299设备
  delay(1);

// ADS1299的附加配置可以在这里添加

}

void getDeviceID(int chipIndex) {
    digitalWrite(ADS_CS[chipIndex], LOW); //Low to communicated
    SPI.transfer(SDATAC); //SDATAC
    SPI.transfer(RREG); //RREG
    SPI.transfer(0x00); //Asking for 1 byte
    byte data = SPI.transfer(0x00); // byte to read (hopefully 0b???11110)
    SPI.transfer(RDATAC); //turn read data continuous back on
    digitalWrite(ADS_CS[chipIndex], HIGH); //Low to communicated
    Serial.println("ID");
    Serial.println(data, BIN);
}



void WR_REG(unsigned char com,unsigned char data)
{
	unsigned char tmp,i=4;
	
  SPI.transfer(com);

	while(i--);i=4;
	tmp = 0x00;
	SPI.transfer(tmp);
	while(i--);
	SPI.transfer(data);
}


unsigned char RD_REG(unsigned char com,unsigned char num)
{
	unsigned char data_return;
	unsigned char tmp;
	SPI.transfer(com);		//发送第1字节（命令字）
	tmp = num - 1;
	data_return = SPI.transfer(0x00);		//发送第2字节（读取字节个数） ====f===连续读多个寄存器
	
	return(data_return);
}
 


void RREGS(byte _address,int chipIndex) {
    byte opcode1 = RREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(ADS_CS[chipIndex], LOW); //Low to communicated
    SPI.transfer(SDATAC); //SDATAC
    SPI.transfer(opcode1); //RREG
    SPI.transfer(0x00); //opcode2
    byte data = SPI.transfer(0x00); // returned byte should match default of register map unless edited manually (Datasheet, pg.39)
    Serial.print("0x");
    if(_address<16) Serial.print("0");
    Serial.print(_address, HEX);
    Serial.print(", ");
    Serial.print("0x");
    if(data<16) Serial.print("0");
    Serial.print(data, HEX);
    Serial.print(", ");
    for(byte j = 0; j<8; j++){
        Serial.print(bitRead(data, 7-j), BIN);
        if(j!=7) Serial.print(", ");
    }
    SPI.transfer(RDATAC); //turn read data continuous back on
    digitalWrite(ADS_CS[chipIndex], HIGH); //High to end communication
    Serial.println();
}


