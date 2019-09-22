
# PAx5

## Note

This repository is archived because I am building a version with better integration
in the [pax-devices](https://github.com/CalinRadoni/pax-devices) repository.

## Description

__PAx5__ is a home automation platform designed to be secure, stable and powerful.

__#__ Hardware is centered around an ultra-low-power ARM Cortex-M0+ and a sub-GHz radio.

__#__ For security:
- AEAD (_Authenticated Encryption with Associated Data_) using the 128 or 256 bits key ChaCha20 algorithm (the _new_ protocol used in HTTPS, TLS, OpenSSL, ...);
- Unique, random, session keys;
- Communication protocol designed to withstand the known attacks against these type of communications.

__#__ Stability is increased with:
- the radio module's power is generally enough to not require intermediary routers;
- designed for high availability, redundant, gateways.

### Dependencies

CMSIS

### Hardware details

MCU: STM32L051xx
Radio: RFM69HW
SPI FLASH Memory

Rendering and pinouts are in `Docs/PAx5_Base` and in `Docs/PAx5_EnvSensor`.

Environment sensor rendering:
![env-sensor](https://github.com/CalinRadoni/PAx5/blob/master/Docs/PAx5_EnvSensor/PAx5_EnvSensorFB.png)

and a photo of the sensor and base board:
![env-sensor](https://github.com/CalinRadoni/PAx5/blob/master/Docs/PAx5EnvSensorAndBaseBoard.png)

### Development Environment

Main tools used are:
- [KiCad EDA](http://kicad-pcb.org/)
- [Atollic TrueSTUDIO](https://atollic.com/truestudio/) Free Edition
- [Atom](https://atom.io/) editor

Supplementary tools:
- [Git](https://git-scm.com/)
- [STM32CubeMX](http://www.st.com/en/development-tools/stm32cubemx.html)
- [Inkscape](https://inkscape.org/en/)

### License

PAx5's software and documentation is released under the [GNU GPLv3](http://www.gnu.org/licenses/gpl-3.0.html) License. See the __LICENSE-GPLv3.txt__ file.

PAx5's hardware schematics are licensed under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).
See the __LICENSE-CC-BY-SA-4.0.txt__ file.
