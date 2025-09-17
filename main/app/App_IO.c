#include "App_IO.h"
#include <string.h> // strcmp 用
#include <stdio.h>  // printf 用

// 正确密码
static const char correct_password[PASSWORD_LEN + 1] = "1234";

// 输入缓冲
static char input_buf[PASSWORD_LEN + 1];
static uint8_t input_len = 0;

// 失败计数
static uint8_t fail_cnt = 0;
// 当前状态
static LockState current_state = STATE_IDLE;

void App_IO_Init(void)
{
    Int_SC12B_Init(); // 初始化触摸IO
}

void App_IO_KeyScan_Task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
        {
            Touch_Key KeyValue = Int_SC12B_Read_TouchKey();

            if (KeyValue == KEY_NO)
            {
                if (current_state == STATE_IDLE)
                {
                    continue; // 忽略空闲时的无按键
                }
            }

            switch (current_state)
            {
            case STATE_IDLE:

                if (KeyValue >= KEY_0 && KeyValue <= KEY_9)
                {

                    // 清空输入缓冲密码
                    memset(input_buf, 0, sizeof(input_buf));
                    input_len = 0;
                    input_buf[input_len++] = '0' + (KeyValue - KEY_0);
                    current_state = STATE_INPUT;
                    MY_LOGI("开始输入密码");
                }
                else
                {
                    // todo 其他按键的处理
                    // 可以用特殊的按键长按来进入管理员模式修改密码
                    MY_LOGI("您按下的按键非数字,请重新输入\n");
                }
                break;

            case STATE_INPUT:
                if (KeyValue >= KEY_0 && KeyValue <= KEY_9)
                {
                    if (input_len < PASSWORD_LEN)
                    {
                        input_buf[input_len++] = '0' + (KeyValue - KEY_0);
                    }
                    else
                    {
                        MY_LOGI("⚠️ 已达到最大输入长度\n");
                    }
                }
                else if (KeyValue == KEY_M) // 删除键
                {
                    if (input_len > 0)
                    {
                        input_len--;
                    }
                }
                else if (KeyValue == KEY_SHARP) // 确认键
                {
                    input_buf[input_len] = '\0';
                    current_state = STATE_VERIFY;
                }
                break;

            case STATE_VERIFY:
                if (strcmp(input_buf, correct_password) == 0)
                {
                    MY_LOGI("✅ 密码正确，解锁成功\n");
                    fail_cnt = 0;
                    current_state = STATE_UNLOCKED;
                }
                else
                {
                    printf("❌ 密码错误\n");
                    fail_cnt++;
                    if (fail_cnt >= MAX_FAIL_CNT)
                    {
                        current_state = STATE_LOCKED_ERR;
                        MY_LOGI("系统锁定！\n");
                    }
                    else
                    {
                        current_state = STATE_IDLE;
                        MY_LOGI("请重新输入密码\n");
                    }
                }
                memset(input_buf, 0, sizeof(input_buf));
                input_len = 0;
                break;

            case STATE_UNLOCKED:
                // 这里可以加开锁后的逻辑，比如延时关锁
                MY_LOGI("门已开，请进\n");
                current_state = STATE_IDLE; // 回到空闲
                break;

            case STATE_LOCKED_ERR:
                // 系统锁定，不接受输入
                MY_LOGI("系统锁定中，请稍后再试\n");
                // 这里可以加定时器/管理员复位逻辑
                break;

            default:
                current_state = STATE_IDLE;
                break;
            }
        }
    }
}
