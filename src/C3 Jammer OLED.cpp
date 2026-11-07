//ESP32-C3 SUPERMINI NRF jammer by @Duc_12_34
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <nRF24L01.h>
#include <WiFi.h>
#include <RF24.h>
#include "images.h"

// ==================== CẤU HÌNH MÀN HÌNH OLED ====================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C
#define OLED_SDA       1
#define OLED_SCL       0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ==================== CẤU HÌNH NRF24 ====================
#define SCK_PIN   4
#define MISO_PIN  5
#define MOSI_PIN  6

// Module 1
#define CE_PIN1   20
#define CSN_PIN1  21

// Module 2
#define CE_PIN2   7
#define CSN_PIN2  10

// Module 3
#define CE_PIN3   2
#define CSN_PIN3  3

#define LED 8  // LED built-in trên ESP32-C3 (GPIO 8)

// ==================== CẤU HÌNH NÚT BOOT & DEEP SLEEP ====================
#define BOOT_BTN_PIN      9
#define BOOT_HOLD_MS      3000   // Giữ 3 giây
#define DEEPSLEEP_SHOW_MS 1500   // Hiển thị bitmap deepsleep 1.5 giây

SPIClass spi(FSPI);

// Khởi tạo 3 module RF24
RF24 radio1(CE_PIN1, CSN_PIN1);
RF24 radio2(CE_PIN2, CSN_PIN2);
RF24 radio3(CE_PIN3, CSN_PIN3);

unsigned long lastBlink = 0;
bool ledState = false;
bool NRF1Connected = false;
bool NRF2Connected = false;
bool NRF3Connected = false;
bool allNRFConnected = false;
bool displayReady = false;
bool langen = false;     // true=EN false=VN
unsigned long errorBlinkTime = 0;
int errorBlinkState = 0;

// ==================== BIẾN CHO UPTIME & JAMMING SCREEN ====================
unsigned long jamStartTime = 0;       // Thời điểm bắt đầu jam
bool jamStarted = false;              // Đã bắt đầu jam chưa
bool showJammingScreen = false;       // Đã chuyển sang màn hình jamming chưa (sau 1 phút)
unsigned long lastBitmapToggle = 0;   // Lần đổi bitmap gần nhất
bool bitmapWhiteState = false;        // false = 1, true = 2
int lastRenderedSecond = -1;          // Giây cuối cùng đã vẽ (tránh vẽ lại)

// Biến lưu trạng thái màn hình hiện tại để tránh vẽ lại liên tục
int lastDisplayedState = -1;

// ==================== BIẾN CHO NÚT BOOT / DEEP SLEEP ====================
bool bootBtnPrevState = HIGH;         // Trạng thái trước của nút (pull-up)
unsigned long bootBtnPressStart = 0;  // Thời điểm bắt đầu giữ nút
bool bootBtnHolding = false;          // Đang giữ nút
bool deepSleepTriggered = false;      // Đã kích hoạt deep sleep chưa
unsigned long deepSleepShowStart = 0; // Thời điểm bắt đầu hiển thị bitmap deepsleep

void setupModule(RF24 &radio);
void showBootImage();
void showStatusImage(bool m1, bool m2, bool m3);
void renderJammingScreen();
void enterDeepSleep();
void handleBootButton();
void showDeepSleepImage();

// ==================== HÀM VẼ BITMAP ====================
// Bitmap 128x64: 1024 byte
void drawBitmapHorizontal(const uint8_t *bitmap) {
  if (!displayReady || !bitmap) return;
  uint8_t *buf = display.getBuffer();
  if (!buf) return;
  memcpy_P(buf, bitmap, 1024);
  display.display();
}

// Bitmap 128x32: 512 byte, vẽ ở nửa trên
void drawBitmapTop128x32(const uint8_t *bitmap) {
  if (!displayReady || !bitmap) return;
  display.drawBitmap(0, 0, bitmap, 128, 32, SSD1306_WHITE);
}

