#include "Dri_NVS.h"

nvs_handle_t my_handle;

static void Dri_NVS_Open(nvs_handle_t* handle)
{
    // Open NVS
    MY_LOGI("\n");
    MY_LOGI("Opening Non-Volatile Storage (NVS) handle... ");

    esp_err_t err = nvs_open("storage", NVS_READWRITE, handle);
    if (err != ESP_OK)
    {
        MY_LOGI("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        MY_LOGI("Done\n");
        MY_LOGI("NVS is opened successfully");
    }
}

static void Dri_NVS_Close(nvs_handle_t handle)
{
    nvs_close(handle);
    MY_LOGI("NVS handle is closed");
}

void Dri_NVS_Init(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    MY_LOGI("NVS is initialized successfully");
}

esp_err_t Dri_NVS_Write_I8( const char *key, int8_t value)
{
    // open nvs
    Dri_NVS_Open(&my_handle);
    // Write
    MY_LOGI("Writing NVS... ");
    esp_err_t err = nvs_set_i8(my_handle, key, value);
    if (err != ESP_OK)
    {
        MY_LOGI("Error (%s) writing NVS!\n", esp_err_to_name(err));
        return err;
    }
    // Commit written value.
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        MY_LOGI("Error (%s) commiting NVS!\n", esp_err_to_name(err));
        return err;
    }
    // Close NVS
    Dri_NVS_Close(my_handle);
    return err;
}
/**
 * @brief  匹配密码正确与否，如123456密码，输入密码123456，则返回正确
 *         输入密码123 密码错误，则返回错误
 *           输入密码1123456789 ，密码正确，则返回正确
 * 
 * @param  key     Comment
 * @param  value   Comment
 * 
 */
void Dri_NVS_MatcheKey(const char *key, int8_t value)
{
    Dri_NVS_Open(&my_handle);
    
    int8_t stored_value;
    char stored_str[8] = {0};
    char input_str[8] = {0};
    
    /* 读取存储值 */
    esp_err_t err = nvs_get_i8(my_handle, key, &stored_value);
    if (err != ESP_OK) {
        MY_LOGE("密码读取失败: %s", esp_err_to_name(err));
        Dri_NVS_Close(my_handle);
        return;
    }
    
    /* 数值转字符串 */
    snprintf(stored_str, sizeof(stored_str), "%d", stored_value);
    snprintf(input_str, sizeof(input_str), "%d", value);
    
    /* 匹配逻辑 */
    if (strstr(input_str, stored_str) != NULL) {
        MY_LOGI("密码验证通过 (输入:%s 存储:%s)", input_str, stored_str);
    } else {
        MY_LOGW("密码验证失败 (输入:%s 存储:%s)", input_str, stored_str);
    }
    
    Dri_NVS_Close(my_handle);
}