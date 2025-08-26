#ifndef _INT_SC12B_H_
#define _INT_SC12B_H_

#include "debug/com_debug.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_SHARP,
        KEY_M,
        KEY_NO
    } Touch_Key;

    void Key_Init(void);

#ifdef __cplusplus
}
#endif

#endif // _INT_SC12B_H_
