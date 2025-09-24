#include "App_IO.h"
#include <string.h> // strcmp 用
#include <stdio.h>  // printf 用
// 输入缓冲
static char input_buf[PASSWORD_LEN + 1];
static uint8_t input_len = 0;
// 输入处理状态
static Input_handle_status_t input_handle_state = VERIFY_PWD;
// 当前状态
static Input_status_t current_state = STATE_IDLE;

static void APP_IO_Input_Handle(void);
void App_IO_Init(void)
{
    // 初始化触摸IO---感应电容模块
    Int_SC12B_Init();
    // 初始化WS2812---LED模块
    Int_WS2812_Init();
    // 初始化WTN6--语音模块
    Int_WTN6_Init();
    // 初始化亮屏同时提示音再熄灭，延迟一秒
    Int_WS2812_Lighting_All_LED_To_Color(&White_LED);
    sayDoorBell();
    vTaskDelay(pdMS_TO_TICKS(1000));
    Int_WS2812_Lighting_All_LED_To_Color(&Black_LED);
}

void App_IO_AddPwd(void)
{
    // 添加成功提示音
    sayAddSucc();
}
void App_IO_DelPwd(void)
{
    // 删除成功提示音
    sayDelSucc();
}
void App_IO_VerifyPwd(void)
{
    // 验证成功提示音
    sayVerifySucc();
}

void App_IO_KeyScan_Task(void *pvParameters)
{

    while (1)
    {
        // 任务延迟等待按键按下
        Touch_Key KeyValue = Int_SC12B_Read_TouchKey();

        if (KeyValue == KEY_NO)
        {
            current_state = STATE_IDLE;
            MY_LOGI("无按键按下");
            // 超过五秒没有按键按下,所以灯熄灭
            Int_WS2812_Lighting_All_LED_To_Color(&Black_LED);
            continue; // 忽略空闲时的无按键
        }

        switch (current_state)
        {
        case STATE_IDLE:
        {
            // 触摸任意按键，点亮所有按键灯带为白色
            Int_WS2812_Lighting_All_LED_To_Color(&White_LED);
            // 清空输入缓存
            memset(input_buf, 0, sizeof(input_buf));
            input_len = 0;
            current_state = STATE_INPUTING;
            MY_LOGI("开始输入密码");
        }
        break;

        case STATE_INPUTING:
            // 点亮当前按键的灯带为蓝色
            Int_WS2812_Set_LED_From_Key(KeyValue, &Blue_LED);
            // 按键提示音----水滴提示音
            sayWaterDrop();
            if (KeyValue >= KEY_0 && KeyValue <= KEY_9)
            {

                input_buf[input_len++] = '0' + (KeyValue - KEY_0);
            }
            else if (KeyValue == KEY_M) // 无效输入键
            {
                MY_LOGI("无效输入");
                // 无效输入提示音
                sayInvalid();
                current_state = STATE_IDLE;
            }
            else if (KeyValue == KEY_SHARP) // 确认键
            {

                input_buf[input_len] = '\0';
                APP_IO_Input_Handle();
                // 如果删除或者添加密码，状态不变，同时清空输入缓冲区input_buf
                if (input_handle_state == ADD_PWD || input_handle_state == DEL_PWD)
                {

                    //  清空输入缓冲密码
                    memset(input_buf, 0, sizeof(input_buf));
                    input_len = 0;
                    // 方便下一次直接输入
                    current_state = STATE_INPUTING;
                }
                else
                {
                    current_state = STATE_IDLE;
                }
            }
            // 按完确认,点亮所有灯为白色
            Int_WS2812_Lighting_All_LED_To_Color(&White_LED);
            break;

        default:
            current_state = STATE_IDLE;
            break;
        }
    }
}

static void APP_IO_Input_Handle(void)
{

    MY_LOGI("输入的密码是：%.*s\n", input_len, input_buf);
    if (!(input_len >= PASSWORD_LEN || input_len == INSTRUCTION_LEN))
    {
        MY_LOGI("无效输入\n");
        // 无效输入提示音
        sayInvalid();
        return;
    }
    // 指令逻辑业务分支
    if (input_len == INSTRUCTION_LEN && (input_handle_state != VERIFY_PWD && input_handle_state != ADD_PWD))
    {
        MY_LOGI("开始处理输入的密码!!!");
        if (input_buf[0] == '0')
        {
            if (input_buf[1] == '1')
            {
                // 添加密码
                MY_LOGI("添加密码\n");
                // 提示添加用户密码提示音
                sayAddUser();
                sayPassword();
                // 指令状态转换
                input_handle_state = ADD_PWD;
            }
            if (input_buf[1] == '2')
            {
                // 删除密码
                MY_LOGI("删除密码\n");
                // 提示删除用户密码提示音
                sayDelUser();
                sayPassword();
                input_handle_state = DEL_PWD;
            }
        }
        else if (input_buf[0] == '1')
        {
            // 指纹
            MY_LOGI("指纹识别\n");
        }
    }
    else if (input_len == 2)
    {
        MY_LOGI("输入密码错误，不能使用俩位密码，%.*s", input_len, input_buf);
        // 非法输入提示音
        sayIllegalOperation();
        return;
    }
    else if (input_len >= PASSWORD_LEN)
    {

        if (input_handle_state == VERIFY_PWD)
        {
            MY_LOGI("验证密码\n");
            MY_LOGI("verifing pwd is %.*s\n", input_len, input_buf);
            App_IO_VerifyPwd();
        }
        else if (input_handle_state == ADD_PWD)
        {
            MY_LOGI("添加密码\n");
            MY_LOGI("add pwd is %.*s\n", input_len, input_buf);
            App_IO_AddPwd();
        }
        else if (input_handle_state == DEL_PWD)
        {
            MY_LOGI("删除密码\n");
            MY_LOGI("del pwd is %.*s\n", input_len, input_buf);
            App_IO_DelPwd();
        }
        // 处理完指令后，重置状态
        input_handle_state = VERIFY_PWD;
    }
}