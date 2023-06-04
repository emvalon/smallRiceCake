/**
 * @file vlonGui_port.c
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-21
 * 
 * Copyright © 2021 - 2022 Weilong Shen (valonshen@foxmail.com)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#include "vlonGui.h"
#include "vlonGui_port.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "vlonGui_ssd1306.h"

struct vlonGui_driver_t vlonGui_driver;

void * vlonGui_malloc(uint32_t size)
{
    return malloc(size);
}

void vlonGui_free(void *addr)
{
    return free(addr);
}

uint32_t vlonGui_getTime(void)
{
    return xTaskGetTickCount();
}

void *
vlonGui_protSemphrCreate(void)
{
    return NULL;
}

void 
vlonGui_protSemphrDestroy(void *semphr)
{

}

void 
vlonGui_portSemphrTake(void *semphr, uint32_t delay_time)
{

}

void 
vlonGui_portSemphrGive(void *semphr)
{

}

static vlonGui_color 
vlonGui_portGetPixel(uint16_t x, uint16_t y)
{
    if (ssd1306_getPixel((uint8_t)x, (uint8_t)y) == White) {
        return VLGUI_COLOR_WHITE;
    } else {
        return VLGUI_COLOR_BLACK;
    }
}

struct vlonGui_driver_t * vlonGui_portGetDriver(void)
{
    memset(&vlonGui_driver, 0, sizeof(vlonGui_driver));
    vlonGui_driver.pInit = ssd1306_Init;
    vlonGui_driver.pDrawPoint = ssd1306_DrawPixel;
    vlonGui_driver.pGetPointColor = vlonGui_portGetPixel;
    vlonGui_driver.pFresh = ssd1306_UpdateScreen;

    return &vlonGui_driver;
}