bool i2cDevicePresent(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

void showBootImage() {
  if (!displayReady) return;
  drawBitmapHorizontal(epd_bitmap_Boot_image);
}

void showStatusImage(bool m1, bool m2, bool m3) {
  if (!displayReady) return;
  const uint8_t *bmp = nullptr;

  if (m1 && m2 && m3)      bmp = epd_bitmap_1O_2O_3O;
  else if (m1 && m2 && !m3) bmp = epd_bitmap_1O_2O_3F;
  else if (m1 && !m2 && m3) bmp = epd_bitmap_1O_2F_3O;
  else if (!m1 && m2 && m3) bmp = epd_bitmap_1F_2O_3O;
  else if (m1 && !m2 && !m3) bmp = epd_bitmap_1O_2F_3F;
  else if (!m1 && m2 && !m3) bmp = epd_bitmap_1F_2O_3F;
  else if (!m1 && !m2 && m3) bmp = epd_bitmap_1F_2F_3O;
  else                       bmp = epd_bitmap_1F_2F_3F;

  if (bmp) {
    drawBitmapHorizontal(bmp);
  }
}

// ==================== MÀN HÌNH JAMMING + UPTIME ====================
// Nửa trên (0..31): bitmap jamming_BL / jamming_WH đổi mỗi 1s
// Nửa dưới (32..63): uptime liên tục
void renderJammingScreen() {
  if (!displayReady) return;

  display.clearDisplay();

  // 1. Vẽ bitmap 128x32 ở nửa trên
  if (langen) {
  const uint8_t *bmp = bitmapWhiteState ? epd_bitmap_jamming_2_en : epd_bitmap_jamming_1_en;
  if (bmp) {
    display.drawBitmap(0, 0, bmp, 128, 32, SSD1306_WHITE);
  }
} else {
  const uint8_t *bmp = bitmapWhiteState ? epd_bitmap_jamming_2_vn : epd_bitmap_jamming_1_vn;
  if (bmp) {
    display.drawBitmap(0, 0, bmp, 128, 32, SSD1306_WHITE);
  }
}

  // 2. Vẽ uptime ở nửa dưới
  unsigned long elapsedSec = (millis() - jamStartTime) / 1000UL;
  unsigned long hh = elapsedSec / 3600UL;
  unsigned long mm = (elapsedSec % 3600UL) / 60UL;
  unsigned long ss = elapsedSec % 60UL;

  char timeBuf[16];
  snprintf(timeBuf, sizeof(timeBuf), "%02lu:%02lu:%02lu", hh, mm, ss);

  // Tiêu đề nhỏ
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 34);

  if (langen) {
  display.print("UPTIME");
  } else {
  display.print("T.GIAN HOAT DONG");
  }

  // Thời gian lớn ở giữa
  display.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(timeBuf, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 46);
  display.print(timeBuf);

  display.display();
}

// Mã trạng thái để so sánh, tránh vẽ lại
int encodeState(bool m1, bool m2, bool m3) {
  return (m1 ? 1 : 0) | (m2 ? 2 : 0) | (m3 ? 4 : 0);
}

// ==================== HIỂN THỊ BITMAP DEEP SLEEP ====================
void showDeepSleepImage() {
  if (langen) {
  if (!displayReady) return;
  drawBitmapHorizontal(epd_bitmap_deepsleep_en);
  } else {
  if (!displayReady) return;
  drawBitmapHorizontal(epd_bitmap_deepsleep_vn);
  }
}

// ==================== XỬ LÝ NÚT BOOT ====================
void handleBootButton() {
  // Nếu đã kích hoạt deep sleep thì không làm gì nữa
  if (deepSleepTriggered) return;

  bool currentState = digitalRead(BOOT_BTN_PIN);

  // Phát hiện cạnh xuống (bắt đầu nhấn)
  if (bootBtnPrevState == HIGH && currentState == LOW) {
    bootBtnPressStart = millis();
    bootBtnHolding = true;
  }

  // Đang giữ nút
  if (bootBtnHolding && currentState == LOW) {
    if (millis() - bootBtnPressStart >= BOOT_HOLD_MS) {
      // Đã giữ đủ 3 giây → kích hoạt deep sleep
      deepSleepTriggered = true;
      bootBtnHolding = false;
      deepSleepShowStart = millis();

      // Hiển thị bitmap deepsleep
      showDeepSleepImage();

      // Tắt LED để tiết kiệm pin (tùy chọn)
      digitalWrite(LED, HIGH);
    }
  }

  // Nhả nút trước khi đủ 3 giây → hủy
  if (bootBtnHolding && currentState == HIGH) {
    bootBtnHolding = false;
  }

  bootBtnPrevState = currentState;
}

// ==================== VÀO DEEP SLEEP ====================
void enterDeepSleep() {
  // Đảm bảo màn hình vẫn hiển thị bitmap deepsleep (không tắt màn hình)
  // SSD1306 vẫn giữ nội dung buffer sau khi ESP32 vào deep sleep,
  // miễn là nguồn VCC của OLED vẫn được cấp.

    if (langen) {
    Serial.println("Entering deep sleep...");
    } else {
    Serial.println("Đang vào chế độ ngủ sâu...");
    }

  // Tắt các module NRF để tiết kiệm pin
  if (NRF1Connected) { 
    radio1.stopConstCarrier();
    delay(10);
    radio1.powerDown(); 
  }

  if (NRF2Connected) { 
    radio2.stopConstCarrier();
    delay(10);
    radio2.powerDown(); 
  }

  if (NRF3Connected) { 
    radio3.stopConstCarrier();
    delay(10); 
    radio3.powerDown(); 
  }

  // Cấu hình wake-up bằng nút BOOT (GPIO9) mức thấp
  esp_deep_sleep_enable_gpio_wakeup((1ULL << BOOT_BTN_PIN), ESP_GPIO_WAKEUP_GPIO_LOW);

  // Vào deep sleep
  esp_deep_sleep_start();
}

