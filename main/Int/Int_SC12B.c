#include "Int_SC12B.h"

// 硬件配置
#define I2C_MASTER_NUM I2C_NUM_0               // 使用I2C0控制器
#define I2C_MASTER_SDA_IO GPIO_NUM_2           // SDA引脚号 
#define I2C_MASTER_SCL_IO GPIO_NUM_1           // SCL引脚号 
#define I2C_MASTER_FREQ_HZ 400000               // I2C时钟频率（400kHz）

static esp_err_t i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,          // 设置为主机模式
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE, // 启用SDA内部上拉
        .scl_pullup_en = GPIO_PULLUP_ENABLE, // 启用SCL内部上拉
        .master.clk_speed = I2C_MASTER_FREQ_HZ, // 设置主时钟频率
    };

    // 配置I2C控制器参数
    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        MY_LOGE("I2C of parameter config failed: %s", esp_err_to_name(err));
        return err;
    }

    // 安装I2C驱动程序
    // 参数含义：I2C端口号，模式，发送缓冲区大小，接收缓冲区大小，中断分配标志
    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        MY_LOGE("I2C of driver install failed: %s", esp_err_to_name(err));
        return err;
    }

    MY_LOGI("I2C of I2C initialized successfully");
    return ESP_OK;
}
void Int_SC12B_Init(void)
{
    // 初始化按键
    i2c_master_init();
    //初始化中断的引脚
     
}


