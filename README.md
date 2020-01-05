# punt

*punt* is a small USB bootloader for STM32 microcontrollers, designed to fit into 2 KiB flash. Currently, only STM32F103x8 devices are supported. A Rust library for communicating with the bootloader can be found [here](https://github.com/fruchti/punt-rs).

## Aim

Consider this project, at this point, as very experimental. Apart from that, *punt* is not meant to be a secure bootloader in any case. There is no encryption nor signing planned so it can be as small as possible. Without any safety and security features like these, full access to the microcontroller via its bootloader is always possible and reliable operation with unknown input cannot be guaranteed anyway.

## Licence

The header files in the third_party directory are provided by ARM Limited and ST Microelectronics and contain their own licence information. Everything else is ISC licenced.
