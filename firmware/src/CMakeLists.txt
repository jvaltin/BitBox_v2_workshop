#-----------------------------------------------------------------------------
# CmakeLists.txt for the Digital Bitbox.
# Copyright (c) 2015-2016 Lucas Betschart, Douglas J. Bakkum
# MIT License


#-----------------------------------------------------------------------------
# Source files

set(DBB-FIRMWARE-SOURCES
    commander.c
    random.c
    util.c
    sd.c
    usb/usb.c
    usb/usb_apdu.c
    demos/demo_sign.c
    demos/demo_hello_world.c
    demos/demo_detect_touch.c
    demos/demo_animation.c
    demos/demo_mini_pong.c
    demos/demo_sd.c
    main.c
)

set(DBB-FIRMWARE-UI-SOURCES
    screen.c
    ugui/ugui.c
    ugui/fonts/font_ab_10X10.c
    ugui/fonts/font_ab_13X13.c
    ugui/fonts/font_ab_15X17.c
    ugui/fonts/font_a_11X12.c
    ugui/fonts/font_a_15X16.c
    ugui/fonts/font_a_17X18.c
)

set(DRIVER-SOURCES
    drivers/driver_init.c
    drivers/oled/ssd1306.c
    drivers/hal/utils/src/utils_syscalls.c
    drivers/hal/utils/src/utils_list.c
    drivers/hal/src/hal_atomic.c
    drivers/hal/src/hal_gpio.c
    drivers/hal/src/hal_init.c
    drivers/hal/src/hal_delay.c
    drivers/hal/src/hal_timer.c
    drivers/hal/src/hal_mci_sync.c
    drivers/hal/src/hal_usart_async.c
    drivers/hal/src/hal_usb_device.c
    drivers/hal/src/hal_io.c
    drivers/hal/src/hal_rand_sync.c 
   	drivers/hal/src/hal_aes_sync.c
	drivers/hal/src/hal_flash.c
	drivers/hal/src/hal_rand_sync.c
	drivers/hal/src/hal_sha_sync.c
    drivers/hal/utils/src/utils_assert.c
    drivers/hal/utils/src/utils_ringbuffer.c
    drivers/hal/src/hal_sleep.c
    drivers/hal/utils/src/utils_event.c
    drivers/hal/src/hal_i2c_m_sync.c 
    drivers/hpl/systick/hpl_systick.c 
    drivers/hpl/dmac/hpl_dmac.c
    drivers/hpl/gclk/hpl_gclk.c
    drivers/hpl/oscctrl/hpl_oscctrl.c
    drivers/hpl/mclk/hpl_mclk.c 
    drivers/hpl/osc32kctrl/hpl_osc32kctrl.c
    drivers/hpl/core/hpl_init.c 
    drivers/hpl/core/hpl_core_m4.c
    drivers/hpl/ramecc/hpl_ramecc.c
    drivers/hpl/sdhc/hpl_sdhc.c
    drivers/hpl/sercom/hpl_sercom.c
    drivers/hpl/spi/spi_lite.c
    drivers/hpl/usb/hpl_usb.c
    drivers/hpl/pm/hpl_pm.c
    drivers/hpl/rtc/hpl_rtc.c
    drivers/hpl/trng/hpl_trng.c
    drivers/hpl/aes/hpl_aes.c 
    drivers/hpl/nvmctrl/hpl_nvmctrl.c 
    drivers/hpl/icm/hpl_icm.c 
    drivers/qtouch/qtouch.c
    drivers/usb/usb_protocol.c
    drivers/usb/device/usbdc.c
    drivers/usb/class/hid/device/hiddf_generic.c
    drivers/sd_mmc/sd_mmc.c
    drivers/diskio/sdmmc_diskio.c
    drivers/ff13a/src/ff.c
    drivers/ff13a/src/ffunicode.c
    drivers/SAMD51_DFP/1.0.70/gcc/system_samd51.c
    drivers/SAMD51_DFP/1.0.70/gcc/gcc/startup_samd51.c
)


#-----------------------------------------------------------------------------
# Include directories

set(INCLUDES
    .
    ugui
    usb
    demos
    drivers
    drivers/oled
    drivers/CMSIS/5.0.1/Include
    drivers/SAMD51_DFP/1.0.70/include
    drivers/Config
    drivers/hal/include
    drivers/hal/utils/include
    drivers/hpl/core
    drivers/hpl/dmac
    drivers/hpl/gclk
    drivers/hpl/mclk
    drivers/hpl/osc32kctrl
    drivers/hpl/oscctrl
    drivers/hpl/pm
    drivers/hpl/port
    drivers/hpl/ramecc
    drivers/hpl/rtc
    drivers/hpl/sdhc
    drivers/hpl/sercom
    drivers/hpl/spi
    drivers/hpl/usb
    drivers/hpl/trng
    drivers/hri
    drivers/qtouch
    drivers/qtouch/datastreamer
    drivers/qtouch/include
    drivers/usb
    drivers/usb/device
    drivers/usb/class/hid
    drivers/usb/class/hid/device
    drivers/sd_mmc
    drivers/diskio
    drivers/ff13a/src
)


#-----------------------------------------------------------------------------
# Build embedded firmware

set(SOURCES
    ${DBB-FIRMWARE-SOURCES} 
    ${DBB-FIRMWARE-UI-SOURCES}
    ${DRIVER-SOURCES}
)
set(CMAKE_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/samd51j20a_flash.ld")
    
set(CMAKE_C_LINK_FLAGS "-mthumb -Wl,-Map=\"../bin/${MYPROJECT}.map\" --specs=nosys.specs -Wl,--start-group  -lm  -Wl,--end-group -Wl,--gc-sections -mcpu=cortex-m4 -T\"${CMAKE_LINKER_SCRIPT}\"")

message(STATUS "C link flags:     ${CMAKE_C_LINK_FLAGS}\n")
include_directories(${INCLUDES})

add_library(qtouchlib_a STATIC IMPORTED)
set_property(TARGET qtouchlib_a PROPERTY
            IMPORTED_LOCATION 
            ${CMAKE_SOURCE_DIR}/src/drivers/qtouch/lib/gcc/libqtm_acq_samd51_0x000f.a    
            )
add_library(qtouchlib_s STATIC IMPORTED)
set_property(TARGET qtouchlib_s PROPERTY
            IMPORTED_LOCATION 
            ${CMAKE_SOURCE_DIR}/src/drivers/qtouch/lib/gcc/libqtm_scroller_cm4_0x000b.a
            )
add_library(qtouchlib_b STATIC IMPORTED)
set_property(TARGET qtouchlib_b PROPERTY
            IMPORTED_LOCATION 
            ${CMAKE_SOURCE_DIR}/src/drivers/qtouch/lib/gcc/libqtm_binding_layer_cm4_0x0005.a
            )
add_library(qtouchlib_t STATIC IMPORTED)
set_property(TARGET qtouchlib_t PROPERTY
            IMPORTED_LOCATION 
            ${CMAKE_SOURCE_DIR}/src/drivers/qtouch/lib/gcc/libqtm_touch_key_cm4_0x0002.a 
            )

link_directories(${CMAKE_SOURCE_DIR}/src/drivers/qtouch/lib/gcc)

add_executable(${ELF} ${SOURCES})

target_link_libraries(${ELF} qtouchlib_a qtouchlib_s qtouchlib_b qtouchlib_t)
