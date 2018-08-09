# Defcon 2018 - Hack on Bitbox Workshop

This is a simple tutorial on how to develop the BitBox device. You will learn how to compile the firmware, flash the device and modify the code to do your own experiments. Enjoy :-)

## Step 1: Setup

To get started, get the Fedora live USB containing the build environment or install the docker image. We recommend using the live USB, because there is no need to download anything. Also, if you're on Mac, you might not be able to run docker in privileged mode. If you prefer docker anyway, for example because you do not want to reboot, follow these steps:

### Get the defcon_workshop code

```
git clone https://github.com/digitalbitbox/defcon_workshop.git
cd defcon_workshop
```

### Run the docker image

```
docker run --privileged -v /dev/bus/usb:/dev/bus/usb -it -e HOST_USER_ID=$(id -u) -e HOST_USER_GID=$(id -g) -v <PATH_TO_DEFCON_WORKSHOP_REPO>:/home/dockeruser/defcon_workshop shiftcrypto/defcon_workshop
```

## Step 2: Build the firmware

```
cd ~/defcon_workshop/firmware
mkdir build
cd build
cmake ..
```

## Step 3: Flash the device

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

**Congratulations**, you compiled and deployed your first BitBox firmware code!

## Step 4: Continue hacking

Check out the next branch [01-helloworld](https://github.com/shiftdevices/defcon_workshop/tree/01-helloworld) to continue...
