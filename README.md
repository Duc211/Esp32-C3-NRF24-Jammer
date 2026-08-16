Đây là code bluejammer mà tôi đã làm riêng cho esp32 c3 supermini (tôi chọn board LOLIN C3 mini)
Code này có support cả 2nrf và 1 nrf đối với phiên bản V3
đây là flash offset cho ae


bootloader.bin   -  0x0

partitions.bin   -  0x8000

boot_app0.bin    -  0xe000 (có thể không cần cái này)

firmware.bin     -  0x10000


Tôi không có web flasher riêng nên ae tải file .bin sau đó tự flash

sơ đồ nối như sau (dành cho phiên bản V3)

SPI của nrf24 đều nối chung


(NRF24) SCK  -  GPIO4 (esp32)

(NRF24) MISO -  GPIO5 (esp32)

(NRF24) MOSI -  GPIO6 (esp32)


Nối riêng


(NRF24 1) CE  -  GPIO20 (esp32)

(NRF24 1) CSN -  GPIO21 (esp32)


(NRF24 2) CE  - GPIO7  (esp32)

(NRF24 2) CSN - GPIO10 (esp32)


Cách hoạt động 

Khi đèn màu xanh trên board nháy nhanh (khoảng 5 lần nháy) là đang boot và kiểm tra nrf24

khi đèn nháy chậm có nghĩa là không có bất kỳ nrf24 nào đc kết nối và setup

Khi đèn sáng liên tục tức là đã có nrf24 và đang phá sóng (kiểm tra trên serial để biết chính xác esp32 đã kết nối đc với nrf24 nào)

Code này sẽ làm ngập toàn bộ băng thông 2.4GHz với tốc độ rất nhanh

Các thiết bị ảnh hưởng có thể là tai nghe bluetooh,chuột không dây,wi-fi 2.4GHz hay bất kỳ thiết bị nào chạy trong băng tần 2.4GHz
