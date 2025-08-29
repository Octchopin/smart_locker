
#ifndef _INT_WS2812_H_
#define _INT_WS2812_H_

/*引入头文件*/
#include "debug/com_debug.h"
#include "driver/rmt_tx.h"
#include "Int_SC12B.h"

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM 6             // 根据CPU链接的端口
// 门锁的12个按键,每个按键对应一个发光LED,每个LED占3个字节
#define KEY_NUM 12
// 定义LED的数量
#define KEY_LED_NUM KEY_NUM
// 单个LED的数据结构大小3字节（参考WS2812数据格式）
#define SiNGLE_LED_STRIP_DATA_SIZE 3
// 定义LED灯带的数据数组
#define LED_STRIP_DATA_SIZE (KEY_LED_NUM * SiNGLE_LED_STRIP_DATA_SIZE)

#ifdef __cplusplus
extern "C"
{
#endif

    // 声明几个常用颜色数组，用于暴漏给外部使用
    extern uint8_t White_LED[3];
    extern uint8_t Red_LED[3];
    extern uint8_t Green_LED[3];
    extern uint8_t Blue_LED[3];
    extern uint8_t Yellow_LED[3];
    extern uint8_t Cyan_LED[3];
    extern uint8_t Purple_LED[3];
    extern uint8_t Orange_LED[3];
    extern uint8_t Pink_LED[3];
    extern uint8_t Brown_LED[3];
    extern uint8_t Gray_LED[3];
    extern uint8_t Black_LED[3];
    extern uint8_t Yellow_Purple_LED[3];
    // 声明函数
    void Int_WS2812_Init(void);
    void Int_WS2812_Set_LED_From_Key(Touch_Key key_value, uint8_t (*color_LED)[3]);
    void Int_WS2812_Lighting_All_LED_To_Color(uint8_t (*color_LED)[3]);
    void Int_WS2812_All_LED_Off(void);

#ifdef __cplusplus
}
#endif

#endif // _INT_WS2812_H_
