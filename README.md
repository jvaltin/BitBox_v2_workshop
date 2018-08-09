# Defcon 2018 - Hack on Bitbox Workshop

This is a simple tutorial on how to develop the BitBox device. You will learn how to compile the firmware, flash the device and modify the code to do your own experiments. Enjoy :-)

## Step 5: Modify the code


## Step 7: Build the firmware

```
cd ~/defcon_workshop
mkdir build
cd build
cmake .. -DBUILD_TYPE=firmware -DHARDWARE=v2
```

## Step 8: Flash the device

**Attention Mac users:**

Since docker cannot run with privileged mode under OSX, you need to switch to your host system and flash the firmware from there.
To do so, download and install the JLink software from [here](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack).

To flash the device, use the `JLinkExe` command and enter the following when requested:

```
Type "connect" to establish a target connection, '?' for help
> connect

Please specify device / core. <Default>: Unspecified
> ATSAMD51J20

Please specify target interface:
  J) JTAG (Default)
  S) SWD
  T) cJTAG
> S

> loadfile bin/firmware.bin

... device is flashed ...

O.K.
> r

> g
```

## Step 9: Continue hacking

Check out the next branch [02-touchsensor](https://github.com/shiftdevices/defcon_workshop/tree/02-touchsensor) to continue...
