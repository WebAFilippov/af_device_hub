// User_Setup.h для ESP32-S3 с дисплеем ILI9341 240x320
// ИСПОЛЬЗУЕМ БЕЗОПАСНЫЕ ПИНЫ (не strapping pins!)

#define USER_SETUP_LOADED

// Драйвер дисплея
#define ILI9341_DRIVER

// Размеры дисплея
#define TFT_WIDTH 240
#define TFT_HEIGHT 320

// Пины дисплея - БЕЗОПАСНЫЕ GPIO (не strapping pins 8-12!)
// Используем FSPI пины + свободные GPIO
#define TFT_MISO -1 // Не используется
#define TFT_MOSI 35 // FSPI MOSI (безопасный)
#define TFT_SCLK 36 // FSPI SCK (безопасный)
#define TFT_CS 13   // Chip Select (безопасный)
#define TFT_DC 14   // Data/Command (безопасный)
#define TFT_RST 21  // Reset (безопасный)

// Используем FSPI порт
#define USE_HSPI_PORT

// Частота SPI
#define SPI_FREQUENCY 20000000      // 20 MHz
#define SPI_READ_FREQUENCY 10000000 // 10 MHz для чтения
#define SPI_TOUCH_FREQUENCY 2500000 // 2.5 MHz для тачскрина

// Поддерживаемые шрифты
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// Пин тачскрина (не используется)
#define TOUCH_CS -1

// Цветовая схема
#define TFT_BLACK 0x0000
#define TFT_NAVY 0x000F
#define TFT_DARKGREEN 0x03E0
#define TFT_DARKCYAN 0x03EF
#define TFT_MAROON 0x7800
#define TFT_PURPLE 0x780F
#define TFT_OLIVE 0x7BE0
#define TFT_LIGHTGREY 0xC618
#define TFT_DARKGREY 0x7BEF
#define TFT_BLUE 0x001F
#define TFT_GREEN 0x07E0
#define TFT_CYAN 0x07FF
#define TFT_RED 0xF800
#define TFT_MAGENTA 0xF81F
#define TFT_YELLOW 0xFFE0
#define TFT_WHITE 0xFFFF
#define TFT_ORANGE 0xFDA0
#define TFT_GREENYELLOW 0xB7E0
#define TFT_PINK 0xFC9F