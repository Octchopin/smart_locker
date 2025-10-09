#include "Dri_NVS.h"

nvs_handle_t my_handle;

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

    // Open NVS
    MY_LOGI("\n");
    MY_LOGI("Opening Non-Volatile Storage (NVS) handle... ");

    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        MY_LOGI("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        MY_LOGI("Done\n");

        MY_LOGI("NVS is initialized successfully");
    }
}

void Dri_NVS_Deinit(nvs_handle_t handle)
{
    nvs_close(handle);
    MY_LOGI("NVS handle is closed");
}

void Dri_NVS_Write(nvs_handle_t handle, const char *key, const char *value)
{
    // Write
    MY_LOGI("Writing NVS... ");
    esp_err_t err = nvs_set_str(handle, key, value);
    if (err != ESP_OK)
    {
        MY_LOGI("Error (%s) writing NVS!\n", esp_err_to_name(err));
        return;
    }
    // Commit written value.
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        MY_LOGI("Error (%s) commiting NVS!\n", esp_err_to_name(err));
        return;
    }
}
