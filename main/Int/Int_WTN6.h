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
#define sayWithoutInt() Int_WTN6_SetCmd(0xf3)

#define sayNum(x) Int_WTN6_SetCmd(x + 1)
#define sayWaterDrop() Int_WTN6_SetCmd(11)
#define sayBuzzer() Int_WTN6_SetCmd(12)
#define sayAlarm() Int_WTN6_SetCmd(13)
#define sayDoorBell() Int_WTN6_SetCmd(14)
#define sayFail() Int_WTN6_SetCmd(16)
#define sayPassword() Int_WTN6_SetCmd(19)
#define sayDoorOpen() Int_WTN6_SetCmd(25)
#define sayDoorClose() Int_WTN6_SetCmd(26)
#define sayIllegalOperation() Int_WTN6_SetCmd(28)
#define sayVerify() Int_WTN6_SetCmd(31)
#define sayFinish() Int_WTN6_SetCmd(33)
#define sayPressSharp() Int_WTN6_SetCmd(34)
#define sayDelSucc() Int_WTN6_SetCmd(36)
#define sayDelFail() Int_WTN6_SetCmd(37)
#define sayVerifySucc() Int_WTN6_SetCmd(41)
#define sayVerifyFail() Int_WTN6_SetCmd(42)
#define saySetSucc() Int_WTN6_SetCmd(43)
#define saySetFail() Int_WTN6_SetCmd(44)
#define sayOperateSucc() Int_WTN6_SetCmd(46)
#define sayOperateFail() Int_WTN6_SetCmd(47)
#define sayInvalid() Int_WTN6_SetCmd(48)
#define sayAddSucc() Int_WTN6_SetCmd(49)
#define sayAddFail() Int_WTN6_SetCmd(50)
#define sayAddUser() Int_WTN6_SetCmd(51)
#define sayDelUser() Int_WTN6_SetCmd(52)
#define sayRetry() Int_WTN6_SetCmd(58)
#define sayInputUserPassword() Int_WTN6_SetCmd(64)
#define sayPasswordAddSucc() Int_WTN6_SetCmd(66)
#define sayPasswordAddFail() Int_WTN6_SetCmd(67)
#define sayPasswordVerifySucc() Int_WTN6_SetCmd(68)
#define sayPasswordVerifyFail() Int_WTN6_SetCmd(69)
#define sayAddUserFingerprint() Int_WTN6_SetCmd(76)
#define sayDelUserFingerprint() Int_WTN6_SetCmd(77)
#define sayPlaceFinger() Int_WTN6_SetCmd(80)
#define sayPlaceFingerAgain() Int_WTN6_SetCmd(81)
#define sayTakeAwayFinger() Int_WTN6_SetCmd(82)
#define sayFingerprintAddSucc() Int_WTN6_SetCmd(83)
#define sayFingerprintAddFail() Int_WTN6_SetCmd(84)
#define sayFingerprintVerifySucc() Int_WTN6_SetCmd(85)
#define sayFingerprintVerifyFail() Int_WTN6_SetCmd(86)

#ifdef __cplusplus
extern "C" {
#endif




void Int_WTN6_Init(void);

void Int_WTN6_SetCmd(uint8_t data);



#ifdef __cplusplus
}
#endif

#endif // _INT_WTN6_H_
