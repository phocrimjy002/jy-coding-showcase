// ###### SETUP FOR CHEAP YELLOW DISPLAY (CYD) ######

#define ILI9341_2_DRIVER     // Specifically for the newer CYD panels

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// ###### CYD PINS ######
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1  // RST is tied to ESP32 Reset button
#define TFT_BL   21  // Backlight Pin
#define TFT_BACKLIGHT_ON HIGH

// ###### FONTS (Enable all to fix missing text) ######
#define LOAD_GLCD   // Font 1
#define LOAD_FONT2  // Font 2
#define LOAD_FONT4  // Font 4
#define LOAD_FONT6  // Font 6
#define LOAD_FONT7  // Font 7 (Digital)
#define LOAD_FONT8  // Font 8
#define LOAD_GFXFF  // FreeFonts
#define SMOOTH_FONT

// ###### SPI SETTINGS ######
#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000