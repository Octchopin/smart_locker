#ifndef _DRI_NVS_H_
#define _DRI_NVS_H_

#include "debug/com_debug.h"
#include "nvs_flash.h"
#include "nvs.h"
#ifdef __cplusplus
extern "C"
{
#endif

    void Dri_NVS_Init(void);
    esp_err_t Dri_NVS_IsMatcheKey(const char *key);
    esp_err_t Dri_NVS_DelKey(const char *key);
    esp_err_t Dri_NVS_Write_I8(const char *key, int8_t value);

#ifdef __cplusplus
}
#endif

#endif // _DRI_NVS_H_
