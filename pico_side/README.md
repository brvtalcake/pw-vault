# PICO-side

This subfolder regroup all the pieces of software that are meant to run on the Raspberry pi pico.

## Subfolder summary

- [picoutil](./picoutil/): A library regrouping utility macros and multiple "modules"
- [picoflash](./picoflash/): A library where I will implement a custom flash filesystem, to store the needed data. This will be the library at the heart of the application
- [picowireless](./picowireless/): A library to facilitate Wi-Fi and Bluetooth data transfer with other devices
- [src](./src/): The source code of the application
- [aes_test](./aes_test/): A reference implementation ([tiny-AES](https://github.com/kokke/tiny-AES-c)) to compare the results of my own AES implementation against
