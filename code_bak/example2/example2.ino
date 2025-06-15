#include <Wire.h>

#define I2C_SDA 4 // GPIO4 for SDA
#define I2C_SCL 5 // GPIO5 for SCL
#define NT3H2111_ADDRESS 0x55 // NT3H2111 I2C address
#define PAGE_SIZE 16 // 每页 16 字节
#define TOTAL_PAGES 64 // 总页数 64 页

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!nt3h2111_check()) {
    Serial.println("NT3H2111 not found");
    while (1); // 停止运行
  } else {
    Serial.println("NT3H2111 detected");
  }

  uint8_t data_to_write[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00};
  nt3h2111_write_page(1, data_to_write, sizeof(data_to_write));
  delay(100);

  // 读取所有页的数据
  for (uint8_t page = 0; page < TOTAL_PAGES; page++) {
    readPage(page);
  }
}

void loop() {
  // 主循环为空
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
}
