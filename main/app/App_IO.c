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
// 密码输入缓冲管理结构体
typedef struct {
    uint8_t buffer[6];     // 最大支持6位密码
    uint8_t index;         // 当前输入位置
    bool    is_inputting; // 输入状态标志
} PasswordBuffer;

static PasswordBuffer pwd_buf = {0};

void App_IO_read_Task(void *pvParameters)
{
    IO_Status Status = FREE;
    
    // 密码输入超时定时器（30秒）
    const TickType_t xMaxWaitTime = pdMS_TO_TICKS(30000);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // 带超时的任务通知等待
        if (ulTaskNotifyTake(pdTRUE, xMaxWaitTime))
        {
            Touch_Key KeyValue = Int_SC12B_Read_TouchKey();
            
            /* 密码输入状态机 */
            switch(KeyValue)
            {
                case KEY_0 ... KEY_9: // 数字键处理
                    if(pwd_buf.index < 6){
                        pwd_buf.buffer[pwd_buf.index++] = KeyValue - KEY_0;
                        pwd_buf.is_inputting = true;
                        MY_LOGI("输入数字:%d", KeyValue - KEY_0);
                    }
                    break;
                
                case KEY_SHARP: // 确认键
                    if(pwd_buf.index == 6){
                        // 触发密码验证
                        xTaskNotify(pwd_verify_handle, (uint32_t)&pwd_buf, eSetValueWithOverwrite);
                    }
                    memset(&pwd_buf, 0, sizeof(pwd_buf));
                    break;
                
                case KEY_NO: // 取消键
                    memset(&pwd_buf, 0, sizeof(pwd_buf));
                    MY_LOGI("输入已取消");
                    break;
                
                default:
                    if (KeyValue != KEY_NO) {
                        MY_LOGI("触摸按键[%d]被按下", KeyValue);
                    }
            }
            xLastWakeTime = xTaskGetTickCount(); // 重置超时计时
        }
        else // 超时处理
        {
            if(pwd_buf.is_inputting){
                MY_LOGW("输入超时，已重置");
                memset(&pwd_buf, 0, sizeof(pwd_buf));
            }
        }
    }
}