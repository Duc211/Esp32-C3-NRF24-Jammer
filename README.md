#ĐANG THỬ NGHIỆM NÊN HIỆN TẠI CHƯA CÓ FIRMWARE !!

 ⚠️ Cảnh báo pháp lý — ĐỌC CHO KỸ VÀO

Việc gây nhiễu sóng vô tuyến là hành vi vi phạm pháp luật tại Việt Nam:

Luật Tần số vô tuyến điện 2009 — Điều 8: cấm gây nhiễu có hại

Theo nghị định 15/2020/NĐ-CP — phạt tiền lên đến 50 triệu đồng  

Bộ luật Hình sự — Điều 285: tội cản trở thông tin liên lạc, phạt tù đến 5 năm 

Thiết bị này chỉ nên dùng cho:

✅ Nghiên cứu bảo mật trong phòng thí nghiệm cách ly

✅ Học tập về giao thức 2.4GHz, NRF24, ESP32, BLUETOOTH

✅ Kiểm tra thiết bị của chính mình trong phòng kín

❌ KHÔNG gây nhiễu WiFi tại nơi công cộng, thiết bị của người khác

❌ KHÔNG dùng ở nơi công cộng, gần sân bay, bệnh viện 

Bạn hoàn toàn chịu trách nhiệm pháp lý về việc sử dụng FW này. 

                             === Nối dây ===
                          +--------------------+   
     SPI BUS (Chung) ---> | GPIO4  (SCK)       |---> SCK của 3x nRF24
                          | GPIO6  (MISO)      |<--- MISO của 3x nRF24
                          | GPIO5  (MOSI)      |---> MOSI của 3x nRF24
                          |                    |
     nRF24 #1 ----------> | GPIO20 (CE1)       |---> CE  của nRF24 số 1
                          | GPIO21 (CSN1)      |---> CSN của nRF24 số 1
                          |                    |
     nRF24 #2 ----------> | GPIO7  (CE2)       |---> CE  của nRF24 số 2
                          | GPIO10 (CSN2)      |---> CSN của nRF24 số 2
                          |                    |
     nRF24 #3 ----------> | GPIO2  (CE3)       |---> CE  của nRF24 số 3
                          | GPIO3  (CSN3)      |---> CSN của nRF24 số 3
                          |                    |
     OLED 0.96" --------> | GPIO0  (SCL)       |---> SCL của màn OLED
                          | GPIO9  (SDA)       |---> SDA của màn OLED
                          |                    |
                 -------> | 3.3V & GND         |---> cấp nguồn cho màn OLED
                 |        |                    |
     Nguồn 5V1A -+------> | 5V & GND           |---> cấp nguồn cho ESP32-C3 
                 |        |                    |
                 -------> | 3.3V & GND         |---> cấp nguồn cho 3 con NRF24 (nên thêm tụ 100uf cho mỗi con)
                          +--------------------+
