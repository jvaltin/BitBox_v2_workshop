# Defcon 2018 - Hack on Bitbox Workshop

This is a simple tutorial on how to develop the BitBox v2 device. You will learn how to compile the firmware, flash the device and modify the code to do your own experiments. Enjoy :-)

## Step 1: Setup

### Option 1 - Fedora Live USB (recommended)

To get started, get the Fedora live USB containing the build environment. We recommend using the live USB, because there is no need to download anything. 

### Option 2 - Download the docker image

If you do not want to use the Live USB, e.g. because you do not want to reboot and leave your system, you can download our docker image.

> Careful, it's about 1.5GB big and, for OSX users, flashing the BitBox doesn't work from inside docker!

### Install and run the docker daemon

Checkout the (docker installation guide)[https://docs.docker.com/install/#supported-platforms] for guidelines on how to install the docker service on your platform.

### Get the defcon_workshop code

```
git clone https://github.com/digitalbitbox/defcon_workshop.git
cd defcon_workshop
```

### Run the docker image

```
docker run --privileged -v /dev/bus/usb:/dev/bus/usb -it 
  -e HOST_USER_ID=$(id -u) 
  -e HOST_USER_GID=$(id -g) 
  -v <PATH_TO_DEFCON_WORKSHOP_REPO>:/home/dockeruser/defcon_workshop 
  shiftcrypto/defcon_workshop
```

## Step 2: Build the firmware

We're using cmake as a build tool. Follow these instructions to get a working binary:

```
cd ~/defcon_workshop/firmware
mkdir build
cd build
cmake ..
make
```

Afterwards, you will find the binary at `bin/firmware.bin`, a sub directory of `build/`.

## Step 3: Flash the device

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

Notice that the `loadfile` command passes the relative path to the firmware binary.

**Congratulations**, you compiled and deployed your first BitBox firmware code!

> **Attention docker users on OSX:**
>
> Since docker cannot run with privileged mode under OSX, you need to switch to your host system and flash the firmware from there.
> To do so, download and install the JLink software from [here](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack).

## Step 4: Try the demos

After you have built and flashed the device, you can start understanding and modifying the code.
Change to the directory ~/defcon/firmware and open the files under the `src` directory with your preferred editor.

You should find the `main.c` inside of `src`. Its `main()` function is called upon start-up. Inside, you see initialization
calls for the system, screen, touch sensor and USB communication.

The `demos/demo_list.h` header file lists the available demo functions. You can copy them below the initialization code
in the `main()` function, recompile like before and flash the new binary with JLink. 

### Hello World

Let's put a classic `Hello World` output on the screen. 
Here is how the `main()` function
should look like after you added the `demo_hello_world` code:

```
int main(void)
{
    system_init();
    screen_init();
    // displays BitBox logo with version 'defcon'
    screen_splash("defcon", 300);
    qtouch_init();
    usb_init();

    demo_hello_world();

    while (1) {}
}
```

Compile and run the code. You should see a `Hello World!` on the screen, which inverts it's colours every 5 seconds.

Let's have a look at what's going on in `demo_hello_world()`:

```
void demo_hello_world(void)
{
    int flip = 0;

    UG_FontSelect(&FONT_6X10);
    
    while (1) {
        UG_ClearBuffer();
        if (flip) {
            UG_FillFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, C_WHITE); 
            UG_SetForecolor(C_BLACK);
            UG_SetBackcolor(C_WHITE);
        } else {
            UG_FillFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, C_BLACK); 
            UG_SetForecolor(C_WHITE);
            UG_SetBackcolor(C_BLACK);
        }
        UG_PutString(26, 30, "Hello world!");
        UG_SendBuffer();
        delay_ms(5000);
        flip = !flip;
    }
}
```

That's all! Pretty straight forward. The `UG_*****` calls interact with the screen. For more functions, have a look at `ugui/ugui.h`.

### Detect Touch

We can use the touch buttons and sliders located on the top and bottom of the BitBox to provide input by the user.

Go back to your main.c and modify the code:

```
int main(void)
{
    system_init();
    screen_init();
    // displays BitBox logo with version 'defcon'
    screen_splash("defcon", 300);
    qtouch_init();
    usb_init();

    demo_detect_touch();

    while (1) {}
}
```

If you compile and run it, you should see the position of the touch sliders on the screen. Scroll the slider to modify the value. 

Try pressing the touch button for longer and see what happens.

You can find the code in `demos/demo_detect_touch.c`

Here's some explanation:

There are 2 sliders located at the top and bottom of the BitBox. They are referenced by 0 (top) and 1 (bottom). The sliders consist of
4 touch buttons each (8 in total). The following code helps to determine the position of the slider and the force applied:

 - `void touch_slider_update(uint16_t nr)` - updates the position of the top and bottom slider. nr is either 0 (top) or 1 (bottom).

 - `uint16_t qtouch_get_sensor_node_signal(uint16_t i)` and `uint16_t qtouch_get_sensor_node_reference(uint16_t i)` - allow us to determine the capacitive value with which a button is pressed. i >= 0 && i < 8


### Read/Write SD card

The BitBox hardware has the ability to read from and write to an SD card. We use that primarily to make backups of the users wallet seed.

You can try out our prepared SD card demo by calling `demo_sd()` from `main()`.

```
int main(void)
{
    system_init();
    screen_init();
    // displays BitBox logo with version 'defcon'
    screen_splash("defcon", 300);
    qtouch_init();
    usb_init();

    demo_sd();

    while (1) {}
}
```

To understand what's going on, have a look at `demos/demo_sd.c`. This file makes use of the functions of `sd.c`.

Here are the important commands to modify and write your own code:

 - `uint8_t sd_card_inserted()` - checks if the SD card was inserted. Returns 1 if it was, 0 if it was not.
 - `uint8_t sd_list()` - outputs the contents of the SD card on the screen. Returns 0 upon success, 1 upon failure.
 - `uint8_t sd_write(const char* name, const char* data, uint8_t replace)` - writes the given data into a file with the given name on the SD card. Replaces it if `replace` is 1. Returns 0 upon success, 1 upon failure.
 - `char* sd_load(const char* name)` - reads the content from the file with the given name on the SD card and returns the content.
 - `uint8_t sd_erase(const char* name)` - deletes the file with the given name.

### Cryptographic functions

The BitBox is a portable hardware security module that comes with a random number generator and hardware support for AES encryption and SHA-256 hashing.

You can find example code in `demos/demo_crypto.c`.

AES encryption:
```
aes_sync_enable(&CRYPTOGRAPHY_0);
aes_sync_set_encrypt_key(&CRYPTOGRAPHY_0, aes_key, AES_KEY_256);
aes_sync_cbc_crypt(&CRYPTOGRAPHY_0, AES_ENCRYPT, aes_plain_text, aes_output, sizeof(aes_plain_text), iv);
```

SHA-256 hashing:
```
sha_sync_enable(&HASH_ALGORITHM_0);
sha_sync_sha256_compute(&HASH_ALGORITHM_0, &context, false, "abc", 3, sha_output);
```

(Note: the third parameter is set to false if you want to use SHA-256 instead of SHA-224).

### Animations and Games

More advanced display hacks can be found in `demos/demo_animation.c` and `demos/demo_mini_pong.c`. 

 - `demos/demo_animation.c` - You can do simple animations by specifying pixel map frames (in our case, 25), 
and iterating through them.
 - `demos/demo_mini_pong.c` - Combine the slider and display capability to make a simple Pong game.

### Sign mock

The `demos/demo_sign.c` file contains mock transaction signing code which you can use as a starting point to build your own Monero wallet. 
Do you have ideas how to improve the usability? Hack it and let us know! :-)

## Contact

Thanks for participating in the workshop. We hope you learned something. :-)

Interested in joining our team and helping us build crypto hardware and software? Then check out [https://www.shiftcrypto.ch](https://www.shiftcrypto.ch) and apply at <jobs@shiftcrypto.ch>.
