#ifndef _APP_IO_H_
#define _APP_IO_H_

#include "debug/com_debug.h"
#include "Int_SC12B.h"
#include "esp_task.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void App_IO_Init(void);
    void App_IO_read_Task(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif // _APP_IO_H_
