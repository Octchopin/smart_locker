/*
 * @FilePath: \smart_locker\main\app\App_Commu.c
 * @Description:
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-11-01 18:06:25
 * @LastEditTime: 2025-11-01 23:33:49
 * @LastEditors: Vesper Shaw (octxgq@gmail.com)
 * Copyright (c) 2025 by XXX有限公司, All Rights Reserved.
 */
#include "App_Commu.h"

void App_Commu_Start(void)
{
    Dri_BLE_Init();
}

/**
 * @brief
 * @param data: 接收的数据指针
 * @param len: 接收的数据长度
 * @retval None
 */
void App_Commu_ReceiveData(uint8_t *data, uint16_t len)
{

    // 获取数据和长度的处理的业务逻辑TODO
    MY_LOGI("App_Commu_ReceiveData: %.*s", len, data);
    
}