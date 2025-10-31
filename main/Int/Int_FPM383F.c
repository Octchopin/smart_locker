/*
 * @FilePath: \smart_locker\main\Int\Int_FPM383F.c
 * @Description:
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-10-16 17:59:45
 * @LastEditTime: 2025-10-31 16:28:59
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
static void Int_FPM383F_CancelAutoIdentifyOrEnroll(void);
static int16_t Int_FPM383F_GetId(void);
static int16_t Int_FPM383F_GetId_Of_DeletedFingerprint(void);

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
    // 清空串口缓存
    uart_flush(FINGER_UART_NUM);
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
    gpio_set_pull_mode(FINGER_UART_RX_PIN, GPIO_PULLUP_ONLY);
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
static void Int_FPM383F_CancelAutoIdentifyOrEnroll(void)
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
    Int_FPM383F_CancelAutoIdentifyOrEnroll();
    // ID号
    int16_t id = Int_FPM383F_GetId();
    if (id < 0)
    {
        MY_LOGI("Failed to generate fingerprint ID");
        sayFingerprintAddFail();
        return;
    }
    /*参数-----------根据通讯协议指令包说明设置
    bit0-bit5: 0001 1011 ---> 0x1B
    bit0: 1 获取图像成功后灭
    bit1: 1 打开预处理
    bit2: 0 返回关键步骤
    bit3: 1 允许覆盖
    bit4: 1 不允许指纹重复注册
    bit5: 0 不用手指离开
    */
    uint16_t param = 0x1B;
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
        if (package_identifier == 0x07)
            continue;
        if (confirmation_code != 0x00)
        {
            // 打印第10字节的值----响应参数100表示成功，0x01表示失败
            MY_LOGI("AutoEnroll failed,conf: 0x%02X", confirmation_code);
            // 指纹注册失败语音提示
            sayFingerprintAddFail();
            break;
        }
        else if (parameter_1 == 0x03)
        {
            MY_LOGI("Please place the same finger again");
            // 指纹注册第二次采集语音提示---手指离开
            sayTakeAwayFinger();
            // 延迟1s,再次采集
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            if (parameter_1 < 0x02)
            {
                // 语音提示
                sayPlaceFingerAgain();
            }
        }
        else if (parameter_1 == 0x05 && parameter_2 == 0xf1)
        {
            MY_LOGI("AutoEnroll success ");
            // 指纹注册成功语音提示
            sayFingerprintAddSucc();
            break; // 注册成功，退出循环
        }
    }

    // 取消自动注册，清理状态
    Int_FPM383F_CancelAutoIdentifyOrEnroll();
}

/**
 * @brief   获取指纹ID(根据索引表生出id)
 * @param None
 * @retval None
 */
static int16_t Int_FPM383F_GetId(void)
{
    // 获取ID指令包
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
        0x1F,
        // 页码
        0x00,
        // 校验和
        '\0',
        '\0'

    };
    // 计算校验和
    Int_FPM383F_CheckSum(cmd, sizeof(cmd));
    // 发送命令
    Int_FPM383F_SendCmd(cmd, sizeof(cmd));
    // 接收响应
    Int_FPM383F_ReceiveData(rx_buffer, 44, 5000 / portTICK_PERIOD_MS);
    int16_t id = -1;
    // 循环获取到数组第10到42字节的数据
    for (size_t i = 10; i < 42; i++)
    {
        // 获取数组第i字节的数据
        uint8_t byte = rx_buffer[i];
        // 循环获取这个字节的每一位，然后遇到0就计算id值，每次获取完一位右移一位
        for (size_t j = 0; j < 8; j++)
        {
            // 判断是否为0
            if ((byte & 0x01) == 0)
            {
                // 计算
                id = j + (i - 10) * 8;
                return id;
            }
            // 右移一位
            byte >>= 1;
        }
    }
    return id;
}

