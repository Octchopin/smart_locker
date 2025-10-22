/***
 * @Author: Vesper xgq2451115363@163.com
 * @Date: 2025-09-05 09:59:36
 * @LastEditTime: 2025-10-17 18:18:22
 * @LastEditors: Vesper xgq2451115363@163.com
 * @Description: 输入输出模块头文件
 * @FilePath: \smart_locker\main\app\App_IO.h
 * @Copyright (c) 2025 by ${name} email: ${email}, All Rights Reserved.
 */
#ifndef _APP_IO_H_
#define _APP_IO_H_

#include "debug/com_debug.h"
#include "Int_SC12B.h"
#include "esp_task.h"
#include "Int_WS2812.h"
#include "Int_WTN6.h"
#include "Dri_NVS.h"
#include "Int_BDR6120S.h"
#include "Int_FPM383F.h"
#include <string.h> // strcmp 用
#include <stdio.h>  // printf 用
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
    typedef enum
    {

        FINGERPRINT_VERIFY = 0,
        FINGERPRINT_ADD,
        FINGERPRINT_DEL
    } Fingerprint_Operation_State_t;

    void App_IO_Init(void);
    void App_IO_KeyScan_Task(void *pvParameters);
    void App_IO_AddPwd(void);
    void App_IO_DelPwd(void);
    void App_IO_VerifyPwd(void);
    void App_IO_FingerPrintScan_Task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // _APP_IO_H_
