
#include "ads1299.h"


// 定义WiFi凭据
const char* ssid = "FAST_DDD9"; // 替换为你的WiFi SSID
const char* password = "pystzw123456"; // 替换为你的WiFi密码

// 定义TCP服务器地址和端口
const char* host = "192.168.1.107"; // 替换为你想要连接的服务器地址
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
  // 初始化I2C接口
  Wire.begin(I2C_SDA, I2C_SCL);
  
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
  
  // 初始化LSM303AGR传感器
  lsm303agr_init();

  //ads1299初始化
  ads1299init();

  //初始化nt3h2111
  if (!nt3h2111_check()) {
    Serial.println("NT3H2111 not found");
    client.println("NT3H2111 not found");
    while (1); // 停止运行
  } else {
    Serial.println("NT3H2111 detected");
    client.println("NT3H2111 detected");
  }
  //写入
  uint8_t data_to_write[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00};
  nt3h2111_write_page(1, data_to_write, sizeof(data_to_write));
  delay(100);

  // 读取所有页的数据
  for (uint8_t page = 0; page < TOTAL_PAGES; page++) {
    readPage(page);
  }
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
    delay(10);
    WREG(CONFIG2,0xd0,CS[i]);
    delay(10);

    WREG(CH1SET,0x55,CS[i]);
    delay(10);
    WREG(CH2SET,0x55,CS[i]);
    delay(10);
    WREG(CH3SET,0x55,CS[i]);
    delay(10);
    WREG(CH4SET,0x55,CS[i]);
    delay(10);
    WREG(CH5SET,0x55,CS[i]);
    delay(10);
    WREG(CH6SET,0x55,CS[i]);
    delay(10);
    WREG(CH7SET,0x55,CS[i]);
    delay(10);
    WREG(CH8SET,0x55,CS[i]);
    delay(10);

    START(CS[i]);
    delay(10);
    RDATAC(CS[i]);
    delay(10);
    //这里就可以等待DRDY发出信号了
    updateData(DRDY[i],CS[i],outputCount);  
    delay(10);
    Serial.println();
    client.println();

    int16_t acc_x, acc_y, acc_z;
    // 读取加速度数据
    lsm303agr_read_acceleration(&acc_x, &acc_y, &acc_z);
    // 打印加速度数据
    Serial.print("X: ");
    Serial.print(acc_x);
    Serial.print(" Y: ");
    Serial.print(acc_y);
    Serial.print(" Z: ");
    Serial.println(acc_z);
    Serial.println();
    client.print("X: ");
    client.print(acc_x);
    client.print(" Y: ");
    client.print(acc_y);
    client.print(" Z: ");
    client.println(acc_z);
    client.println();
    delay(500);
  }


}


bool nt3h2111_check() {
  Wire.beginTransmission(NT3H2111_ADDRESS);
  return (Wire.endTransmission() == 0);
}

void nt3h2111_write_page(uint8_t page, uint8_t *data, size_t len) {
  Wire.beginTransmission(NT3H2111_ADDRESS);
  Wire.write(page * 16); // 每页16字节
  for (size_t i = 0; i < len; i++) {
    Wire.write(data[i]);
  }
  Wire.endTransmission();
}

void readPage(uint8_t page) {
  uint8_t buffer[PAGE_SIZE];

  Wire.beginTransmission(NT3H2111_ADDRESS);
  Wire.write(page);
  Wire.endTransmission();

  Wire.requestFrom(NT3H2111_ADDRESS, PAGE_SIZE);
  for (uint8_t i = 0; i < PAGE_SIZE; i++) {
    if (Wire.available()) {
      buffer[i] = Wire.read();
    }
  }

  Serial.print("Page ");
  Serial.print(page);
  Serial.print(": ");
  for (uint8_t i = 0; i < PAGE_SIZE; i++) {
    if (buffer[i] < 0x10) Serial.print("0"); // 补零
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  client.print("Page ");
  client.print(page);
  client.print(": ");
  for (uint8_t i = 0; i < PAGE_SIZE; i++) {
    if (buffer[i] < 0x10) client.print("0"); // 补零
    client.print(buffer[i], HEX);
    client.print(" ");
  }
  client.println();
}

void lsm303agr_init() {
  // 写入0x47到寄存器0x20，设置加速度计50Hz输出数据速率和正常工作模式
  // 设置CTRL_REG1_A寄存器 (0x20):
  // - 数据速率为50Hz (ODR3, ODR2, ODR1, ODR0 = 0100)
  // - 打开所有轴 (Xen, Yen, Zen = 1)
  lsm303agr_write_byte(0x20, 0x47);
}

void lsm303agr_write_byte(uint8_t reg_addr, uint8_t data) {
  Wire.beginTransmission(LSM303AGR_ADDR);//加速度计地址0x19
  Wire.write(reg_addr);
  Wire.write(data);
  Wire.endTransmission();
}

void lsm303agr_read_bytes(uint8_t reg_addr, uint8_t *data, size_t len) {
  Wire.beginTransmission(LSM303AGR_ADDR);
  Wire.write(reg_addr | 0x80); //从指定寄存器地址开始读取指定长度的数据。地址 0x28 | 0x80 中的 0x80 表示自动递增地址，以便连续读取多个寄存器的数据。
  Wire.endTransmission();
  
  Wire.requestFrom(LSM303AGR_ADDR, len);
  for (size_t i = 0; i < len; i++) {
    data[i] = Wire.read();
  }
}

void lsm303agr_read_acceleration(int16_t *acc_x, int16_t *acc_y, int16_t *acc_z) {
  uint8_t data[6];
  lsm303agr_read_bytes(0x28, data, 6);//从地址 0x28 开始读取6个字节的数据，这些数据包含 X、Y 和 Z 轴的加速度数据

  *acc_x = (int16_t)((data[1] << 8) | data[0]);
  *acc_y = (int16_t)((data[3] << 8) | data[2]);
  *acc_z = (int16_t)((data[5] << 8) | data[4]);
}



void ads1299init(void)
{

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
