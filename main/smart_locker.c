/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "esp_task.h"
#include "esp_log.h"
#include "debug/com_debug.h"
#include "Int_WTN6.h"

void app_main(void)
{
   MY_LOGD("语音模块测试");
    /*初始化*/
    Int_WTN6_Init();
    Int_WTN6_SetCmd(30);
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    return; /*如果添加返回则主任务相当于被kill了*/
}
