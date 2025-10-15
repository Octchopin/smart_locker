#ifndef _INT_BDR6120S_H_
#define _INT_BDR6120S_H_


#include "debug/com_debug.h"
#include "esp_err.h"    // 提供了esp_err_t类型和ESP_OK等错误码的定义，用于错误处理
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

void Int_BDR6120S_Init(void);

#ifdef __cplusplus
}
#endif

#endif // _INT_BDR6120S_H_
