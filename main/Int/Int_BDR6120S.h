#ifndef _INT_BDR6120S_H_
#define _INT_BDR6120S_H_

#include "debug/com_debug.h"
#include "esp_err.h" // 提供了esp_err_t类型和ESP_OK等错误码的定义，用于错误处理
#include "driver/gpio.h"

// 定义INA和INB的GPIO引脚
#define INA GPIO_NUM_4
#define INB GPIO_NUM_5

// 电机运行时间（单位：ms），可根据实际情况调整
#define MOTOR_RUN_TIME_MS 500

#ifdef __cplusplus
extern "C"
{
#endif
    // 初始化
    void Int_BDR6120S_Init(void);
    // 开锁
    void Int_BDR6120S_Unlock(void);
    // 关锁
    void Int_BDR6120S_Lock(void);

#ifdef __cplusplus
}
#endif

#endif // _INT_BDR6120S_H_
