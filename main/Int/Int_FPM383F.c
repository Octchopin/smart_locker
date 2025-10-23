/*
 * @FilePath: \smart_locker\main\Int\Int_FPM383F.c
 * @Description:
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-10-16 17:59:45
 * @LastEditTime: 2025-10-23 18:55:44
 * @LastEditors: Vesper Shaw (octxgq@gmail.com)
 * Copyright (c) 2025 by XXX有限公司, All Rights Reserved.
 */
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
// 接收缓冲区---------------用于接受指纹响应的数据
static uint8_t rx_buffer[FINGER_RX_BUF_SIZE];
static void Int_FPM383F_CheckSum(uint8_t *cmd, uint16_t len);
static esp_err_t Int_FPM383F_ReceiveData(uint8_t *rx_buffer, uint16_t data_size, TickType_t ticks_to_wait);
static esp_err_t Int_FPM383F_SendCmd(uint8_t *cmd, uint16_t len);
static void Int_FPM383F_CancelAutoVerifyOrEnroll(void);

static void IRAM_ATTR Fpm383f_int_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    if (gpio_num == FINGERER_INT_PIN)
    {
        // 关闭中断
        gpio_intr_disable(FINGERER_INT_PIN);
        MY_LOGI("指纹中断产生，指纹被按下");
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // 给指纹任务发通知 TODO
        xTaskNotifyFromISR(App_IO_FingerPrintScan_Handle, '3', eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/**
 * @brief   计算并设置命令的校验和
 * @param   cmd  命令数组指针
 * @retval None
 */
static void Int_FPM383F_CheckSum(uint8_t *cmd, uint16_t len)
{

    // 计算校验和 (从包标识开始到参数结束的所有字节之和)
    uint8_t checksum = 0;
    for (int i = 6; i < len - 2; i++)
    {
        checksum += cmd[i];
    }
    cmd[len - 2] = (checksum >> 8) & 0xFF; // 高字节
    cmd[len - 1] = checksum & 0xFF;
}
/**
 * @brief 发送命令到指纹模块
 * @param  cmd  命令数组指针
 * @retval  ESP_OK  成功， ESP_FAIL  失败
 */
static esp_err_t Int_FPM383F_SendCmd(uint8_t *cmd, uint16_t len)
{
    esp_err_t err = ESP_FAIL;
    // 发送命令
    int n = uart_write_bytes(FINGER_UART_NUM, (const char *)cmd, len);
    if (n >= 0)
    {
        MY_LOGI("Sent %d bytes to fingerprint module", n);
        err = ESP_OK;
    }
    return err;
}

/**
 * @brief 接收指纹模块数据
 * @param   rx_buffer  接收缓冲区指针
 * @param   data_size  期望接收的数据大小
 * @param   ticks_to_wait  等待时间
 * @retval  ESP_OK  成功， ESP_FAIL  失败
 */
static esp_err_t Int_FPM383F_ReceiveData(uint8_t *rx_buffer, uint16_t data_size, TickType_t ticks_to_wait)
{

    // 先清空缓冲区
    memset(rx_buffer, 0, data_size);
    // 实际业务接受数据
    esp_err_t err = ESP_FAIL;
    int n = uart_read_bytes(FINGER_UART_NUM, rx_buffer, data_size, ticks_to_wait);
    if (n >= 0)
    {
        MY_LOGI("Received %d bytes from fingerprint module", n);
        err = ESP_OK;
    }
    return err;
}

/***************************************************************具体业务函数******************************************************************** */
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
    gpio_isr_handler_add(FINGERER_INT_PIN, Fpm383f_int_handler, (void *)FINGERER_INT_PIN);
    // 中断初始化成功
    MY_LOGI("GPIO interrupt initialized successfully");
    MY_LOGI("Int_FPM383F_Init done");

    // 读取指纹设备ID测试
    Int_FPM383F_ReadId();
    // 进入睡眠
    Int_FPM383F_Sleep();
}

/**
 * @brief 读取指纹设备ID
 * @param None
 * @retval None
 */
void Int_FPM383F_ReadId(void)
{
    // 发送读取指纹ID设备命令
    uint8_t cmd[] = {
        // 包头
        0xEF, 0x01,
        // 设备地址
        0xFF, 0xFF, 0xFF, 0xFF,
        // 包标识
        0x01,
        // 包长度
        0x00, 0x04,
        // 指令码
        0x34,
        // 参数
        0x00,
        // 校验和
        '\0',
        '\0'};
    // 计算校验和
    Int_FPM383F_CheckSum(cmd, sizeof(cmd));
    // 发送命令
    if (Int_FPM383F_SendCmd(cmd, sizeof(cmd)) == ESP_OK)
    {
        MY_LOGI("ReadId have sent successfully");
    }
    else
    {
        MY_LOGE("ReadId sent failed");
        return;
    }
    // 接收响应
    if (Int_FPM383F_ReceiveData(rx_buffer, 44, 5000 / portTICK_PERIOD_MS) == ESP_OK)
    {
        MY_LOGI("ReadId have received successfully");
        // 处理接收到的数据,第10到41字节是ID信息
        MY_LOGI("Fingerprint id:%.*s", 32, rx_buffer + 10);
    }
    else
    {
        MY_LOGE("ReadId  failed to receive data");
        return;
    }
}
/**
 * @brief  进入睡眠模式
 * @param   none
 * @retval None
 */
void Int_FPM383F_Sleep(void)
{

    // 休眠指纹ID命令
    uint8_t cmd[] = {
        // 包头
        0xEF, 0x01,
        // 设备地址
        0xFF, 0xFF, 0xFF, 0xFF,
        // 包标识
        0x01,
        // 包长度
        0x00, 0x03,
        // 指令码
        0x33,
        // 校验和
        '\0',
        '\0'};
    // 计算校验和
    Int_FPM383F_CheckSum(cmd, sizeof(cmd));
    // 进入休眠模式，判断响应结果是否为0x00，若不是则重新发送指令
    esp_err_t err = ESP_FAIL;
    do
    {

        // 发送命令
        Int_FPM383F_SendCmd(cmd, sizeof(cmd));
        // 读取响应
        err = Int_FPM383F_ReceiveData(rx_buffer, 44, 1000 / portTICK_PERIOD_MS);
        // 打印第9字节的值----响应确认码0x00表示成功，0x01表示失败
        MY_LOGI("Sleep response code: 0x%02X", rx_buffer[9]);
    } while ((rx_buffer[9] != 0x00 && err == ESP_OK) || err != ESP_OK);

    // 说明返回指令正确,第9字节是0x00，进入休眠
    //  启用中断
    gpio_intr_enable(FINGERER_INT_PIN);
}

/**
 * @brief  取消自动验证或者注册
 * @param  None
 * @retval None
 */
static void Int_FPM383F_CancelAutoVerifyOrEnroll(void)
{
    // 取消自动验证命令
    uint8_t cmd[] = {
        // 包头
        0xEF, 0x01,
        // 设备地址
        0xFF, 0xFF, 0xFF, 0xFF,
        // 包标识
        0x01,
        // 包长度
        0x00, 0x03,
        // 指令码
        0x30,
        // 校验和
        '\0',
        '\0'};
    // 计算校验和
    Int_FPM383F_CheckSum(cmd, sizeof(cmd));
    // 循环发送指令，直到收到成功响应
    esp_err_t err = ESP_FAIL;
    do
    {
        // 发送命令
        Int_FPM383F_SendCmd(cmd, sizeof(cmd));
        // 读取响应超时时间1s
        err = Int_FPM383F_ReceiveData(rx_buffer, 44, 1000 / portTICK_PERIOD_MS);
        // 打印第9字节的值----响应确认码0x00表示成功，0x01表示失败
        MY_LOGI("CancelAutoVerifyOrRegister response code: 0x%02X", rx_buffer[9]);
    } while ((rx_buffer[0] != 0x00 && rx_buffer[9] != 0x00) || err != ESP_OK);
}

/**
 * @brief  一站式注册指纹， 包含采集指纹、 生成特征、 组合模板、 存储模板等功能
 * @param  None
 * @retval None
 */
void Int_FPM383F_AutoEnroll(void)
{
    // 停止自动验证或者注册-------------解决芯片内部因为自动注册完不及时清理可能出现bug
    Int_FPM383F_CancelAutoVerifyOrEnroll();
    // ID号
    uint16_t id = 0x01;
    /*参数-----------根据通讯协议指令包说明设置
    bit0-bit5: 0011 1010 ---> 0x3A
    bit0: 1 获取图像成功后灭
    bit1: 1 打开预处理
    bit2: 0 返回关键步骤
    bit3: 1 允许覆盖
    bit4: 1 不允许指纹重复注册
    bit5: 0 不用手指离开
    */
    uint16_t param = 0x3A;
    // 自动注册模板指令包
    uint8_t cmd[] = {
        // 包头
        0xEF, 0x01,
        // 设备地址
        0xFF, 0xFF, 0xFF, 0xFF,
        // 包标识
        0x01,
        // 包长度
        0x00, 0x08,
        // 指令码
        0x31,
        // ID号
        id >> 8, id & 0xFF,
        // 录入次数
        0x02,
        // 参数
        param >> 8, param & 0xFF,
        // 校验和
        '\0',
        '\0'};
    // 计算校验和
    Int_FPM383F_CheckSum(cmd, sizeof(cmd));
    // 发送命令
    Int_FPM383F_SendCmd(cmd, sizeof(cmd));
    // 循环接受不同响应
    while (1)
    {
        // 接受响应包
        Int_FPM383F_ReceiveData(rx_buffer, 44, 5000 / portTICK_PERIOD_MS);
        // 定义确认码、包标识、参数1、参数2
        uint8_t package_identifier = rx_buffer[6];
        uint8_t confirmation_code = rx_buffer[9];
        uint8_t parameter_1 = rx_buffer[10];
        uint8_t parameter_2 = rx_buffer[11];
        // 根据包标识判断是否收到数据，根据确认码判断是否收到正确数据
        if (package_identifier == 0x00)
            continue;
        if (confirmation_code != 0x00)
        {
            // 打印第10字节的值----响应参数100表示成功，0x01表示失败
            MY_LOGI("AutoEnroll failed,conf: 0x%02X", confirmation_code);
            // 指纹注册失败语音提示
            sayFingerprintAddFail();
        
        }else if (parameter_1)
        {
            
        }
    }

    // 取消自动注册，清理状态
    Int_FPM383F_CancelAutoVerifyOrEnroll();
}