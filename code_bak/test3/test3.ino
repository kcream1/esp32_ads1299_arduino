#include <Wire.h>

#define I2C_SDA 4 // GPIO4 for SDA
#define I2C_SCL 5 // GPIO5 for SCL
#define LSM303AGR_ADDR 0x19 // LSM303AGR accelerometer address

void setup() {
  // 初始化串口用于调试输出
  Serial.begin(115200);

  // 初始化I2C接口
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // 初始化LSM303AGR传感器
  lsm303agr_init();
}

void loop() {
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

  // 每20ms读取一次数据，对应50Hz采样率
  delay(100);
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

