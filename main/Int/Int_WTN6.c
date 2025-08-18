/**
 * @file Int_WTN6.c
 * @brief 
 * @author Vesper Shaw (xgq@163.com)
 * @version 1.0
 * @date 2025-08-11
 * @copyright Copyright (c) 2025  XXXX有限公司
 * 
 */

#include "Int_WTN6.h"

/**
 * @brief 初始化wtn6的引脚
 */
void Int_WTN6_Init(void)
{

    /*初始化DATA引脚*/
    gpio_config_t io_conf = {};
    // 禁用中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // 设置引脚输出模式为推挽输出
    io_conf.mode = GPIO_MODE_OUTPUT;
    // 设置data引脚号
    io_conf.pin_bit_mask = 1 << DATA_PIN;
    // 禁用下拉模式
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // 禁用上拉模式
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    // 配置GPIO
    gpio_config(&io_conf);

    /*初始化BUSY引脚,读取其状态，高电平空闲状态，低电平语音播放状态*/
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // 设置引脚输入模式 输入
    io_conf.mode = GPIO_MODE_INPUT;
    // 设置busy引脚号
    io_conf.pin_bit_mask = 1 << BUSY_PIN;
    // 禁用下拉模式
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // 禁用上拉模式
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    // 配置GPIO
    gpio_config(&io_conf);
}

void Int_WTN6_SetCmd(uint8_t data)
{
    // 等待BUSY引脚为高电平,高电平空闲状态，低电平语音播放状态
    while (BUSY_READ == 0);
    // 设置DATA引脚为高电平
    DATA_HIGH;
    // 延时5ms
    vTaskDelay(5);
    // 设置DATA引脚为低电平
    DATA_LOW;
    // 延时5ms
    vTaskDelay(5);
    // 发送数据
    for (int i = 0; i < 8; i++)
    {
        if (data & 0x01)
        {
            DATA_HIGH;
            // 延时600us
            usleep(600);
            DATA_LOW;
            // 延时200us
            usleep(200);
        }
        else
        {
            DATA_HIGH;
            usleep(200);
            DATA_LOW;
            usleep(600);
        }
        // 右移一位
        data >>= 1;
    }
    DATA_HIGH;
    vTaskDelay(4);
}
