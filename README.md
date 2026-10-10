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


                        +----Nguồn điện-----+
                 -----> |3.3V & GND---------|---> cấp nguồn cho màn OLED
                 |      |                   |
     Nguồn 5V1A -+----> |5V & GND-----------|---> cấp nguồn cho ESP32-C3 
                 |      |                   |
                 ---->  |5V & GND---> AMS1117 3.3--> cấp nguồn cho 3 con NRF24 (nên thêm tụ 100uf)
                        +-------------------+

<img width="625" height="537" alt="ảnh" src="https://github.com/user-attachments/assets/0c70aca3-9c2d-4ca4-b0fd-6101072c2bd0" />
