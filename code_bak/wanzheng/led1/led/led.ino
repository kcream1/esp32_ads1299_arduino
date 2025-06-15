
#include "ads1299.h"


// 定义WiFi凭据
const char* ssid = "FAST_DDD9"; // 替换为你的WiFi SSID
const char* password = "pystzw123456"; // 替换为你的WiFi密码

// 定义TCP服务器地址和端口
const char* host = "192.168.1.104"; // 替换为你想要连接的服务器地址
const int port = 8888; // 替换为你想要连接的服务器的端口


const int CS[4] = {10,9,46,3};
const int DRDY[4] = {39,40,41,42};

int outputCount;
WiFiClient client;

void getDeviceID(int px);
void updateData(int DRDY, int CS, int outputCount);

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
  
  Serial.begin(9600);
  
  // start the SPI library:
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16); //Divides 16MHz clock by 16 to set CLK speed to 1MHz
  SPI.setDataMode(SPI_MODE1);  //clock polarity = 0; clock phase = 1 (pg. 8)
  SPI.setBitOrder(MSBFIRST);  //data format is MSB (pg. 25)

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

  pinMode(DRDY[0],INPUT);
  pinMode(CS[0],OUTPUT);
  pinMode(DRDY[1],INPUT);
  pinMode(CS[1],OUTPUT);
  pinMode(DRDY[2],INPUT);
  pinMode(CS[2],OUTPUT);
  pinMode(DRDY[3],INPUT);
  pinMode(CS[3],OUTPUT);

  digitalWrite(CS[0], HIGH);
  digitalWrite(CS[1], HIGH);
  digitalWrite(CS[2], HIGH);
  digitalWrite(CS[3], HIGH);
  delay(10);  
  
  RESET(CS[3]); 
  delay(10);
  RESET(CS[1]); 
  delay(10);
  RESET(CS[2]); 
  delay(10);
  RESET(CS[0]); 
  delay(10);

  SDATAC(CS[3]);
  delay(10);
  SDATAC(CS[0]);
  delay(10);
  SDATAC(CS[1]);
  delay(10);
  SDATAC(CS[2]);
  delay(500);

  //配置主ads1299模块
  WREG(CONFIG3,0xE0,CS[3]);
  delay(100);
  WREG(CONFIG1,0xb6,CS[3]);
  delay(100);
  WREG(CONFIG2,0xd0,CS[3]);
  delay(100);

  //配置从ads1299模块
  WREG(CONFIG3,0xE0,CS[0]);
  delay(100);
  WREG(CONFIG1,0x96,CS[0]);
  delay(100);
  WREG(CONFIG2,0xc0,CS[0]);
  delay(100);

  WREG(CONFIG3,0xE0,CS[1]);
  delay(100);
  WREG(CONFIG1,0x96,CS[1]);
  delay(100);
  WREG(CONFIG2,0xc0,CS[1]);
  delay(100);

  WREG(CONFIG3,0xE0,CS[2]);
  delay(100);
  WREG(CONFIG1,0x96,CS[2]);
  delay(100);
  WREG(CONFIG2,0xc0,CS[2]);
  delay(100);


}

void loop() {

  for(int i=0;i<4;i++)
  {
    Serial.print("Device:");
    Serial.print(i);
    client.print("Device:");
    client.print(i);

    getDeviceID(CS[i]);
    //设置内部测试信号
    SDATAC(CS[i]);
    delay(100);
    WREG(CONFIG2,0xd0,CS[i]);
    delay(100);

    WREG(CH1SET,0x55,CS[i]);
    delay(100);
    WREG(CH2SET,0x55,CS[i]);
    delay(100);
    WREG(CH3SET,0x55,CS[i]);
    delay(100);
    WREG(CH4SET,0x55,CS[i]);
    delay(100);
    WREG(CH5SET,0x55,CS[i]);
    delay(100);
    WREG(CH6SET,0x55,CS[i]);
    delay(100);
    WREG(CH7SET,0x55,CS[i]);
    delay(100);
    WREG(CH8SET,0x55,CS[i]);
    delay(100);

    START(CS[i]);
    delay(100);
    RDATAC(CS[i]);
    delay(100);
    //这里就可以等待DRDY发出信号了
    updateData(DRDY[i],CS[i],outputCount);  
    delay(500);
    Serial.println();
    client.println();
  }


}

void getDeviceID(int px){
//  SPI.transfer(START);
  digitalWrite(px, LOW); //Low to communicated
  SPI.transfer(_SDATAC);
  SPI.transfer(0x20); //RREG
  SPI.transfer(0x00); //Asking for 1 byte (hopefully 0b???11110)
  byte temp = SPI.transfer(0x00);
  digitalWrite(px, HIGH); //Low to communicated
  
  Serial.print("   ID:");
  Serial.println(temp, HEX);
  client.print("   ID:");
  client.println(temp, HEX);

}

void updateData(int DRDY,int CS,int outputCount){
    
    if(digitalRead(DRDY) == LOW){

        digitalWrite(CS, LOW);
//        long output[100][9];
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
        digitalWrite(CS, HIGH);
        Serial.print(outputCount);
        Serial.print(", ");
        client.print(outputCount);
        client.print(", ");
        for (int i=0;i<9; i++) {
            Serial.print(output[i], HEX);
            client.print(output[i], HEX);
            if(i!=8) {
              Serial.print(", ");
              client.print(", ");
            }
        }
        Serial.println();
        client.println();
        outputCount++;
        
    }
}
