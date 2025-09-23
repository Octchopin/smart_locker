#ifndef _APP_IO_H_
#define _APP_IO_H_

#include "debug/com_debug.h"
#include "Int_SC12B.h"
#include "esp_task.h"
#include "Int_WS2812.h"
#include "Int_WTN6.h"

#ifdef __cplusplus
extern "C"
{
#endif

// 密码 长度
#define PASSWORD_LEN 4
// 验证密码最大失败次数
#define MAX_FAIL_CNT 3
#define INSTRUCTION_LEN 2

    // 状态枚举
    typedef enum
    {
        STATE_IDLE = 0, // 空闲等待输入
        STATE_INPUTING, // 输入中
    } Input_status_t;
    // 输入状态枚举
    typedef enum
    {
        VERIFY_PWD, // 验证密码
        ADD_PWD,    // 添加密码
        DEL_PWD,    // 删除密码

    } Input_handle_status_t;

    void App_IO_Init(void);
    void App_IO_KeyScan_Task(void *pvParameters);
    void App_IO_AddPwd(void);
    void App_IO_DelPwd(void);
    void App_IO_VerifyPwd(void);

#ifdef __cplusplus
}
#endif

#endif // _APP_IO_H_
