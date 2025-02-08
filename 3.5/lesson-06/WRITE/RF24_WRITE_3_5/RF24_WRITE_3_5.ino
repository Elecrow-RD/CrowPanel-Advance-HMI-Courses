#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9488     _panel_instance;
    lgfx::Bus_SPI       _bus_instance; 
    // lgfx::Touch_GT911  _touch_instance;

  public:
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();

        // SPI 总线配置
        cfg.spi_host = SPI2_HOST;  // 选择 SPI 以使用 ESP32-S2、C3：SPI2_HOST 或 SPI3_HOST / ESP32：VSPI_HOST 或 HSPI_HOST
        // 随着 ESP-IDF 版本升级，VSPI_HOST , HSPI_HOST 的描述已被弃用，如果出现错误，请使用 SPI2_HOST , SPI3_HOST 代替。
        cfg.spi_mode = 0;                     // SPI通信モードを設定 (0 ~ 3)
        cfg.freq_write = 40000000;            // 传输时的SPI时钟（最高80MHz，四舍五入为80MHz除以整数得到的值）
        cfg.freq_read = 16000000;             // 接收时的SPI时钟
        cfg.spi_3wire = false;                // 如果通过 MOSI 引脚接收，则设为 true
        cfg.use_lock = true;                  // 使用事务锁时设置为 true
        cfg.dma_channel = SPI_DMA_CH_AUTO;    // 设置要使用的 DMA 通道（0=未使用 DMA / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自动设置）
        // 随着 ESP-IDF 版本升级，现在推荐使用 SPI_DMA_CH_AUTO（自动设置）作为 DMA 通道。
        cfg.pin_sclk = 42;                    // 设置 SPI 的 SCLK 引脚编号m
        cfg.pin_mosi = 39;                    // 设置 SPI 的 MOSI 引脚编号
        cfg.pin_miso = -1;                    // 设置 SPI 的 MISO 引脚编号 (-1 = disable)
        cfg.pin_dc = 41;                      // 设置 SPI 的 DC 引脚编号  (-1 = disable)

        _bus_instance.config(cfg);               // 设定值将反映在总线上。
        _panel_instance.setBus(&_bus_instance);  // 在面板上设置总线。
      }

      { // 配置显示面板控制设置。
        auto cfg = _panel_instance.config();  // 获取显示面板配置结构。

        cfg.pin_cs = 40;    // CS 所连接的引脚编号。   (-1 = disable)
        cfg.pin_rst = 2;   // 连接 RST 的引脚编号。  (-1 = disable)
        cfg.pin_busy = -1;  // 连接 BUSY 的引脚编号。 (-1 = disable)

        // 为每个面板设置了以下默认值，以及 BUSY 所连接的针脚编号（-1 = 禁用），因此如果您对某个项目不确定，可将其注释出来并试一试。

        cfg.memory_width = 320;    // 驱动集成电路支持的最大宽度
        cfg.memory_height = 480;   // 驱动集成电路支持的最大高度
        cfg.panel_width = 320;     // 实际可显示宽度
        cfg.panel_height = 480;    // 实际可显示高度
        cfg.offset_x = 0;          // 面板 X 方向的偏移量
        cfg.offset_y = 0;         // 面板 Y 方向的偏移量
        cfg.offset_rotation = 3;   //值在旋转方向的偏移0~7（4~7是倒置的）
        cfg.dummy_read_pixel = 8;  // 在读取像素之前读取的虚拟位数
        cfg.dummy_read_bits = 1;   // 读取像素以外的数据之前的虚拟读取位数
        cfg.readable = false;      // 如果可以读取数据，则设置为 true
        cfg.invert = true;         // 如果面板的明暗反转，则设置为 true
        cfg.rgb_order = false;      // 如果面板的红色和蓝色被交换，则设置为 true
        cfg.dlen_16bit = false;    // 对于以 16 位单位发送数据长度的面板，设置为 true
        cfg.bus_shared = true;    // 如果总线与 SD 卡共享，则设置为 true（使用 drawJpgFile 等执行总线控制）

        _panel_instance.config(cfg);
      }

      // {
      //   auto cfg = _touch_instance.config();
      //   cfg.x_min = 0;
      //   cfg.x_max = 319;
      //   cfg.y_min = 0;
      //   cfg.y_max = 479;
      //   cfg.pin_int = 47;
      //   cfg.bus_shared = false;
      //   cfg.offset_rotation = 0;
      //   // I2C接続
      //   cfg.i2c_port = I2C_NUM_0;
      //   cfg.pin_sda = GPIO_NUM_15;
      //   cfg.pin_scl = GPIO_NUM_16;
      //   cfg.pin_rst = 48;
      //   cfg.freq = 400000;
      //   cfg.i2c_addr = 0x14;  // 0x5D , 0x14
      //   _touch_instance.config(cfg);
      //   _panel_instance.setTouch(&_touch_instance);
      // }

      setPanel(&_panel_instance);
    }
};

LGFX gfx;

#define CE_PIN 1
#define CSN_PIN 2

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

SPIClass* hspi = nullptr;

#define HSPI_MISO  9
#define HSPI_MOSI  3
#define HSPI_SCLK  10
#define HSPI_SS    46

/*
Function function: Display text on the screen
    lcd_w: Product horizontal axis resolution
    lcd_h： Product vertical axis resolution
    x： Screen displays the starting horizontal axis
    y： Screen displays the starting vertical axis
    text： The text content displayed on the screen
*/
void show_test(int lcd_w, int lcd_h, int x, int y, const char * text)
{
  gfx.fillScreen(TFT_BLACK);
  gfx.setTextSize(3);
  gfx.setTextColor(TFT_RED);
  gfx.setCursor(x, y);
  gfx.print(text); // 显示文本
}

const byte address[6] = "00001";
void setup() {
  Serial.begin(115200);

  Wire.begin(15, 16);
  delay(50);

  pinMode(38, OUTPUT);  //  Backlight pin
  digitalWrite(38, HIGH);

  /*Switch GPO45 to low level to enable wireless module*/
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);// Switch between microphone and wireless module

  // Init Display
  gfx.init();
  gfx.initDMA();
  gfx.startWrite();
  gfx.fillScreen(TFT_BLACK);

  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  hspi = new SPIClass(HSPI); // by default VSPI is used
  // to use the custom defined pins, uncomment the following
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  if (!radio.begin(hspi)) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  else
  {
    Serial.println(F("radio hardware is OK!!"));
  }
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);  //RF24_250KBPS  RF24_1MBPS  RF24_2MBPS
  radio.setChannel(50);
  radio.stopListening();
}

int i = 0;
void loop() {
  Serial.println(F("SENDING..."));
  String str = "SENDING...";
  str += String(i);
  show_test(480, 320, 150, 150, str.c_str());
  i++;
  const char text[] = "Hello World I2:";
  radio.write(&text, sizeof(text));// Send the content in the text
  delay(1000);
}
