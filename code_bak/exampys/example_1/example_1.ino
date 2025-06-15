#include <Wire.h>
#include <WiFi.h>

#define SDA_PIN 4
#define SCL_PIN 5
#define NT3H2111_I2C_ADDRESS 0x55

char ssid[32] = {0};
char password[32] = {0};

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  // 不断读取NT3H2111中的数据直到成功连接WiFi
  while (!connectToWiFi()) {
    delay(1000); // 等待1秒后重试
  }

  // 连接成功后，开始发送数据
  sendData();
}

void loop() {
  // 空的主循环
}

bool readNFCData() {
  Wire.beginTransmission(NT3H2111_I2C_ADDRESS);
  Wire.write(0x00); // 起始地址
  Wire.endTransmission();
  Wire.requestFrom(NT3H2111_I2C_ADDRESS, 64);

  if (Wire.available() == 64) {
    for (int i = 0; i < 32; i++) {
      ssid[i] = Wire.read();
    }
    for (int i = 0; i < 32; i++) {
      password[i] = Wire.read();
    }

    // 打印读取到的SSID和密码
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    return true;
  }
  return false;
}

bool connectToWiFi() {
  if (!readNFCData()) {
    Serial.println("Failed to read NFC data");
    return false;
  }

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempts++;
    if (attempts >= 30) { // 30秒后仍未连接上，返回false
      Serial.println("\nFailed to connect to WiFi");
      return false;
    }
  }

  Serial.println("\nConnected to WiFi");
  return true;
}

void sendData() {
  const char* host = "192.168.1.107";
  const int port = 8888;
  WiFiClient client;

  while (true) {
    if (client.connect(host, port)) {
      Serial.println("Connected to TCP server");

      while (client.connected()) {
        client.print("pys");
        Serial.println("Sent: pys");
        delay(1000); // 每秒发送一次
      }
    } else {
      Serial.println("Failed to connect to TCP server");
      delay(5000); // 5秒后重试连接
    }
  }
}
