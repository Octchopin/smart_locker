#include "App_IO.h"

void App_IO_Init(void)
{
    // 初始化触摸IO
    Int_SC12B_Init();
}

// 系统操作模式枚举
typedef enum {
    NORMAL_MODE,       // 常规输入模式
    SET_PWD_MODE,      // 设置密码模式
    DEL_PWD_MODE       // 删除密码模式
} SystemMode;

// 密码操作状态机
typedef enum {
    IDLE,
    WAIT_OLD_PWD,
    WAIT_NEW_PWD,
    CONFIRM_NEW_PWD
} PwdOperationState;

static SystemMode current_mode = NORMAL_MODE;
static PwdOperationState pwd_state = IDLE;
static uint8_t admin_password[6] = {1,2,3,4,5,6}; // 初始管理员密码
static uint8_t attempt_count = 0;

// 密码操作处理函数
static void handle_password_operation(PasswordBuffer* buf, SystemMode mode, PwdOperationState* state){
    switch(*state){
        case WAIT_OLD_PWD:
            if(verify_password(buf->buffer)){
                *state = WAIT_NEW_PWD;
                MY_LOGI("请输入新密码");
            } else {
                MY_LOGE("原密码错误");
                *state = IDLE;
            }
            break;
        
        case WAIT_NEW_PWD:
            memcpy(admin_password, buf->buffer, 6);
            *state = CONFIRM_NEW_PWD;
            MY_LOGI("请再次输入新密码");
            break;
            
        case CONFIRM_NEW_PWD:
            if(memcmp(admin_password, buf->buffer, 6) == 0){
                save_to_nvs(admin_password);
                MY_LOGI("密码修改成功");
            } else {
                MY_LOGE("两次输入不一致");
            }
            *state = IDLE;
            break;
    }
    memset(buf, 0, sizeof(PasswordBuffer));
}
// 密码输入缓冲管理结构体
typedef struct {
    uint8_t buffer[6];     // 最大支持6位密码
    uint8_t index;         // 当前输入位置
    bool    is_inputting; // 输入状态标志
} PasswordBuffer;

static PasswordBuffer pwd_buf = {0};

// 密码验证函数
static bool verify_password(const uint8_t* input){
    return memcmp(admin_password, input, 6) == 0;
}

// 密码锁定检测
static void check_lock_status(){
    if(++attempt_count >= 3){
        MY_LOGW("密码错误超限，锁定30秒");
        vTaskDelay(pdMS_TO_TICKS(30000));
        attempt_count = 0;
    }
}

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
                    handle_confirm_key(&pwd_buf, &current_mode, &pwd_state);
                    break;
                
                case KEY_NO: // 取消键
                    handle_cancel_key(&pwd_buf, &current_mode, &pwd_state);
                    break;
                
                default:
                    if (KeyValue == KEY_M) {
                        // 长按M键2秒进入管理模式
                        if(check_long_press(KEY_M, 2000)) {
                            current_mode = (current_mode == NORMAL_MODE) ? SET_PWD_MODE : NORMAL_MODE;
                            MY_LOGI("进入%s模式", current_mode==SET_PWD_MODE?"管理":"普通");
                        }
                    }
                    else if (KeyValue != KEY_NO) {
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