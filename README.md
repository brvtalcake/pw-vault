# pw-vault

A random personal project on my Raspberry Pi PICO W.

> [!NOTE]  
> While this project may be used as inspiration / other, I never planned to make this usable for anyone other than me. So keep in mind that it may not even compile on your computer, since I configured it for myself, and for my own GNU ARM toolchain.

The ultimate goal for this project (on the ["pico-side"](./pico_side/)) is to build an application capable of receiving compromising data (for instance a password, hence the name `pw-vault`), via Bluetooth, Wi-Fi, or USB, and to store it in the available flash memory or an SD card connected via QSPI interface. The transfer would be optionally encrypted with AES, and stored (non-optionally) after being encrypted with AES again, if not already done.

On the ["host-side"](./host_side/) (maybe not the best wording, but I had no idea how to call it), there would be a command-line application (for Linux computers) and an app for Android, to communicate with the PICO. And maybe also an application to handle the custom filesystem supposed to reside on the SD card, if one day I need to directly look at its content via my computer.

## Licensing

All of this is licensed under the [MIT license](./LICENSE).