/**
 * @brief   一站式验证指纹， 包含采集指纹、 生成特征、 比对模板等功能
 * @param   None
 * @retval None
 */
void Int_FPM383F_AutoIdentify(void)
{

    // 清理状态-------------解决芯片内部因为自动验证完不及时清理可能出现bug
    Int_FPM383F_CancelAutoIdentifyOrEnroll();
    // 自动识别指纹指令包
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
        0x32,
        // 分数等级
        0x00,
        // ID号0xffff表示1:N识别
        0xFF, 0xFF,
        // 参数  0x0000表示默认参数
        0x00, 0x00,
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
        // 根据包标识判断是否收到数据，根据确认码判断是否收到正确数据
        if (package_identifier == 0x07)
            continue;
        if (confirmation_code != 0x00)
        {
            // 获取错误信息
            MY_LOGI("AutoIdentify failed,conf: 0x%02X", confirmation_code);
            // 指纹验证失败语音提示
            sayFingerprintVerifyFail();
            break;
        }
        else if (parameter_1 == 0x05 && confirmation_code == 0x00)
        {
            // 指纹验证成功语音提示
            sayFingerprintVerifySucc();
            // 动作电机驱动开锁
            Int_BDR6120S_Unlock();
            // 门已开语音提示
            sayDoorOpen();
            break;
        }
    }

    // 取消自动识别，清理状态
    Int_FPM383F_CancelAutoIdentifyOrEnroll();
}

/**
 * @brief   删除指纹获取ID
 * @param None
 * @retval None
 */
static int16_t Int_FPM383F_GetId_Of_DeletedFingerprint(void)
{
    // 通过删除指纹获取ID
    //  清理状态-------------解决芯片内部因为自动验证完不及时清理可能出现bug
    Int_FPM383F_CancelAutoIdentifyOrEnroll();
    // 自动识别指纹指令包
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
        0x32,
        // 分数等级
        0x00,
        // ID号0xffff表示1:N识别
        0xFF, 0xFF,
        // 参数  0x0000表示默认参数
        0x00, 0x00,
        // 校验和
        '\0',
        '\0'};
    // 计算校验和
    Int_FPM383F_CheckSum(cmd, sizeof(cmd));
    // 发送命令
    Int_FPM383F_SendCmd(cmd, sizeof(cmd));
    // 循环接受不同响应
    int16_t id = -1;
    while (1)
    {
        // 接受响应包
        Int_FPM383F_ReceiveData(rx_buffer, 44, 5000 / portTICK_PERIOD_MS);
        // 定义确认码、包标识、参数1、参数2
        uint8_t package_identifier = rx_buffer[6];
        uint8_t confirmation_code = rx_buffer[9];
        uint8_t parameter_1 = rx_buffer[10];
        // 根据包标识判断是否收到数据，根据确认码判断是否收到正确数据
        if (package_identifier == 0x07)
            continue;
        if (confirmation_code != 0x00)
        {
            // 获取错误信息
            MY_LOGI("Get fingerprint ID failed");
            break;
        }
        else if (parameter_1 == 0x05 && confirmation_code == 0x00)
        {
            // 获取ID
            id = (rx_buffer[11] << 8) | rx_buffer[12];
            break;
        }
    }
    // 取消自动识别，清理状态
    Int_FPM383F_CancelAutoIdentifyOrEnroll();

    return id;
}

/**
 * @brief   删除指纹
 * @param   None
 * @retval None
 */
