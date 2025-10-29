#include "App_IO.h"

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
    // 初始化按键扫描任务
    Int_SC12B_Init();
    // 初始化WS2812---LED模块
    Int_WS2812_Init();
    // 初始化WTN6--语音模块
    Int_WTN6_Init();
    // 初始化NVS
    Dri_NVS_Init();
    // 电机初始化
    Int_BDR6120S_Init();
    // 初始化指纹模块
    Int_FPM383F_Init();

    // 初始化亮屏同时提示音再熄灭，延迟一秒
    Int_WS2812_Lighting_All_LED_To_Color(&White_LED);
    sayDoorBell();
    vTaskDelay(pdMS_TO_TICKS(1000));
    Int_WS2812_Lighting_All_LED_To_Color(&Black_LED);
}

void App_IO_AddPwd(void)
{

    // 添加密码
    esp_err_t err = Dri_NVS_Write_I8(input_buf, 0);
    if (err != ESP_OK)
    {
        MY_LOGI("add pwd failed");
        sayPasswordAddFail();
        return;
    }
    else
    {
        MY_LOGI("add pwd success");
        // 添加成功提示音
        sayPasswordAddSucc();
    }
}
void App_IO_DelPwd(void)
{
    // 删除密码
    esp_err_t err = Dri_NVS_DelKey(input_buf);
    if (err != ESP_OK)
    {
        MY_LOGI("del pwd failed");
        sayPassword();
        sayDelFail();
        return;
    }
    else
    {
        MY_LOGI("del pwd success");
        // 删除成功提示音
        sayPassword();
        sayDelSucc();
    }
}
void App_IO_VerifyPwd(void)
{
    // 验证密码
    esp_err_t err = Dri_NVS_IsMatcheKey(input_buf);
    if (err != ESP_OK)
    {
        MY_LOGI("verify pwd failed");
        sayPasswordVerifyFail();
        return;
    }
    else
    {
        MY_LOGI("verify pwd success");
        // 验证成功提示音
        sayPasswordVerifySucc();
        // 开锁
        Int_BDR6120S_Unlock();
        // 锁已开提示音
        sayDoorOpen();
    }
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
        else if (input_buf[0] == '1') // 指纹识别11为添加指纹，12为删除指纹，13为识别指纹
        {
            // 指纹
            MY_LOGI("指纹识别\n");
            if (input_buf[1] == '1')
            {
                // 添加指纹
                MY_LOGI("添加指纹\n");
                // 添加指纹任务通知
                xTaskNotify(App_IO_FingerPrintScan_Handle, input_buf[1], eSetValueWithOverwrite);
            }
            else if (input_buf[1] == '2')
            {
                // 删除指纹
                MY_LOGI("删除指纹\n");
                // 删除指纹任务通知
                xTaskNotify(App_IO_FingerPrintScan_Handle, input_buf[1], eSetValueWithOverwrite);
            }
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

void App_IO_FingerPrintScan_Task(void *pvParameters)
{

    MY_LOGI("指纹任务启动");
    while (1)
    {
        Fingerprint_Operation_State_t fingerprint_operation_state = FINGERPRINT_VERIFY;
        uint32_t ulNotifiedValue;
        // 等待指纹中断通知
        xTaskNotifyWait(0, ULONG_MAX, &ulNotifiedValue, portMAX_DELAY);
        MY_LOGI("value notified is:%c", (char)ulNotifiedValue);
        // 执行指纹逻辑
        switch (ulNotifiedValue)
        {
        case '1':
        {
            // 添加用户指纹提示音-----
            sayAddUserFingerprint();
            // 请放置手指
            sayPlaceFinger();
            // 改变指纹操作状态为添加指纹
            fingerprint_operation_state = FINGERPRINT_ADD;
            break;
        }

        case '2':
        {
            // 删除用户指纹提示音-----
            sayDelUserFingerprint();
            // 请放置手指
            sayPlaceFinger();
            // 改变指纹操作状态为删除指纹
            fingerprint_operation_state = FINGERPRINT_DEL;
            break;
        }
        case '3':
        {
            // 根据状态放置指纹操作逻辑
            if (fingerprint_operation_state == FINGERPRINT_VERIFY)
            {
                MY_LOGI("验证指纹");
                // 验证指纹逻辑 TODO
            }
            else if (fingerprint_operation_state == FINGERPRINT_ADD)
            {
                MY_LOGI("添加指纹");
                // 添加指纹逻辑 TODO
            }
            else if (fingerprint_operation_state == FINGERPRINT_DEL)
            {
                MY_LOGI("删除指纹");
                // 删除指纹逻辑 TODO
            }
            Int_FPM383F_Sleep();
            break;
        }
        default:
            break;
        }
        // 执行完进入睡眠，等待下次中断
        Int_FPM383F_Sleep();
    }
}
