

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "driver_init.h"
#include "ssd1306.h"
#include "ugui.h"


uint8_t spioutput[32];

bool mirror;

uint8_t frameBuffer[128*8],outputbuffer[128*8];
static uint8_t frameBufferUpdated;

#define POWERON_PIN                  IOPORT_CREATE_PIN(PIOC, 20)


//! \name OLED controller write and read functions
//@{
/**
 * \brief Writes a command to the display controller
 *
 * This functions pull pin D/C# low before writing to the controller. Different
 * data write function is called based on the selected interface.
 *
 * \param command the command to write
 */
static void ssd1306_write_command(uint8_t command)
{
	spioutput[0]=command;
	gpio_set_pin_level(PIN_OLED_CMD,0);
	gpio_set_pin_level(PIN_OLED_CS,0);
	SPI_0_write_block((void *)spioutput, 1);
	gpio_set_pin_level(PIN_OLED_CS,1);
}


/**
 * \brief Write data to the display controller
 *
 * This functions sets the pin D/C# before writing to the controller. Different
 * data write function is called based on the selected interface.
 *
 * \param data the data to write
 */
static inline void ssd1306_write_data(uint8_t data)
{
	spioutput[0]=data;
	gpio_set_pin_level(PIN_OLED_CMD,1);
	gpio_set_pin_level(PIN_OLED_CS,0);
	SPI_0_write_block((void *)spioutput, 1);
	gpio_set_pin_level(PIN_OLED_CS,1);
}


//! \name Address setup for the OLED
//@{
/**
 * \brief Set current page in display RAM
 *
 * This command is usually followed by the configuration of the column address
 * because this scheme will provide access to all locations in the display
 * RAM.
 *
 * \param address the page address
 */
static inline void ssd1306_set_page_address(uint8_t address)
{
	// Make sure that the address is 4 bits (only 8 pages SH1107 16pages)
	address &= 0x0F;
	ssd1306_write_command(SSD1306_CMD_SET_PAGE_START_ADDRESS(address));
}

/**
 * \brief Set current column in display RAM
 *
 * \param address the column address
 */
static inline void ssd1306_set_column_address(uint8_t address)
{
	// Make sure the address is 7 bits
	address &= 0x7F;
	ssd1306_write_command(SSD1306_CMD_SET_HIGH_COL(address >> 4));
	ssd1306_write_command(SSD1306_CMD_SET_LOW_COL(address & 0x0F));
}

/**
 * \brief Set the display start draw line address
 *
 * This function will set which line should be the start draw line for the OLED.
 */
static inline void ssd1306_set_display_start_line_address(uint8_t address)
{
	// Make sure address is 6 bits
	address &= 0x3F;
	ssd1306_write_command(SSD1306_CMD_SET_START_LINE(address));
}
//@}

//! \name Display hardware control
//@{
/**
 * \brief Turn the OLED display on
 *
 * This function will turn on the OLED.
 */
static inline void ssd1306_display_on(void)
{
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_ON);
}


/**
 * \brief Initialize the OLED controller
 *
 * Call this function to initialize the hardware interface and the OLED
 * controller. When initialization is done the display is turned on and ready
 * to receive data.
 */
void ssd1306_init(void)
{
    // DC-DC OFF
    gpio_set_pin_level(PIN_OLED_ON,0);
    delay_us(5);
    
    // Hard reset OLED display controller
    gpio_set_pin_level(PIN_OLED_RES,0);
    delay_us(5);
    gpio_set_pin_level(PIN_OLED_RES,1);
    delay_us(5);

    // Initialize
    ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFF);
    ssd1306_write_command(SSD1306_CMD_SET_LOW_COL(0));
    ssd1306_write_command(SSD1306_CMD_SET_HIGH_COL(0));
    ssd1306_write_command(SSD1306_CMD_SET_PAGE_START_ADDRESS(0));
    ssd1306_write_command(0xDC); /*set display start line*/
    ssd1306_write_command(0x00); // +0x7f
    ssd1306_write_command(SSD1306_CMD_SET_CONTRAST_CONTROL_FOR_BANK0);
    ssd1306_write_command(0x80); /* 0x00..0xFF */
    ssd1306_write_command(SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE);
    ssd1306_write_command(SSD1306_CMD_SET_SEGMENT_RE_MAP_COL0_SEG0);
    ssd1306_write_command(SSD1306_CMD_SET_COM_OUTPUT_SCAN_UP);
    ssd1306_write_command(SSD1306_CMD_SET_NORMAL_DISPLAY);
    ssd1306_write_command(SSD1306_CMD_SET_MULTIPLEX_RATIO);
    ssd1306_write_command(0x3F); /* duty = 1/64; 0x00..0x7F */
    ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFFSET);
    ssd1306_write_command(0x60); // /* 0x00..0x7F */ <- todo set value ?? 96  Col32..C95 are used
    ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO);
    ssd1306_write_command(0x51);//41);//0x51);
    ssd1306_write_command(SSD1306_CMD_SET_PRE_CHARGE_PERIOD);
    ssd1306_write_command(0x22); /* 0x00..0xFF */
    ssd1306_write_command(SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL);
    ssd1306_write_command(0x35); /* 0x00..0xFF */ // 53 (?)
    ssd1306_write_command(0xAD); /* DC-DC control mode set*/
    ssd1306_write_command(0x8A); /* Built-in DC-DC enable (8A:disable; 8B:enable) */
    ssd1306_write_command(SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON);
    OLED_ClearBuffer();
    OLED_SendBuffer();
    ssd1306_display_on();
    delay_ms(100);
    // DC-DC ON
    gpio_set_pin_level(PIN_OLED_ON,1);
}


