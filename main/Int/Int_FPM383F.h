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

#ifdef __cplusplus
}
#endif

#endif // _INT_FPM383F_H_
