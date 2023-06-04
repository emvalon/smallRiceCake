#include "ota.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "esp_log.h"
#include "esp_system.h"

#define TAG  "ota"

static const esp_partition_t * partition;
static esp_ota_handle_t handle;
static uint32_t offset;
static size_t imageSize; 
static uint8_t percentage;


void
ota_upgradeStart(size_t size)
{
    esp_err_t err;

    ESP_LOGI(TAG, "ota_upgradeStart, size:%d", size);

    offset = 0;
    partition = esp_ota_get_next_update_partition(NULL);
    if (!partition) {
        ESP_LOGW(TAG, "No partition found!");
        return;
    }

    err = esp_ota_begin((const esp_partition_t *)partition, size, &handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Partition Erase error:%d", err);
        partition = NULL;
    }

    imageSize = size;
    percentage = 0;
}

void
ota_upgradeAbort(void)
{   
    ESP_LOGI(TAG, "ota_upgradeAbort");
    
    esp_ota_end(handle);
    partition = NULL;
}


void 
ota_recvData(uint8_t *data, uint32_t len)
{
    esp_err_t err;
    uint8_t localPrecent;


    if (partition) {
        err = esp_ota_write_with_offset(handle, data, len, offset);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Partition Write error:%d", err);
            partition = NULL;
        }
        offset += len;
        localPrecent = (uint8_t)(offset * 100 / imageSize);
        if (localPrecent != percentage) {
            ESP_LOGI(TAG, "%d%%", localPrecent);
            percentage = localPrecent;
        }
    } else {
        ESP_LOGI(TAG, "no partition");
    }
}

int 
ota_checkImage(void)
{
    esp_err_t err;


    ESP_LOGI(TAG, "ota_checkImage");

    if (partition) {
        err = esp_ota_end(handle);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Partition check error:%d", err);
            partition = NULL;
            return -1;
        }

        return 0;
    } else {
        ESP_LOGI(TAG, "no partition");
        return -1;
    }
}

void 
ota_reboot(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "ota_reboot");

    if (partition) {
        err = esp_ota_set_boot_partition(partition);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Partition set error:%d", err);
            partition = NULL;
            return;
        }
        esp_restart();
        return;
    } else {
        ESP_LOGI(TAG, "no partition");
    }
}