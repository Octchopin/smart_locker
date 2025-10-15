#include "Int_BDR6120S.h"
// 定义INA和INB的GPIO引脚
#define INA GPIO_NUM_4
#define INB GPIO_NUM_5

// 电机运行时间（单位：ms），可根据实际情况调整
#define MOTOR_RUN_TIME_MS 500

static void Int_BDR6120S_Ready(void)
{
    MY_LOGI("Int_BDR6120S_Ready");
    gpio_set_level(INA, 0);
    gpio_set_level(INB, 0);
}
static void Int_BDR6120S_Forward(void)
{
    MY_LOGI("Int_BDR6120S_Forward");
    gpio_set_level(INA, 1);
    gpio_set_level(INB, 0);
}

static void Int_BDR6120S_Backward(void)
{
    MY_LOGI("Int_BDR6120S_Backward");
    gpio_set_level(INB, 1);
    gpio_set_level(INA, 0);
}

static void Int_BDR6120S_Brake(void)
{
    MY_LOGI("Int_BDR6120S_Brake");
    gpio_set_level(INA, 1);
    gpio_set_level(INB, 1);
}
void Int_BDR6120S_Init(void)
{
    MY_LOGI("Int_BDR6120S_Init");
    // 初始化GPIO句柄
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << INA) | (1ULL << INB),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
    // 初始化电机为就绪状态
    Int_BDR6120S_Ready();
}

void Int_BDR6120S_Unlock(void)
{
    MY_LOGI("Int_BDR6120S_Unlock");
    Int_BDR6120S_Forward();
    vTaskDelay(pdMS_TO_TICKS(MOTOR_RUN_TIME_MS)); // 电机正转一段时间
    Int_BDR6120S_Brake();
    vTaskDelay(pdMS_TO_TICKS(100)); // 刹车短暂延时
    Int_BDR6120S_Ready();
}
void Int_BDR6120S_Lock(void)
{
    MY_LOGI("Int_BDR6120S_Lock");
    Int_BDR6120S_Backward();
    vTaskDelay(pdMS_TO_TICKS(MOTOR_RUN_TIME_MS)); // 电机反转一段时间
    Int_BDR6120S_Brake();
    vTaskDelay(pdMS_TO_TICKS(100)); // 刹车短暂延时
    Int_BDR6120S_Ready();
}