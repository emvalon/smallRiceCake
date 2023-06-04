/**
 * @file vlGui_port.c
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
#include "vlGui.h"
#include "vlGui_port.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "vlGui_ssd1306.h"

struct vlGui_driver_t vlGui_driver;

void * vlGui_malloc(uint32_t size)
{
    return malloc(size);
}

void vlGui_free(void *addr)
{
    return free(addr);
}

uint32_t vlGui_getTime(void)
{
    return xTaskGetTickCount();
}

void *
vlGui_protSemphrCreate(void)
{
    return NULL;
}

void 
vlGui_protSemphrDestroy(void *semphr)
{

}

void 
vlGui_portSemphrTake(void *semphr, uint32_t delay_time)
{

}

void 
vlGui_portSemphrGive(void *semphr)
{

}

static vlGui_color 
vlGui_portGetPixel(uint16_t x, uint16_t y)
{
    if (ssd1306_getPixel((uint8_t)x, (uint8_t)y) == White) {
        return VLGUI_COLOR_WHITE;
    } else {
        return VLGUI_COLOR_BLACK;
    }
}

struct vlGui_driver_t * vlGui_portGetDriver(void)
{
    memset(&vlGui_driver, 0, sizeof(vlGui_driver));
    vlGui_driver.pInit = ssd1306_Init;
    vlGui_driver.pDrawPoint = ssd1306_DrawPixel;
    vlGui_driver.pGetPointColor = vlGui_portGetPixel;
    vlGui_driver.pFresh = ssd1306_UpdateScreen;

    return &vlGui_driver;
}