#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define SCK_PIN   4
#define MISO_PIN  5
#define MOSI_PIN  6

// Module 1
#define CE_PIN1   20
#define CSN_PIN1  21

// Module 2
#define CE_PIN2   7
#define CSN_PIN2  10

#define LED_BUILTIN 8  // LED built-in trên ESP32-C3 (GPIO 8)

SPIClass spi(FSPI);

// Khởi tạo 2 module RF24
RF24 radio1(CE_PIN1, CSN_PIN1);
RF24 radio2(CE_PIN2, CSN_PIN2);

unsigned long lastBlink = 0;
bool ledState = false;
bool module1Connected = false;
bool module2Connected = false;
bool anyModuleConnected = false;
unsigned long errorBlinkTime = 0;
int errorBlinkState = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  // Cấu hình LED built-in
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  spi.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  Serial.println("Dual NRF24L01+ Bluejammer by Duc211");
  Serial.println("=== Module 1 ===");
  Serial.println("CE:20, CSN:21");
  Serial.println("=== Module 2 ===");
  Serial.println("CE:7, CSN:10");
  Serial.println("===SPI nối chung===");
  Serial.println("SCK:4, MISO:5, MOSI:6");
  Serial.println("Tiến hành phá sóng 2.4Ghz nếu nhận được NRF24");
  Serial.println("Đang chờ RF24...");

  // Blink LED nhanh khi đang khởi tạo
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }

  // Kiểm tra module 1
  Serial.println("Kiểm tra module 1...");
  if (!radio1.begin(&spi)) {
    Serial.println("Module 1: KHÔNG nhận được NRF24!");
    module1Connected = false;
  } else {
    Serial.println("Module 1: Đã kết nối thành công!");
    module1Connected = true;
    setupModule(radio1);
  }

  // Kiểm tra module 2
  Serial.println("Kiểm tra module 2...");
  if (!radio2.begin(&spi)) {
    Serial.println("Module 2: KHÔNG nhận được NRF24!");
    module2Connected = false;
  } else {
    Serial.println("Module 2: Đã kết nối thành công!");
    module2Connected = true;
    setupModule(radio2);
  }

  anyModuleConnected = module1Connected || module2Connected;

  if (!anyModuleConnected) {
    Serial.println("=== LỖI: KHÔNG CÓ MODULE NRF24 NÀO ĐƯỢC KẾT NỐI! ===");
    Serial.println("Kiểm tra dây nối");
    Serial.println("Module 1: CE:20, CSN:21");
    Serial.println("Module 2: CE:7, CSN:10");
    Serial.println("===SPI nối chung===");
    Serial.println("SCK:4, MISO:5, MOSI:6");
    errorBlinkTime = millis();
  } else {
    Serial.println("=== Bắt đầu phá sóng với các module đã kết nối ===");
    if (module1Connected) Serial.println("Module 1: Đang phá sóng");
    if (module2Connected) Serial.println("Module 2: Đang phá sóng");

    randomSeed(analogRead(0));
    // LED sáng liên tục khi đang hoạt động
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void setupModule(RF24 &radio) {
  radio.setAutoAck(false);
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_250KBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setChannel(0);
  radio.stopListening();
  radio.startConstCarrier(RF24_PA_MAX, 45);
  radio2.setAutoAck(false);
  radio2.setRetries(0, 0);
  radio2.setPALevel(RF24_PA_MAX, true);
  radio2.setDataRate(RF24_250KBPS);
  radio2.setCRCLength(RF24_CRC_DISABLED);
  radio2.setChannel(0);
  radio2.stopListening();
  radio2.startConstCarrier(RF24_PA_MAX, 45);
}

void loop() {
  // Nếu không có module nào được kết nối -> nháy LED báo lỗi
  if (!anyModuleConnected) {
    unsigned long currentMillis = millis();
    if (currentMillis - errorBlinkTime >= 500) {
      errorBlinkTime = currentMillis;
      errorBlinkState = !errorBlinkState;
      digitalWrite(LED_BUILTIN, errorBlinkState);
    }
    return; // Thoát loop, không thực hiện phá sóng
  }

  // Chọn channel ngẫu nhiên
  byte channel1 = random(1, 126);
  byte channel2 = random(1, 126);

  // Set channel cho cả 2 module (nếu đã kết nối)
  if (module1Connected) {
    radio1.setChannel(channel1);
  }
  if (module2Connected) {
    radio2.setChannel(channel2);
  }

  delayMicroseconds(40);
  //Sáng liên tục để đảm bảo đang phá sóng
  unsigned long currentMillis = millis();
  if (currentMillis - lastBlink >= 0) {
    lastBlink = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
}
