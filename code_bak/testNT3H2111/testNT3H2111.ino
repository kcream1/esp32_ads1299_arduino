#include <Wire.h>

#define I2C_SDA 4 // GPIO4 for SDA
#define I2C_SCL 5 // GPIO5 for SCL
#define NT3H2111_ADDR 0x55 // NT3H2111 I2C address (assuming I2C_ADDR pin is low)
#define PAGE_SIZE 16 // Each page has 16 bytes
#define TOTAL_PAGES 64 // NT3H2111 has 64 pages

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!nt3h2111_check()) {
    Serial.println("NT3H2111 not found");
    while (1);
  } else {
    Serial.println("NT3H2111 detected");
  }
}

void loop() {
  uint8_t data[PAGE_SIZE];

  // 循环读取所有页
  for (uint8_t page = 0; page < TOTAL_PAGES; page++) {
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

  delay(5000); // 每次读取所有页面后等待5秒
}

bool nt3h2111_check() {
  Wire.beginTransmission(NT3H2111_ADDR);
  return (Wire.endTransmission() == 0);
}

void nt3h2111_read_page(uint8_t page, uint8_t *data, size_t len) {
  Wire.beginTransmission(NT3H2111_ADDR);
  Wire.write(page * PAGE_SIZE); // 每页16字节
  Wire.endTransmission();

  Wire.requestFrom(NT3H2111_ADDR, len);
  for (size_t i = 0; i < len; i++) {
    if (Wire.available()) {
      data[i] = Wire.read();
    } else {
      data[i] = 0; // 如果没有数据可读，则填充0
    }
  }
}
