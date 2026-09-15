Đây là code bluejammer mà tôi đã làm riêng cho esp32 c3 supermini (tôi chọn board LOLIN C3 mini)
Code này có support cả 2nrf và 1 nrf
đây là flash offset cho ae


bootloader.bin   -  0x0

partitions.bin   -  0x8000

boot_app0.bin    -  0xe000 (có thể không cần cái này)

firmware.bin     -  0x10000


sơ đồ nối như sau


SPI của nrf24 đều nối chung


(NRF24) SCK  -  GPIO4 (esp32)

(NRF24) MISO -  GPIO5 (esp32)

(NRF24) MOSI -  GPIO6 (esp32)


Nối riêng


(NRF24 1) CE  -  GPIO20 (esp32)

(NRF24 1) CSN -  GPIO21 (esp32)


(NRF24 2) CE  - GPIO7  (esp32)

(NRF24 2) CSN - GPIO10 (esp32)
