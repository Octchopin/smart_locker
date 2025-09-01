#ifndef _INT_SC12B_H_
#define _INT_SC12B_H_

#include "debug/com_debug.h"
#include "driver/i2c.h" // 老版本I2C驱动主要头文件
#include "esp_err.h"    // 提供了esp_err_t类型和ESP_OK等错误码的定义，用于错误处理


#ifdef __cplusplus
extern "C"
{
#endif
    // 按键枚举
typedef enum{
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
        KEY_NO
    } Touch_Key;

  
    void Int_SC12B_Init(void);

#ifdef __cplusplus
}
#endif

#endif // _INT_SC12B_H_
