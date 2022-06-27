#ifndef _OTA_H_
#define _OTA_H_

#include <stddef.h>
#include <stdint.h>

void ota_upgradeStart(size_t size);

void ota_upgradeAbort(void);

void ota_recvData(uint8_t *data, uint32_t len);

int ota_checkImage(void);

void ota_reboot(void);

#endif