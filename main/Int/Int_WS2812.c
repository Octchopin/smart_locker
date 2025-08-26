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

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_GPIO_NUM 6             // 根据CPU链接的端口

rmt_channel_handle_t led_chan = NULL;

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

static size_t encoder_callback(const void *data, size_t data_size, size_t symbols_written, size_t symbols_free, rmt_symbol_word_t *symbols, bool *done, void *arg);

/**
 * @brief     WS2812 init
 * This function initializes the RMT channel for WS2812 LED strip control.
 * It sets up the RMT TX channel configuration, creates a simple encoder,
 * and enables the RMT TX channel.
 */
void Int_WS2812_Init(void)
{
    MY_LOGI("Create RMT TX channel");
    rmt_channel_handle_t led_chan = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = RMT_LED_STRIP_GPIO_NUM,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    rmt_new_tx_channel(&tx_chan_config, &led_chan);

    MY_LOGI("Create simple callback-based encoder");
    rmt_encoder_handle_t simple_encoder = NULL;
    const rmt_simple_encoder_config_t simple_encoder_cfg = {
        .callback = encoder_callback
        // Note we don't set min_chunk_size here as the default of 64 is good enough.
    };
    rmt_new_simple_encoder(&simple_encoder_cfg, &simple_encoder);

    MY_LOGI("Enable RMT TX channel");
    rmt_enable(led_chan);
}


/**
 * @brief 根据传入的按键值和颜色的值，来点亮对应WLED
 *
 * @param key_value 按键值
 * @param  color_LED
 *
 */
*/
    void Int_WS2812_Set_LED(uint8_t key_value, uint32_t *color_LED)
{

    // 默认熄灭所以灯带

    // 点亮对应按键灯带
}

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