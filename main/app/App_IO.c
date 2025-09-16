#include "App_IO.h"

void App_IO_Init(void)
{
    // 初始化触摸IO
    Int_SC12B_Init();
}


void App_IO_read_Task(void *pvParameters)
{
    
   
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // 带超时的任务通知等待
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
        {
            Touch_Key KeyValue = Int_SC12B_Read_TouchKey();
            
           
        }
    }
}