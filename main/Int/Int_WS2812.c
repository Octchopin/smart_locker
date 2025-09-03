/**
 * @file Int_WS2812.c
 * @brief  LCD WS2812 LED strip control using RMT
 * @author Vesper Shaw (octxgq@gmail.com)
 * @version 1.0
 * @date 2025-08-15
 *
 * @copyright Copyright (c) 2025  XXX有限公司
 *
 */
#include "Int_WS2812.h"

// 定义几个常用颜色数组，用于暴漏给外部使用
uint8_t White_LED[3] = {255, 255, 255};
uint8_t Red_LED[3] = {255, 0, 0};
uint8_t Green_LED[3] = {0, 255, 0};
uint8_t Blue_LED[3] = {0, 0, 255};
uint8_t Yellow_LED[3] = {255, 255, 0};
uint8_t Cyan_LED[3] = {0, 255, 255};
uint8_t Purple_LED[3] = {255, 0, 255};
uint8_t Orange_LED[3] = {255, 165, 0};
uint8_t Pink_LED[3] = {255, 192, 203};
uint8_t Brown_LED[3] = {165, 42, 42};
uint8_t Gray_LED[3] = {128, 128, 128};
uint8_t Black_LED[3] = {0, 0, 0};
uint8_t Yellow_Purple_LED[3] = {255, 255, 255};

// 定义全局变量
static uint8_t led_strip_pixels[LED_STRIP_DATA_SIZE] = {0};
static rmt_encoder_handle_t simple_encoder = NULL;
static rmt_channel_handle_t led_chan = NULL;

static const rmt_symbol_word_t ws2812_zero = {
    .level0 = 1,
    .duration0 = 0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T0H=0.3us
    .level1 = 0,
    .duration1 = 0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T0L=0.9us
};

static const rmt_symbol_word_t ws2812_one = {
    .level0 = 1,
    .duration0 = 0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T1H=0.9us
    .level1 = 0,
    .duration1 = 0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000, // T1L=0.3us
};



// reset defaults to 50uS
static const rmt_symbol_word_t ws2812_reset = {
    .level0 = 1,
    .duration0 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
    .level1 = 0,
    .duration1 = RMT_LED_STRIP_RESOLUTION_HZ / 1000000 * 50 / 2,
};

/**
 * @brief Encoder callback function for WS2812 LED strip control.
 * This function encodes the data into RMT symbols for transmission.
 * It handles the encoding of bytes into symbols and manages the end of the transaction.
 *
 * @param data Pointer to the data to be encoded.
 * @param data_size Size of the data in bytes.
 * @param symbols_written Number of symbols already written.
 * @param symbols_free Number of free symbol spaces available.
 * @param symbols Pointer to the array where encoded symbols will be stored.
 * @param done Pointer to a boolean indicating if the encoding is complete.
 * @param arg Opaque user-supplied argument (not used here).
 * @return Number of symbols written by this callback.
 */
static size_t encoder_callback(const void *data, size_t data_size, size_t symbols_written, size_t symbols_free, rmt_symbol_word_t *symbols, bool *done, void *arg)
{
    // We need a minimum of 8 symbol spaces to encode a byte. We only
    // need one to encode a reset, but it's simpler to simply demand that
    // there are 8 symbol spaces free to write anything.
    if (symbols_free < 8)
    {
        return 0;
    }

    // We can calculate where in the data we are from the symbol pos.
    // Alternatively, we could use some counter referenced by the arg
    // parameter to keep track of this.
    size_t data_pos = symbols_written / 8;
    uint8_t *data_bytes = (uint8_t *)data;
    if (data_pos < data_size)
    {
        // Encode a byte
        size_t symbol_pos = 0;
        for (int bitmask = 0x80; bitmask != 0; bitmask >>= 1)
        {
            if (data_bytes[data_pos] & bitmask)
            {
                symbols[symbol_pos++] = ws2812_one;
            }
            else
            {
                symbols[symbol_pos++] = ws2812_zero;
            }
        }
        // We're done; we should have written 8 symbols.
        return symbol_pos;
    }
    else
    {
        // All bytes already are encoded.
        // Encode the reset, and we're done.
        symbols[0] = ws2812_reset;
        *done = 1; // Indicate end of the transaction.
        return 1;  // we only wrote one symbol
    }
}

