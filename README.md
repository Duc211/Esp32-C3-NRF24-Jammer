#ĐANG THỬ NGHIỆM NÊN HIỆN TẠI CHƯA CÓ FIRMWARE !!

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

Thấy hay thì cho 1 sao

