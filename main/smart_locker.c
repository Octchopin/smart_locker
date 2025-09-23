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
#include "Int_SC12B.h"
#include "App_IO.h"
// 任务句柄TCB
TaskHandle_t App_IO_KeyScan_Handle = NULL;
void app_main(void)
{

    /*测试中断*/
    App_IO_Init();
    /*创建IO任务用于读写IO */
    xTaskCreate(App_IO_KeyScan_Task, "App_IO_KeyScan_Task", 2048, NULL, 3, &App_IO_KeyScan_Handle);
}
