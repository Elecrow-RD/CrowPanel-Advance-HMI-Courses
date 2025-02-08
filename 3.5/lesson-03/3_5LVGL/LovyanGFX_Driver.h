#define LGFX_USE_V1
#include <LovyanGFX.hpp>
//#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
//#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

class LGFX : public lgfx::LGFX_Device
{
    // lgfx::Panel_ST7789     _panel_instance;
    // lgfx::Panel_ST7796     _panel_instance;
    // lgfx::Panel_ST7735     _panel_instance;
    lgfx::Panel_ILI9488     _panel_instance;
    // lgfx::Panel_ILI9341     _panel_instance;
    lgfx::Bus_SPI       _bus_instance;   // SPIバスのインスタンス
    // lgfx::Touch_FT5x06  _touch_instance;
    lgfx::Touch_GT911  _touch_instance;

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

      // { // 配置触摸屏控制设置。 (不需要时删除）
      //   auto cfg = _touch_instance.config();

      //   cfg.x_min = 0;            // 从触摸屏获得的最小 X 值(原始值)
      //   cfg.x_max = 319;          // 触摸屏可提供的最大 X 值(原始值)
      //   cfg.y_min = 0;            // 从触摸屏获得的最小 Y 值(原始值)
      //   cfg.y_max = 479;          // 触摸屏可提供的最大 Y 值(原始值)
      //   cfg.pin_int = 47;         // INT 所连接的针脚编号。
      //   cfg.bus_shared = false;   // 如果您使用与屏幕相同的总线，则设置为 true
      //   cfg.offset_rotation = 0;  // 显示和触摸方向不匹配时的调整 设置为 0 到 7 的值

      //   // 用于 I2C 连接
      //   cfg.i2c_port = 0;     // 选择要使用的 I2C（0 或 1）
      //   // cfg.i2c_addr = 0x38;  // I2C 设备地址编号
      //   cfg.i2c_addr = 0x14;  // I2C 设备地址编号
      //   cfg.pin_sda = 15;     // 连接 SDA 的引脚编号。
      //   cfg.pin_scl = 16;     // 连接 SCL 的引脚编号。
      //   cfg.freq = 400000;    // 设置 I2C 时钟。

      //   _touch_instance.config(cfg);
      //   _panel_instance.setTouch(&_touch_instance);  // 在面板上设置触摸屏。
      // }

      {
        auto cfg = _touch_instance.config();
        cfg.x_min = 0;
        cfg.x_max = 319;
        cfg.y_min = 0;
        cfg.y_max = 479;
        cfg.pin_int = 47;
        cfg.bus_shared = false;
        cfg.offset_rotation = 0;
        // I2C接続
        cfg.i2c_port = I2C_NUM_0;
        cfg.pin_sda = GPIO_NUM_15;
        cfg.pin_scl = GPIO_NUM_16;
        cfg.pin_rst = 48;
        cfg.freq = 400000;
        cfg.i2c_addr = 0x14;  // 0x5D , 0x14
        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
      }

      setPanel(&_panel_instance);
    }
};
