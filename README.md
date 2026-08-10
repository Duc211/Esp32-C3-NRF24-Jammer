Đây là file .bin bluejammer dành cho esp32 c3 supermini
               Mã nguồn đóng (phải có mật khẩu để lấy mã nguồn)
Sơ đồ chân nối như sau:

ESP32        NRF24L01+
GPIO4  <--->   SCK
GPIO5  <--->   MISO
GPIO6  <--->   MOSI
GPIO20 <--->   CE
GPIO21 <--->   CSN

Cách flash file .bin cho ESP32 C3
frimware     0x10000
bootloader   0x0
partions     0x8000
