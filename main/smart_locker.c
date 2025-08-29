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
#include "Int_WS2812.h"

void app_main(void)
{
    MY_LOGD("语音模块测试");
    /*初始化*/
    Int_WTN6_Init();
    Int_WTN6_SetCmd(30);
    MY_LOGD("LED模块测试");
    Int_WS2812_Init();

    while (1)
    {
        Int_WS2812_Set_LED_From_Key(KEY_1, &Red_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_Set_LED_From_Key(KEY_2, &Green_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_Set_LED_From_Key(KEY_3, &Blue_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_Set_LED_From_Key(KEY_4, &Yellow_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_Set_LED_From_Key(KEY_5, &Cyan_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_Set_LED_From_Key(KEY_6, &Purple_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_Set_LED_From_Key(KEY_7, &Orange_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_Lighting_All_LED_To_Color(&White_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Int_WS2812_All_LED_Off();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
    // UNTODO: 添加返回
    // return; /*如果添加返回则主任务相当于被kill了*/
}
