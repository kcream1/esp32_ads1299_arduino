#include <Wire.h>

#define I2C_SDA 4 // GPIO4 for SDA
#define I2C_SCL 5 // GPIO5 for SCL
#define NT3H2111_ADDR 0x55 // NT3H2111 I2C address (assuming I2C_ADDR pin is low)

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  // 检查芯片是否响应
  if (!nt3h2111_check()) {
    Serial.println("NT3H2111 not found");
    while (1);
  } else {
    Serial.println("NT3H2111 detected");
  }

  // 写入数据到 EEPROM 页 4
  //uint8_t data_to_write[16] = {0x12, 0x11, 0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0};
  uint8_t data_to_write[16] = {0x70, 0x79, 0x73, 0x74, 0x7a, 0x77, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00, 0x00, 0x00, 0x0};
  nt3h2111_write_page(4, data_to_write, sizeof(data_to_write));
}

void loop() {
  uint8_t data[16];
  for (uint8_t page = 0; page < 64; page++) {
    nt3h2111_read_page(page, data, sizeof(data));
    Serial.print("Page ");
    Serial.print(page);
    Serial.print(": ");
    for (int i = 0; i < sizeof(data); i++) {
      if (data[i] < 0x10) {
        Serial.print("0"); // 补零，确保每个字节有两位
      }
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(100); // 每读取一页延迟100毫秒
  }


  // uint8_t data[16];
  // uint8_t data0[16];

  // // 读取 EEPROM 页 4 的数据
  // nt3h2111_read_page(0, data0, sizeof(data0));

  // // 打印读取的数据
  // Serial.print("Page 0 Data: ");
  // for (int i = 0; i < sizeof(data0); i++) {
  //   Serial.print(data0[i], HEX);
  //   Serial.print(" ");
  // }
  // Serial.println();

  // // 读取 EEPROM 页 4 的数据
  // nt3h2111_read_page(4, data, sizeof(data));

  // // 打印读取的数据
  // Serial.print("Page 4 Data: ");
  // for (int i = 0; i < sizeof(data); i++) {
  //   Serial.print(data[i], HEX);
  //   Serial.print(" ");
  // }
  // Serial.println();

  delay(8000); // 每秒读取一次
}

bool nt3h2111_check() {
  Wire.beginTransmission(NT3H2111_ADDR);
  return (Wire.endTransmission() == 0);
}

void nt3h2111_write_page(uint8_t page, uint8_t *data, size_t len) {
  Wire.beginTransmission(NT3H2111_ADDR);
  Wire.write(page * 16); // 每页16字节
  for (size_t i = 0; i < len; i++) {
    Wire.write(data[i]);
  }
  Wire.endTransmission();
}

void nt3h2111_read_page(uint8_t page, uint8_t *data, size_t len) {
  Wire.beginTransmission(NT3H2111_ADDR);
  Wire.write(page * 16); // 每页16字节
  Wire.endTransmission();

  Wire.requestFrom(NT3H2111_ADDR, len);
  for (size_t i = 0; i < len; i++) {
    data[i] = Wire.read();
  }
}
