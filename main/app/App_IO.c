#include "App_IO.h"


void App_IO_Init(void)
{
    //初始化触摸IO
    Int_SC12B_Init();
    
}

void   App_IO_read_Task(void *pvParameters)
{
    while (1)
    {
        //任务通知
        if (xTaskNotifyGive(App_IO_read_Handle) == pdTRUE)
        {
            MY_LOGI("任务通知成功");
            //读取触摸Key值
            uint8_t KeyValue = Int_SC12B_Read();
            if (KeyValue != 0)
            {
                MY_LOGI("触摸Key值为: %d", KeyValue);
            }
        }
      
       
    }
}