//*****************************************************************************
//
void OLED_SendBuffer(void)
{
  uint32_t i,n,x;
  
  if(frameBufferUpdated) 
  { // 1.4msec
    frameBufferUpdated=0;
    memset(outputbuffer,0,sizeof(outputbuffer));
    for(n=0;n<8;n++)
    {
      for(i=0;i<128;i++) // Framebuffer Columns
      {
        uint32_t target;
		
		if(!mirror)
		{
			// 0..127 x 8x [Bit0..7] -> translate 16 x [0..7] 0..63
			target=64*(15-i/8)+n*8;
			x=frameBuffer[i+n*128];
        
			if(x & 0x01) outputbuffer[target+0] |= 0x01<<(7-(i%8));
			if(x & 0x02) outputbuffer[target+1] |= 0x01<<(7-(i%8));
			if(x & 0x04) outputbuffer[target+2] |= 0x01<<(7-(i%8));
			if(x & 0x08) outputbuffer[target+3] |= 0x01<<(7-(i%8));
			if(x & 0x10) outputbuffer[target+4] |= 0x01<<(7-(i%8));
			if(x & 0x20) outputbuffer[target+5] |= 0x01<<(7-(i%8));
			if(x & 0x40) outputbuffer[target+6] |= 0x01<<(7-(i%8));
			if(x & 0x80) outputbuffer[target+7] |= 0x01<<(7-(i%8));
		}
		else
		{
			// 0..127 x 8x [0..7] -> translate 16 x [0..7] 0..63
			target=64*(i/8)+(7-n)*8;
			x=frameBuffer[i+n*128];
			
			if(x & 0x01) outputbuffer[target+7] |= 0x01<<(0+(i%8));
			if(x & 0x02) outputbuffer[target+6] |= 0x01<<(0+(i%8));
			if(x & 0x04) outputbuffer[target+5] |= 0x01<<(0+(i%8));
			if(x & 0x08) outputbuffer[target+4] |= 0x01<<(0+(i%8));
			if(x & 0x10) outputbuffer[target+3] |= 0x01<<(0+(i%8));
			if(x & 0x20) outputbuffer[target+2] |= 0x01<<(0+(i%8));
			if(x & 0x40) outputbuffer[target+1] |= 0x01<<(0+(i%8));
			if(x & 0x80) outputbuffer[target+0] |= 0x01<<(0+(i%8));
			
		}
      }
    }
  }
  
	// 3.5msec 
    for(i=0;i<128/8;i++) {
      ssd1306_write_command(0x10); /*set higher column address*/
      ssd1306_write_command(0x00); /*set lower column address*/
      ssd1306_set_page_address(i);
      gpio_set_pin_level(PIN_OLED_CMD,1);
      gpio_set_pin_level(PIN_OLED_CS,0);
      SPI_0_write_block((unsigned char *)&outputbuffer[i*64], 64);
      gpio_set_pin_level(PIN_OLED_CS,1);
    }
}

//*****************************************************************************
//
void OLED_ClearBuffer(void)
{
  memset(outputbuffer,0,sizeof(outputbuffer));
  memset(frameBuffer,0,sizeof(frameBuffer));
}

//*****************************************************************************
//
void OLED_Brightness(uint8_t brightness)
{
  ssd1306_write_command(0x81);    /* contrast control */
  ssd1306_write_command(brightness);  /* 0..255 */
}

void OLED_Mirror(bool mirr)
{
	mirror=mirr;
}


//*****************************************************************************
//
void OLED_PSET(uint16_t x, uint16_t y, uint8_t c) // y x instead x y
{
  uint32_t p;

  if ( x > 127 ) return;
  p = y>>3; // :8
  p = p<<7; // *128
  p +=x;

  if( c )
  {
    frameBuffer[p] |= 1<<(y%8);
  }
  else
  {
    frameBuffer[p] &= ~(1<<(y%8));
  }
  frameBufferUpdated=1;
}