void Int_FPM383F_DeleteFingerprint(void)
{
    // 获取要删除的指纹ID
    int16_t id = Int_FPM383F_GetId_Of_DeletedFingerprint();
    if (id < 0)
    {
        MY_LOGI("Failed to get fingerprint ID");
        sayDelFail();
        return;
    }
    // 分别获取高八位和第八位
    uint8_t id_high = (uint8_t)(id >> 8);
    uint8_t id_low = (uint8_t)id & 0xFF;
    // 删除指纹指令包
    uint8_t cmd[] = {
        // 包头
        0xEF, 0x01,
        // 设备地址
        0xFF, 0xFF, 0xFF, 0xFF,
        // 包标识
        0x01,
        // 包长度
        0x00, 0x07,
        // 指令码
        0x0c,
        // 页码
        id_high, id_low,
        // 删除个数
        0x00, 0x01,
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
        Int_FPM383F_ReceiveData(rx_buffer, 12, 5000 / portTICK_PERIOD_MS);
        // 获取确认码、包标识
        uint8_t package_identifier = rx_buffer[6];
        uint8_t confirmation_code = rx_buffer[9];
        // 根据包标识判断是否收到数据，根据确认码判断是否
        if (package_identifier == 0x07)
            continue;

        if (confirmation_code == 0x00)
        {
            // 指纹删除成功语音提示
            sayDelUserFingerprint();
            sayDelSucc();
            MY_LOGI("Delete fingerprint succ");
            break;
        }
        else
        {
            // 获取错误信息
            MY_LOGI("Delete fingerprint failed,conf: 0x%02X", confirmation_code);
            // 指纹删除失败语音提示
            sayDelUserFingerprint();
            sayDelFail();
            break;
        }
    }
}

/**
 * @brief   LED控制: 呼吸灯控制---------一般指示灯控制普通呼吸灯
 * @param {uint8_t} fun
 *      1-普通呼吸灯，2-闪烁灯，3-常开灯，4-常闭灯，5-渐开灯，6-渐闭灯
 * @param {uint8_t} startColor
 *      设置为普通呼吸灯时，由灭到亮的颜色，只限于普通呼吸灯（功能码 01）功能，其他功能时，与结束颜色保持一致。
 *      其中，bit0 是蓝灯控制位；bit1 是绿灯控制位；bit2 是红灯控制位。
 *      置 1 灯亮，置 0 灯灭。
 *      例如 0x01_蓝灯亮，
 *           0x02_绿灯亮，
 *           0x04_红灯亮，
 *           0x06_红绿灯亮，
 *           0x05_红蓝灯亮，
 *           0x03_绿蓝灯亮，
 *           0x07_红绿蓝灯亮，
 *           0x00_全灭；
 * @param {uint8_t} endColor
 * @param {uint8_t} cycle
 *              表示呼吸或者闪烁灯的次数。
 *                  当设为 0 时，表示无限循环，
 *                  当设为其他值时，表示呼吸有限次数。
 *              循环次数适用于呼吸、闪烁功能，其他功能中无效，
 *                  例如在常开、常闭、开和渐闭中是无效的；
 * @return {*}
 */
void Int_FPM383F_LedControl(uint8_t fun, uint8_t startColor, uint8_t endColor, uint8_t cycle)
{
    uint8_t cmd[] = {
        0xEF,
        0x01, // 包头
        0xFF,
        0xFF,
        0xFF,
        0xFF, // 默认地址
        0x01, // 包标识
        0x00,
        0x07, // 包长度
        0x3C, // 指令码
        '\0', // 功能码占位符
        '\0', // 起始颜色占位符
        '\0', // 结束颜色占位符
        '\0', // 循环次数占位符
        '\0',
        '\0' // 校验和占位符
    };

    cmd[10] = fun;
    cmd[11] = startColor;
    cmd[12] = endColor;
    cmd[13] = cycle;

    Int_FPM383F_CheckSum(cmd, sizeof(cmd));
    Int_FPM383F_SendCmd(cmd, sizeof(cmd));
    while (1)
    {

        Int_FPM383F_ReceiveData(rx_buffer, 12, portMAX_DELAY);
        if (rx_buffer[6] == 0x07)
            continue;
        if (rx_buffer[9] == 0x00)
        {

            MY_LOGI("LED  is executed successfully");
            break;
        }
        else
        {
            MY_LOGI("LED  fail to execute");
            break;
        }
    }
}
