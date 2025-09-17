#ifndef _APP_IO_H_
#define _APP_IO_H_

#include "debug/com_debug.h"
#include "Int_SC12B.h"
#include "esp_task.h"

#ifdef __cplusplus
extern "C"
{
#endif

//密码 长度
#define PASSWORD_LEN 4
// 验证密码最大失败次数
#define MAX_FAIL_CNT 3

    // 状态枚举
    typedef enum
    {
        STATE_IDLE = 0,  // 空闲等待输入
        STATE_INPUT,     // 输入中
        STATE_VERIFY,    // 验证密码
        STATE_UNLOCKED,  // 解锁成功
        STATE_LOCKED_ERR // 错误锁定
    } LockState;

    void App_IO_Init(void);
    void App_IO_KeyScan_Task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // _APP_IO_H_