/******************************************************主业务逻辑********************************************************************************************** */
/**
 * @brief     Send color to PIN
 * This function sends the color data to the LED strip PIN.
 * It uses the RMT TX channel and the simple encoder to transmit the data.
 *
 * @param color_LED The color data to be sent.
 * @param color_LED_size The size of the color data.
 *
 * @return None
 */
static void Int_WS2812_Send_Color_To_PIN(void)
{

    rmt_transmit_config_t tx_config = {
        .loop_count = 0, // no transfer loop
    };
    rmt_transmit(led_chan, simple_encoder, led_strip_pixels, sizeof(led_strip_pixels), &tx_config);
    rmt_tx_wait_all_done(led_chan, portMAX_DELAY);
}

/**
 * @brief     WS2812 init
 * This function initializes the RMT channel for WS2812 LED strip control.
 * It sets up the RMT TX channel configuration, creates a simple encoder,
 * and enables the RMT TX channel.
 */
void Int_WS2812_Init(void)
{
    MY_LOGI("Create RMT TX channel");
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = RMT_LED_STRIP_GPIO_NUM,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    rmt_new_tx_channel(&tx_chan_config, &led_chan);

    MY_LOGI("Create simple callback-based encoder");
    const rmt_simple_encoder_config_t simple_encoder_cfg = {
        .callback = encoder_callback
        // Note we don't set min_chunk_size here as the default of 64 is good enough.
    };
    rmt_new_simple_encoder(&simple_encoder_cfg, &simple_encoder);

    MY_LOGI("Enable RMT TX channel");
    rmt_enable(led_chan);
}

/**
 * @brief 根据传入的按键值和颜色的值，来点亮对应W-S2812 LED
 *
 * @note 颜色值是一个3字节的数组，分别代表RGB三个颜色通道的亮度值
 * @param key_value 按键值
 * @param  color_LED 颜色值
 * @return None
 */
void Int_WS2812_Set_LED_From_Key(Touch_Key key_value, uint8_t (*color_LED)[3])
{

    // 默认清空灯带数据
    memset(led_strip_pixels, 0, sizeof(led_strip_pixels));
    // 点亮对应按键灯带,偏移量是3，因为WS2812灯带是3字节的RGB
    memcpy(led_strip_pixels + key_value * 3, (*color_LED), sizeof((*color_LED)) / sizeof(uint8_t));
    // 发送颜色值到LED灯带
    Int_WS2812_Send_Color_To_PIN();
}

/**
 * @brief 点亮所有LED灯带
 * This function turns on all the LEDs in the LED strip by setting their color values to the specified color
 * and sending the updated color data to the LED strip.
 *
 * @param color_LED 颜色值
 * @return None
 */
void Int_WS2812_Lighting_All_LED_To_Color(uint8_t (*color_LED)[3])
{
    // 点亮所有LED灯带
    for (int i = 0; i < KEY_LED_NUM; i++)
    {
        // 偏移量是3，因为WS2812灯带是3字节的RGB
        memcpy(led_strip_pixels + i * 3, (*color_LED), sizeof((*color_LED)) / sizeof(uint8_t));
    }

    // 发送颜色值到LED灯带
    Int_WS2812_Send_Color_To_PIN();
}
/**
 * @brief 熄灭所有LED灯带
 * This function turns off all the LEDs in the LED strip by setting their color values to zero
 * and sending the updated color data to the LED strip.
 *
 *  @return None
 */
void Int_WS2812_All_LED_Off(void)
{
    // 熄灭所有LED灯带
    Int_WS2812_Lighting_All_LED_To_Color(&Black_LED);
}