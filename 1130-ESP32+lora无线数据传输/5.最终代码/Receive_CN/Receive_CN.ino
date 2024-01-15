// Arduino9x_RX

#include <SPI.h>      // SPI库
#include <RH_RF95.h>  // RFM95库

#define LEDPIN 2         // 板载LED指示灯引脚
#define DHTPIN 12        // 温湿度传感器信号引脚
#define RFM95_CS 5       // CS引脚
#define RFM95_RST 13     // RST引脚
#define RFM95_INT 27     // G0(IRQ)引脚
#define RF95_FREQ 915.0  // 无线电频率

RH_RF95 rf95(RFM95_CS, RFM95_INT);  // 创建无线电对象

void setup() {
  Serial.begin(115200);               // 设置串口波特率
  delay(100);                         // 延时100ms

  pinMode(LEDPIN, OUTPUT);                       // 引脚设为输出模式
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);              // 引脚的电平设置为高
  Serial.println("Arduino LoRa RX Test!");    // 串口打印

  digitalWrite(RFM95_RST, LOW);               // 手动复位
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {                      // 模块初始化
    Serial.println("LoRa radio init failed");
    while (1);}
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {        // 设置无线电频率
    Serial.println("setFrequency failed");
    while (1);}

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);                 // 设置发射功率
}

void loop() {
  if (rf95.available()) {                     // 如果有数据传入
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];     // 定义数组接收数据
    uint8_t len = sizeof(buf);                // 定义变量存储数组长度

    if (rf95.recv(buf, &len)) {               // 如果数据接收成功
      digitalWrite(LEDPIN, HIGH);             // 点亮板载LED
      Serial.print("Got: ");
      Serial.println((char*)buf);             // 打印接收到的数据
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);   // 打印信号强度(-15 —— -100)

      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));          // 发送回复信息
      rf95.waitPacketSent();                  // 等待无线电传输完成
      Serial.println("Sent a reply");
      digitalWrite(LEDPIN, LOW);              // 关闭板载LED
    } else {                                  // 否则
      Serial.println("Receive failed");       // 打印接收失败
    }
  }
}