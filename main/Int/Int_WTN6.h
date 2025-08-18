#ifndef _INT_WTN6_H_
#define _INT_WTN6_H_



/*引入头文件*/
#include "debug/com_debug.h"
#include "driver/gpio.h"
#include "esp_task.h"
#include "sys/unistd.h"


#define DATA_PIN GPIO_NUM_9
#define BUSY_PIN GPIO_NUM_7
#define DATA_HIGH gpio_set_level(DATA_PIN, 1)
#define DATA_LOW gpio_set_level(DATA_PIN, 0)
#define BUSY_READ gpio_get_level(BUSY_PIN)

/*常用语音宏定义*/
#define sayWithoutInt() Inf_WTN6170_SendCmd(0xf3)

#define sayNum(x) Inf_WTN6170_SendCmd(x + 1)
#define sayWaterDrop() Inf_WTN6170_SendCmd(11)
#define sayBuzzer() Inf_WTN6170_SendCmd(12)
#define sayAlarm() Inf_WTN6170_SendCmd(13)
#define sayDoorBell() Inf_WTN6170_SendCmd(14)
#define sayFail() Inf_WTN6170_SendCmd(16)
#define sayPassword() Inf_WTN6170_SendCmd(19)
#define sayDoorOpen() Inf_WTN6170_SendCmd(25)
#define sayDoorClose() Inf_WTN6170_SendCmd(26)
#define sayIllegalOperation() Inf_WTN6170_SendCmd(28)
#define sayVerify() Inf_WTN6170_SendCmd(31)
#define sayFinish() Inf_WTN6170_SendCmd(33)
#define sayPressSharp() Inf_WTN6170_SendCmd(34)
#define sayDelSucc() Inf_WTN6170_SendCmd(36)
#define sayDelFail() Inf_WTN6170_SendCmd(37)
#define sayVerifySucc() Inf_WTN6170_SendCmd(41)
#define sayVerifyFail() Inf_WTN6170_SendCmd(42)
#define saySetSucc() Inf_WTN6170_SendCmd(43)
#define saySetFail() Inf_WTN6170_SendCmd(44)
#define sayOperateSucc() Inf_WTN6170_SendCmd(46)
#define sayOperateFail() Inf_WTN6170_SendCmd(47)
#define sayInvalid() Inf_WTN6170_SendCmd(48)
#define sayAddSucc() Inf_WTN6170_SendCmd(49)
#define sayAddFail() Inf_WTN6170_SendCmd(50)
#define sayAddUser() Inf_WTN6170_SendCmd(51)
#define sayDelUser() Inf_WTN6170_SendCmd(52)
#define sayRetry() Inf_WTN6170_SendCmd(58)
#define sayInputUserPassword() Inf_WTN6170_SendCmd(64)
#define sayPasswordAddSucc() Inf_WTN6170_SendCmd(66)
#define sayPasswordAddFail() Inf_WTN6170_SendCmd(67)
#define sayPasswordVerifySucc() Inf_WTN6170_SendCmd(68)
#define sayPasswordVerifyFail() Inf_WTN6170_SendCmd(69)
#define sayAddUserFingerprint() Inf_WTN6170_SendCmd(76)
#define sayDelUserFingerprint() Inf_WTN6170_SendCmd(77)
#define sayPlaceFinger() Inf_WTN6170_SendCmd(80)
#define sayPlaceFingerAgain() Inf_WTN6170_SendCmd(81)
#define sayTakeAwayFinger() Inf_WTN6170_SendCmd(82)
#define sayFingerprintAddSucc() Inf_WTN6170_SendCmd(83)
#define sayFingerprintAddFail() Inf_WTN6170_SendCmd(84)
#define sayFingerprintVerifySucc() Inf_WTN6170_SendCmd(85)
#define sayFingerprintVerifyFail() Inf_WTN6170_SendCmd(86)

#ifdef __cplusplus
extern "C" {
#endif




void Int_WTN6_Init(void);

void Int_WTN6_SetCmd(uint8_t data);



#ifdef __cplusplus
}
#endif

#endif // _INT_WTN6_H_