// ==================== SETUP ====================
void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);  
  WiFi.mode(WIFI_OFF); 

  Serial.begin(115200);
  delay(100);

  // Cấu hình LED built-in
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // Cấu hình nút BOOT với pull-up nội
  pinMode(BOOT_BTN_PIN, INPUT_PULLUP);
  bootBtnPrevState = digitalRead(BOOT_BTN_PIN);

  // Khởi tạo I2C cho OLED
  Wire.begin(OLED_SDA, OLED_SCL, 100000);
  delay(100);

  // 1. Quét I2C xem có device ở địa chỉ 0x3C không
  bool oledPresent = i2cDevicePresent(OLED_ADDR);

  if (!oledPresent) {
    displayReady = false;
    Wire.end();                // ← Tắt I2C để không log lỗi nữa
  } else {
    // 2. Có device trả lời → mới init display
    if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
      display.clearDisplay();
      display.display();
      displayReady = true;
    } else {
      displayReady = false;
      Wire.end();
    }
  }
   
  //Khởi tạo SPI
  spi.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

  // Hiển thị ảnh boot
  showBootImage();
  lastDisplayedState = -1;
  
  if (langen) {
  Serial.println("NRF JAMMER by @Duc_12_34");   
  Serial.println("Waiting NRF24...");           
  } else {
  Serial.println("NRF JAMMER BY @Duc_12_34");   
  Serial.println("Đang chờ NRF24...");          
  }

/*  // Blink LED nhanh khi đang khởi tạo
  for (int i = 0; i < 1; i++) { 
    digitalWrite(LED, LOW);
    delay(75);
    digitalWrite(LED, HIGH);
    delay(75);
  }
*/
  // Kiểm tra module 1
  if (!radio1.begin(&spi)) {
    if (langen) {
    Serial.println("NRF 1: Not found!");
    } else {
    Serial.println("NRF 1; Không có!");
    }
    NRF1Connected = false;
  } else {
    if (langen) {
    Serial.println("NRF 1: Connected!");
    } else {
    Serial.println("NRF 1: Đã kết nối");
    }    
    NRF1Connected = true;
    setupModule(radio1);
  }

  // Kiểm tra module 2
  if (!radio2.begin(&spi)) {
    if (langen) {
    Serial.println("NRF 2: Not found!");
    } else {
    Serial.println("NRF 2: Không có!");
    }    
    NRF2Connected = false;
  } else {
    if (langen) {
    Serial.println("NRF 2: Connected!");
    } else {
    Serial.println("NRF 2: Đã kết nối");
    } 
    NRF2Connected = true;
    setupModule(radio2);    
  }

  // Kiểm tra module 3
  if (!radio3.begin(&spi)) {
    if (langen) {
    Serial.println("NRF 3: Not found!");
    } else {
    Serial.println("NRF 3: Không có!");
    }
    NRF3Connected = false;    
  } else {
    if (langen) {
    Serial.println("NRF 3: Connected!");
    } else {
    Serial.println("NRF 3: Đã kết nối");
    }
    NRF3Connected = true;
    setupModule(radio3);    
  }

  allNRFConnected = NRF1Connected || NRF2Connected || NRF3Connected;

  if (!allNRFConnected) {
    if (langen) {
    Serial.println("=== ERR: Not found any NRF modules! ===");
    } else {
    Serial.println("=== LỖI: Không tìm module NRF nào! ===");
    }
   errorBlinkTime = millis();
    //randomSeed(analogRead(0));
    digitalWrite(LED, LOW); 
  } else {
    if (langen) {
    Serial.println("=== Start Jamming with connected modules ===");
    if (NRF1Connected) Serial.println("NRF 1: Jamming !");
    if (NRF2Connected) Serial.println("NRF 2: Jamming !");
    if (NRF3Connected) Serial.println("NRF 3: Jamming !");
    } else {
    Serial.println("=== Bắt đầu phá sóng với module NRF đã kết nối ===");
    if (NRF1Connected) Serial.println("NRF 1: Đang phá !");
    if (NRF2Connected) Serial.println("NRF 2: Đang phá !");
    if (NRF3Connected) Serial.println("NRF 3: Đang phá !");
    }

    // Bắt đầu đếm uptime ngay khi jammer khởi động
    jamStartTime = millis();
    jamStarted = true;
    lastBitmapToggle = millis();
  }

  // Hiển thị trạng thái sau khi boot xong
  showStatusImage(NRF1Connected, NRF2Connected, NRF3Connected);
  lastDisplayedState = encodeState(NRF1Connected, NRF2Connected, NRF3Connected);
}

