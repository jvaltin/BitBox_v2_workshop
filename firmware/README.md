Bitbox v2 Firmware
==================

# Work in progress

### Instructions

Dependencies:
- [HIDAPI](https://github.com/signal11/hidapi)
- [GNU ARM Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
- [SEGGER J-Link OSX download page](https://www.segger.com/downloads/jlink/JLink_MacOSX_V630d.pkg)
- cmake
- git

Connect the J-Link to the debug pins on the BitBox v2 prototype board.

Plug in both the J-Link hardware and the BitBox v2 device into USB ports on your computer or a hub connected to your computer.

Build the firmware:
```
git clone https://github.com/shiftdevices/firmware_v2 && cd firmware_v2
mkdir build && cd build
cmake .. -DBUILD_TYPE=firmware  #  requires a GNU ARM toolchain installed
make
```

Load the firmware:
```
cd build
/Applications/SEGGER/JLink/JLinkExe # starts JLink binary by command line
    connect
        device ATSAMD51J20
        S (TIF SWD)
        <enter> (default speed 4000 kHz)
    loadfile bin/firmware.bin
    r (reset; stops MCU)
    g (go; starts MCU)
```
