#include "Int_SC12B.h"


/**
 * @brief   I2C0主机初始化
 * @note    该函数配置I2C0控制器的参数并安装I2C驱动程序
 * @return esp_err_t 
 */
static esp_err_t i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,          // 设置为主机模式
        .sda_io_num = SC_I2C_MASTER_SDA_IO,
        .scl_io_num = SC_I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE, // 启用SDA内部上拉
        .scl_pullup_en = GPIO_PULLUP_ENABLE, // 启用SCL内部上拉
        .master.clk_speed = SC_I2C_MASTER_FREQ_HZ, // 设置主时钟频率
    };

    // 配置I2C控制器参数
    esp_err_t err = i2c_param_config(SC_I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        MY_LOGE("I2C of parameter config failed: %s", esp_err_to_name(err));
        return err;
    }

    // 安装I2C驱动程序
    // 参数含义：I2C端口号，模式，发送缓冲区大小，接收缓冲区大小，中断分配标志
    err = i2c_driver_install(SC_I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        MY_LOGE("I2C of driver install failed: %s", esp_err_to_name(err));
        return err;
    }

    MY_LOGI("I2C of I2C initialized successfully");
    return ESP_OK;
}


/**
 * @brief 中断引脚初始化，引脚号是GPIO0
 * 
 * 
 */
static void  gpio_interrupt_init(void)
{
   //初始化引脚局部变量
    gpio_config_t io_conf = {0};
    // 开启中断
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    // 设置引脚输入模式 输入
    io_conf.mode = GPIO_MODE_INPUT;
    // 设置中断引脚号
    io_conf.pin_bit_mask = 1 << SC_INT_PIN;
    // 禁用下拉模式
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // 禁用上拉模式
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    // 配置GPIO
    gpio_config(&io_conf);
}


void Int_SC12B_Init(void)
{
    // 初始化按键
    i2c_master_init();
    //初始化中断的引脚
    
}


