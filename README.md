# Defcon 2018 Workshop

## Step 1: Get the code from GitHub

```
git clone https://github.com/digitalbitbox/defcon_workshop.git
cd defcon_workshop
```

## Step 2: Build the docker image

TODO: remove this step and pull from docker repo instead.

```
./before_docker_build.sh # Downloads required binary dependencies
docker build -t firmware .
```

## Step 3: Run the docker image

```
docker run --privileged -v /dev/bus/usb:/dev/bus/usb -it -e HOST_USER_ID=$(id -u) -e HOST_USER_GID=$(id -g) -v <PATH_TO_FIRMWARE_V2_REPO>/firmware_v2/:/home/dockeruser/defcon_workshop firmware
cd ~/defcon_workshop
```

## Step 4: Build the firmware

```
mkdir build
cd build
cmake .. -DBUILD_TYPE=firmware -DHARDWARE=v2
```

## Step 5: Flash the device

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

