// Arduino9x_TX

#include <SPI.h>                    // SPI库
#include <RH_RF95.h>                // RFM95库

#define RFM95_CS  5                 // CS引脚
#define RFM95_RST 13                // RST引脚
#define RFM95_INT 27                // G0(IRQ)引脚
#define RF95_FREQ 915.0             // 无线电频率
#define TIME_TO_SLEEP  5            // ESP32 睡眠的时间（以秒为单位）
#define uS_TO_S_FACTOR 1000000ULL   // 微秒到秒的转换系数

RH_RF95 rf95(RFM95_CS, RFM95_INT);  // 创建无线电对象

int16_t packetnum = 0;              // 记录发送次数

// 初始化函数
void setup() {
  Serial.begin(115200);               // 设置串口波特率
  delay(100);                       // 延时100ms

  pinMode(RFM95_RST, OUTPUT);       // 引脚设为输出模式
  digitalWrite(RFM95_RST, HIGH);    // 引脚的电平设置为高
  Serial.println("Arduino LoRa TX Test!");    // 串口打印

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

  Serial.println("\nSending to rf95_server");

  // 整理需要发送的字符串
  int Value = random(0,100);        // 产生随机数
  char radiopacket[12] = "Value: ";
  itoa(Value, radiopacket + 7, 10); // 数字转字符
  Serial.print("Sending ");
  Serial.println(radiopacket);
  radiopacket[11] = 0;

  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);                // 发送数据

  Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();  // 等待无线电传输完成
  delay(1000);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); //配置唤醒源，将ESP32设置为每5秒唤醒一次
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
}
