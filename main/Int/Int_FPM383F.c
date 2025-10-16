#include "Int_FPM383F.h"

// UART 配置
#define FINGER_UART_NUM UART_NUM_1
#define FINGER_UART_TX_PIN (GPIO_NUM_21) // 你选的 TX 引脚
#define FINGER_UART_RX_PIN (GPIO_NUM_20) // 你选的 RX 引脚
#define FINGER_UART_BAUDRATE 57600

// 模块控制 / 中断引脚（如果有的话）
#define FINGERER_INT_PIN (GPIO_NUM_10) // 中断 / 检测引脚（如果模组有的话）

// 接收缓冲区大小
#define FINGER_RX_BUF_SIZE 256

static void IRAM_ATTR fpm383f_int_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    if (gpio_num == FINGERER_INT_PIN)
    {
        // 清除中断标志
        gpio_intr_disable(gpio_num);
        // 读取中断标志
        MY_LOGI("指纹中断产生，指纹被按下");
        gpio_intr_enable(gpio_num); // 重新使能中断
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // 给指纹任务发通知 TODO
      //  vTaskNotifyGiveFromISR(App_IO_KeyScan_Handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void Int_FPM383F_Init(void)
{
    MY_LOGI("Uart_Init start");
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

    MY_LOGI("Intrupt_Init start");
    // 初始化中断引脚----上升沿触发
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << FINGERER_INT_PIN),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
    // install gpio isr service (JUST ONCE CALLED WHEN WHOLE PROGRAM START,SO COMMENT IT)
    // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // hook isr handler for specific gpio pin    UNTODO: check return value
    gpio_isr_handler_add(FINGERER_INT_PIN, fpm383f_int_handler, (void *)FINGERER_INT_PIN);
    // 中断初始化成功
    MY_LOGI("GPIO interrupt initialized successfully");
    MY_LOGI("Int_FPM383F_Init done");
}
