/***
 * @FilePath: \smart_locker\main\Int\Int_FPM383F.h
 * @Description:
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-10-25 19:38:22
 * @LastEditTime: 2025-10-31 15:35:22
 * @LastEditors: Vesper Shaw (octxgq@gmail.com)
 * @Copyright (c) 2025 by XXX有限公司, All Rights Reserved.
 */
/***
 * @FilePath: \smart_locker\main\Int\Int_FPM383F.h
 * @Description: 指纹模块FPM383F接口函数头文件
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-10-16 17:59:59
 * @LastEditTime: 2025-10-30 18:50:23
 * @LastEditors: Vesper Shaw (octxgq@gmail.com)
 * @Copyright (c) 2025 by XXX有限公司, All Rights Reserved.
 */
#ifndef _INT_FPM383F_H_
#define _INT_FPM383F_H_

#include "debug/com_debug.h"
#include "esp_err.h" // 提供了esp_err_t类型和ESP_OK等错误码的定义，用于错误处理
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_task.h"
#include "Int_WTN6.h"
#include "Int_BDR6120S.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern TaskHandle_t App_IO_FingerPrintScan_Handle;

    void Int_FPM383F_Init(void);
    void Int_FPM383F_ReadId(void);
    void Int_FPM383F_Sleep(void);
    void Int_FPM383F_AutoIdentify(void);
    void Int_FPM383F_AutoEnroll(void);
    void Int_FPM383F_DeleteFingerprint(void);
    void Int_FPM383F_LedControl(uint8_t fun, uint8_t startColor, uint8_t endColor, uint8_t cycle);
// 定义LED控制的宏函数
#define LED_RED_FLICKER Int_FPM383F_LedControl(2, 0x04, 0x04, 0)
#define LED_GREEN_FLICKER Int_FPM383F_LedControl(2, 0x02, 0x02, 0)
#define LED_BLUE_FLICKER Int_FPM383F_LedControl(2, 0x01, 0x01, 0)

#define LED_RED_ON Int_FPM383F_LedControl(3, 0x04, 0x04, 0)
#define LED_GREEN_ON Int_FPM383F_LedControl(3, 0x02, 0x02, 0)
#define LED_BLUE_ON Int_FPM383F_LedControl(3, 0x01, 0x01, 0)

#define LED_COLOR_OFF Int_FPM383F_LedControl(4, 0x07, 0x07, 0)

#ifdef __cplusplus
}
#endif

#endif // _INT_FPM383F_H_
