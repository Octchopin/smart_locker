/***
 * @FilePath: \smart_locker\main\app\App_Commu.h
 * @Description:
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-11-01 18:05:57
 * @LastEditTime: 2025-11-01 23:28:52
 * @LastEditors: Vesper Shaw (octxgq@gmail.com)
 * @Copyright (c) 2025 by XXX有限公司, All Rights Reserved.
 */
#ifndef _APP_COMMU_H_
#define _APP_COMMU_H_

#include "Dri_BLE.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void App_Commu_Start(void);

    void App_Commu_ReceiveData(uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // _APP_COMMU_H_
