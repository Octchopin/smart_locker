#include "App_IO.h"

void App_IO_Init(void)
{
    // 初始化触摸IO
    Int_SC12B_Init();
}

typedef enum
{
  FREE,
  INPUTTING,
  COMPLETED
  
} IO_Status;
void App_IO_read_Task(void *pvParameters)
{  
    IO_Status Status = FREE;

    while (1)
    {
        // 任务通知
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE)
        {

            // 读取触摸Key值
            Touch_Key KeyValue = Int_SC12B_Read_TouchKey();
            // 枚举能这么判断么
            if (KeyValue != KEY_NO)
            {
                MY_LOGI("触摸按键[%d]键被按下", KeyValue);
            }
        }


    }
}
