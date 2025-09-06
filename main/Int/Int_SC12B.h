#ifndef _INT_SC12B_H_
#define _INT_SC12B_H_

#include "debug/com_debug.h"
#include "driver/i2c.h" // 老版本I2C驱动主要头文件
#include "esp_err.h"    // 提供了esp_err_t类型和ESP_OK等错误码的定义，用于错误处理
#include "driver/gpio.h"
#include "esp_task.h"

extern TaskHandle_t App_IO_read_Handle; // 声明外部任务句柄

// 硬件配置
#define SC_INT_PIN GPIO_NUM_0           // 中断引脚号
#define SC_I2C_MASTER_NUM I2C_NUM_0     // 使用I2C0控制器
#define SC_I2C_MASTER_SDA_IO GPIO_NUM_2 // SDA引脚号
#define SC_I2C_MASTER_SCL_IO GPIO_NUM_1 // SCL引脚号
#define SC_I2C_MASTER_FREQ_HZ 400000    // I2C时钟频率（400kHz）
#define ESP_INTR_FLAG_DEFAULT 0
//SC12B设备地址
#define SC_I2C_SLAVE_ADDR 0x42
//SC12B寄存器地址
#define SC12B_SENSITIVITY 0x25
#define SC12B_SenSet0 0x00
#define SC12B_SenSet1 0x01
#define SC_REG_Output1 0x08
#define SC_REG_Output2 0x09





#ifdef __cplusplus
extern "C"
{
#endif
    // 按键枚举
    typedef enum
    {
        KEY_0 = 0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_SHARP,
        KEY_M,
        KEY_NO,

    } Touch_Key;

    void Int_SC12B_Init(void);
    Touch_Key Int_SC12B_Read_TouchKey(void);

#ifdef __cplusplus
}
#endif

#endif // _INT_SC12B_H_
