#include "Int_FPM383F.h"

// UART 配置
#define FINGER_UART_NUM UART_NUM_1
#define FINGER_UART_TX_PIN   (GPIO_NUM_?)   // 你选的 TX 引脚
#define FINGER_UART_RX_PIN   (GPIO_NUM_?)   // 你选的 RX 引脚
#define FINGER_UART_BAUDRATE 57600

// 模块控制 / 中断引脚（如果有的话）
#define FINGERER_INT_PIN       (GPIO_NUM_?)   // 中断 / 检测引脚（如果模组有的话）
#define FINGERER_POWER_EN_PIN  (GPIO_NUM_?)   // 如果你加了电源开关控制的话

// 接收缓冲区大小
#define FINGER_RX_BUF_SIZE 256

void Int_FPM383F_Init(void)
{
    MY_LOGI("Int_FPM383F_Init");
    // 初始化 UART
    uart_config_t uart_conf = {
        .baud_rate = FINGER_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_driver_install(FINGER_UART_NUM, FINGER_RX_BUF_SIZE, 0, 0, NULL, 0);
    uart_param_config(FINGER_UART_NUM, &uart_conf);
    uart_set_pin(FINGER_UART_NUM, FINGER_UART_TX_PIN, FINGER_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 延时让模块上电稳定
    vTaskDelay(pdMS_TO_TICKS(200));
}