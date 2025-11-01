/*
 * @FilePath: \smart_locker\main\smart_locker.c
 * @Description: 
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-07-30 14:24:39
 * @LastEditTime: 2025-11-01 23:18:11
 * @LastEditors: Vesper Shaw (octxgq@gmail.com)
 * Copyright (c) 2025 by XXX有限公司, All Rights Reserved.
 */
 

#include <stdio.h>
#include "esp_task.h"
#include "esp_log.h"
#include "debug/com_debug.h"
#include "App_IO.h"
#include "App_Commu.h"
// 任务句柄TCB
TaskHandle_t App_IO_KeyScan_Handle = NULL;
TaskHandle_t App_IO_FingerPrintScan_Handle = NULL;
void app_main(void)
{

    /*App初始化*/
    App_IO_Init();
    // 启动通信模块
    App_Commu_Start();
 

    /*创建IO任务用于读写IO */
    xTaskCreate(App_IO_KeyScan_Task, "App_IO_KeyScan_Task", 2048, NULL, 3, &App_IO_KeyScan_Handle);

    xTaskCreate(App_IO_FingerPrintScan_Task, "App_IO_FingerPrintScan_Task", 2048, NULL, 3, &App_IO_FingerPrintScan_Handle);
}