// ==================== CẤU HÌNH MODULE ====================
void setupModule(RF24 &radio) {
  // Thiết lập các thông số cho module NRF24
 // radio.setAutoAck(false);                      // Không kiểm tra dữ liệu
 // radio.setRetries(0, 0);                       // Không thử lại chỉ gửi 1 lần
  radio.setPALevel(RF24_PA_MAX, true);          // Công suất phát tối đa
  radio.setDataRate(RF24_250KBPS);              // Tốc độ dữ liệu 250kbps để tăng khoảng cách
 // radio.setCRCLength(RF24_CRC_DISABLED);        // Tắt CRC
 // radio.stopListening();                        // Chế độ chỉ gửi dữ liệu
  radio.startConstCarrier(RF24_PA_MAX, 45);     // Phát sóng mang kênh 45 khi bắt đầu
}

// ==================== LOOP ====================
void loop() {
  unsigned long now = millis();

  // ----- KIỂM TRA NÚT BOOT (VÔ ĐIỀU KIỆN, CHẠY TRƯỚC MỌI THỨ) -----
  handleBootButton();

  // Nếu đã kích hoạt deep sleep → hiển thị bitmap đủ 3 giây rồi vào deep sleep
  if (deepSleepTriggered) {
    if (now - deepSleepShowStart >= DEEPSLEEP_SHOW_MS) {
      enterDeepSleep();
    }
    return;  // Không làm gì khác trong thời gian chờ
  }

  // Nếu không có module nào được kết nối -> sáng LED báo lỗi
  if (!allNRFConnected) {
    if (displayReady) {
     int curState = encodeState(NRF1Connected, NRF2Connected, NRF3Connected);
      if (curState != lastDisplayedState) {
        showStatusImage(NRF1Connected, NRF2Connected, NRF3Connected);
        lastDisplayedState = curState;
      }
    }
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
    return;
  }

  // ----- XỬ LÝ MÀN HÌNH -----
  if (displayReady) {
    // Tính uptime (giây)
    unsigned long elapsedSec = (now - jamStartTime) / 1000UL;

    // Đủ 1 phút → chuyển sang màn hình jamming
    if (!showJammingScreen && elapsedSec >= 10UL) {
      showJammingScreen = true;
      lastBitmapToggle = now;          // reset mốc đổi bitmap
      bitmapWhiteState = false;        // bắt đầu bằng BL
      lastRenderedSecond = -1;         // buộc vẽ lại
      if (displayReady) display.clearDisplay();
    }

    if (showJammingScreen) {
      // Đổi bitmap mỗi 1 giây
      if (now - lastBitmapToggle >= 500UL) {
        lastBitmapToggle = now;
        bitmapWhiteState = !bitmapWhiteState;
      }

      // Chỉ vẽ lại khi giây thay đổi (giảm tải I2C) hoặc vừa đổi bitmap
      int curSec = (int)(elapsedSec % 60);
      static bool lastWhiteState = false;
      if (curSec != lastRenderedSecond || bitmapWhiteState != lastWhiteState) {
        lastRenderedSecond = curSec;
        lastWhiteState = bitmapWhiteState;
        renderJammingScreen();
      }
    } else {
      // Trước 1 phút: hiển thị ảnh trạng thái module như cũ
      int curState = encodeState(NRF1Connected, NRF2Connected, NRF3Connected);
      if (curState != lastDisplayedState) {
        showStatusImage(NRF1Connected, NRF2Connected, NRF3Connected);
        lastDisplayedState = curState;
      }
    }
  }

  // ----- NHẢY KÊNH NRF -----
  if (NRF1Connected) {
   // for (int i = 1; i < 125; i++) {
   //   radio1.setChannel(i);
   // delayMicroseconds(60);
   // }
    radio1.setChannel(random(0, 126));
    delayMicroseconds(55);
  }

  if (NRF2Connected) {
   // for (int i2 = 0; i2 < 65; i2++) {
   //   radio2.setChannel(i2);
   //  delayMicroseconds(60);
   // }
    radio2.setChannel(random(0, 126));
    delayMicroseconds(55);
  }

  if (NRF3Connected) {
    for (int i3 = 0; i3 < 126; i3++) {
      radio3.setChannel(i3);
      delayMicroseconds(130);
    }
    //radio3.setChannel(random(0, 126));
    //delayMicroseconds(55);
  }

  // Nháy LED báo đang phá sóng
  if (now - lastBlink >= 2000) {
    lastBlink = now;
    ledState = !ledState;
    digitalWrite(LED, ledState);
  }
}