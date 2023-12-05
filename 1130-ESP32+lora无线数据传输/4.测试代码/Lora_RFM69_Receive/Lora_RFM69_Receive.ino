#include <SPI.h>
#include <RH_RF69.h>

RH_RF69 rf69;

void setup() {
  Serial.begin(115200);
  if (!rf69.init())
    Serial.println("init failed");
  if (!rf69.setFrequency(915.0))
    Serial.println("setFrequency failed");

  // 加密密钥必须与服务器中的密钥相同
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  rf69.setEncryptionKey(key);

  Serial.println("ok");
}

void loop() {
  if (rf69.available()) {
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      RH_RF69::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);

      uint8_t data[] = "And hello back to you";
      rf69.send(data, sizeof(data));
      rf69.waitPacketSent();
      Serial.println("Sent a reply");
    } else {
      Serial.println("recv failed");
    }
  }

  delay(400);
}
