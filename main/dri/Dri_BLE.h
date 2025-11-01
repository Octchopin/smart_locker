/***
 * @FilePath: \smart_locker\main\dri\Dri_BLE.h
 * @Description:
 * @Author:  Vesper Shaw (octxgq@gmail.com)
 * @Date: 2025-11-01 15:14:50
 * @LastEditTime: 2025-11-01 23:04:15
 * @LastEditors: Vesper Shaw (octxgq@gmail.com)
 * @Copyright (c) 2025 by XXX有限公司, All Rights Reserved.
 */

#ifndef _DRI_BLE_H_
#define _DRI_BLE_H_

#include "debug/com_debug.h"
#include <inttypes.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * DEFINES
     ****************************************************************************************
     */

#define HRPS_HT_MEAS_MAX_LEN (13)

#define HRPS_MANDATORY_MASK (0x0F)
#define HRPS_BODY_SENSOR_LOC_MASK (0x30)
#define HRPS_HR_CTNL_PT_MASK (0xC0)

    /// Attributes State Machine
    enum
    {
        HRS_IDX_SVC,

        HRS_IDX_HR_MEAS_CHAR,
        HRS_IDX_HR_MEAS_VAL,
        HRS_IDX_HR_MEAS_NTF_CFG,

        HRS_IDX_BOBY_SENSOR_LOC_CHAR,
        HRS_IDX_BOBY_SENSOR_LOC_VAL,

        HRS_IDX_HR_CTNL_PT_CHAR,
        HRS_IDX_HR_CTNL_PT_VAL,

        HRS_IDX_NB,
    };
    void Dri_BLE_Init(void);
#ifdef __cplusplus
}
#endif

#endif // _DRI_BLE_H_
