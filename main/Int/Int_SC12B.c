#include "Int_SC12B.h"

/**
 * @brief  SC12B中断处理函数
 *
 * @param  arg    中断处理函数的参数
 *
 */
static void IRAM_ATTR sc12b_int_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    if (gpio_num == SC_INT_PIN)
    {

        // 清除中断标志
        gpio_intr_disable(gpio_num);
        // 读取中断标志
        MY_LOGI("按键中断产生，按键被按下");
        // 通知APP任务处理按键事件
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(App_IO_read_Handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/**
 * @brief   I2C0主机初始化
 * @note    该函数配置I2C0控制器的参数并安装I2C驱动程序
 * @return esp_err_t
 */

static esp_err_t i2c_master_init(void)
{
    // SC12B的i2C接口初始化
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER, // 设置为主机模式
        .sda_io_num = SC_I2C_MASTER_SDA_IO,
        .scl_io_num = SC_I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,       // 启用SDA内部上拉
        .scl_pullup_en = GPIO_PULLUP_ENABLE,       // 启用SCL内部上拉
        .master.clk_speed = SC_I2C_MASTER_FREQ_HZ, // 设置主时钟频率
    };

    // 配置I2C控制器参数
    esp_err_t err = i2c_param_config(SC_I2C_MASTER_NUM, &conf);
    if (err != ESP_OK)
    {
        MY_LOGE("I2C of parameter config failed: %s", esp_err_to_name(err));
        return err;
    }

    // 安装I2C驱动程序
    // 参数含义：I2C端口号，模式，发送缓冲区大小，接收缓冲区大小，中断分配标志
    err = i2c_driver_install(SC_I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (err != ESP_OK)
    {
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
static void gpio_interrupt_init(void)
{
    // INT：触摸信号输出指示端口。 有按键时输出高电平， 无按键时输出低电平。
    // 初始化引脚局部变量
    gpio_config_t io_conf = {0};
    // 开启中断,高电平触发
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    // 设置引脚输入模式 输入
    io_conf.mode = GPIO_MODE_INPUT;
    // 设置中断引脚号
    io_conf.pin_bit_mask = 1 << SC_INT_PIN;
    // 启用下拉模式，确保引脚输入稳定性
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    // 禁用上拉模式
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    // 配置GPIO
    gpio_config(&io_conf);

    // 安装中断服务
    gpio_install_isr_service(0);
    // 注册中断处理函数
    gpio_isr_handler_add(SC_INT_PIN, sc12b_int_handler, NULL);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // hook isr handler for specific gpio pin    UNTODO: check return value
    gpio_isr_handler_add(SC_INT_PIN, sc12b_int_handler, (void *)SC_INT_PIN);
    // 中断初始化成功
    MY_LOGI("GPIO interrupt initialized successfully");
}

/*************************************业务逻辑***************************************** */

#define SC_I2C_SLAVE_ADDR 0x42
/**
 * @brief  根据设备地址和寄存器地址读取SC12B寄存器
 * @param  reg    寄存器地址
 * @return 读取到的数据
 */
static uint8_t Int_SC12B_ReadRegsiter(uint8_t reg)
{
    uint8_t data = 0;
    i2c_master_read_from_device(SC_I2C_MASTER_NUM, SC_I2C_SLAVE_ADDR, &data, 1, portMAX_DELAY);
    return data;
}

/**
 * @brief  根据设备地址和寄存器地址写入SC12B寄存器
 * @param  reg    寄存器地址
 * @param  data   写入的数据
 */
static void Int_SC12B_WriteRegsiter(uint8_t reg, uint8_t data)
{
    i2c_master_write_to_device(SC_I2C_MASTER_NUM, SC_I2C_SLAVE_ADDR, &data, 1, portMAX_DELAY);
}

/*********************************实际暴漏的接口业务代码************************************************** */

/**
 * @brief  SC12B初始化
 * @note   该函数初始化I2C总线，等待传感器上电完成，然后配置中断引脚和寄存器
 * @return void
 */
void Int_SC12B_Init(void)
{
    i2c_master_init();                    // 1.总线初始化
    vTaskDelay(300 / portTICK_PERIOD_MS); // 2.等待300ms传感器上电完成
    gpio_interrupt_init();                // 3.最后启用中断
    // 根据手册设置灵敏度寄存器，值越大灵敏度越高，分别设置两个寄存器对应12个按键
    Int_SC12B_WriteRegsiter(SC12B_SenSet0, SC12B_SET_SENSITIVITY);
    Int_SC12B_WriteRegsiter(SC12B_SenSet1, SC12B_SET_SENSITIVITY);
    MY_LOGI("SC12B initialized successfully");
}

/**
 * @brief 读取触摸Key值
 * @return Touch_Key 枚举类型的按键值
 *
 */

Touch_Key Int_SC12B_Read_TouchKey(void)
{
    Touch_Key key = KEY_NO;
    // 分别读取SC12B两个输出寄存器的值
    uint8_t reg_value = Int_SC12B_ReadRegsiter(SC_REG_Output1);  // 读取寄存器值
    uint8_t reg_value2 = Int_SC12B_ReadRegsiter(SC_REG_Output2); // 读取寄存器值
    // 拼接两个寄存器的值,前一个寄存器高位到低位分别对应KEY0~KEY7,后一个寄存器高4位分别对应KEY8~KEY#,合成一个16位值，最低位对应Key0,最高位对应Key#
    uint16_t value = (reg_value2 >> 4) | (reg_value << 4);
    switch (value)
    {
    case 1 << 11:
        key = KEY_0;
        break;
    case 1 << 10:
        key = KEY_1;
        break;
    case 1 << 9:
        key = KEY_2;
        break;
    case 1 << 8:
        key = KEY_3;
        break;
    case 1 << 7:
        key = KEY_4;
        break;
    case 1 << 6:
        key = KEY_5;
        break;
    case 1 << 5:
        key = KEY_6;
        break;
    case 1 << 4:
        key = KEY_7;
        break;
    case 1 << 3:
        key = KEY_8;
        break;
    case 1 << 2:
        key = KEY_9;
        break;
    case 1 << 1:
        key = KEY_SHARP;
        break;
    case 1 << 0:
        key = KEY_M;
        break;
    default:
        key = KEY_NO;
    }

    return key;
}