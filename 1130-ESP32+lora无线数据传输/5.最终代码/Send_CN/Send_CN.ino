// Arduino9x_TX

#include <DHT.h>                    // 温湿度传感器库
#include <SPI.h>                    // SPI库
#include <RH_RF95.h>                // RFM95库

#define DHTPIN    12                // 温湿度传感器信号引脚
#define RFM95_CS  5                 // CS引脚
#define RFM95_RST 13                // RST引脚
#define RFM95_INT 27                // G0(IRQ)引脚
#define RF95_FREQ 915.0             // 无线电频率
#define TIME_TO_SLEEP  5            // ESP32 睡眠的时间（以秒为单位）
#define uS_TO_S_FACTOR 1000000ULL   // 微秒到秒的转换系数

DHT dht(DHTPIN, DHT11);             // 创建传感器对象
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

  /*dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;}

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); //配置唤醒源，将ESP32设置为每5秒唤醒一次
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();*/
}

void loop() {
  Serial.println("Sending to rf95_server");

  // 整理需要发送的字符串
  char radiopacket[20] = "Hello World #      ";
  itoa(packetnum++, radiopacket + 13, 10);
  Serial.print("Sending ");
  Serial.println(radiopacket);
  radiopacket[19] = 0;

  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);                // 发送数据

  Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();  // 等待无线电传输完成
  delay(1000);

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];     // 定义数组接收数据
  uint8_t len = sizeof(buf);                // 定义变量存储数组长度
  Serial.println("Waiting for reply...");
  delay(10);

  if (rf95.waitAvailableTimeout(1000)) {      // 如果在1000ms内有收到返回的数据
    if (rf95.recv(buf, &len)) {               // 数据接收
      Serial.print("Got reply: ");
      Serial.println((char*)buf);             // 打印接收到的数据
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);   // 打印信号强度(-15 —— -100)
    } else {                                  // 否则
      Serial.println("Receive failed");       // 打印接收失败
    }
  } else {                                    // 如果在1000ms内没有收到返回的数据
    Serial.println("No reply, is there a listener around?");
  }
}
