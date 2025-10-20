#ifndef _INT_FPM383F_H_
#define _INT_FPM383F_H_

#include "debug/com_debug.h"
#include "esp_err.h" // 提供了esp_err_t类型和ESP_OK等错误码的定义，用于错误处理
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_task.h"

#ifdef __cplusplus
extern "C" {
#endif

extern TaskHandle_t App_IO_FingerPrintScan_Handle;
void Int_FPM383F_Init(void);
void Int_FPM383F_ReadId(void);

void Int_FPM383F_Sleep(void);

#ifdef __cplusplus
}
#endif

#endif // _INT_FPM383F_H_
