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
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

// static void vlonGui_portDrawPixel(uint16_t x, uint16_t y, uint8_t color)
// {
//     ssd1306_DrawPixel(x, y, color);
// }

struct vlonGui_driver_t * vlonGui_portGetDriver(void)
{
    vlonGui_driver.pInit = ssd1306_Init;
    vlonGui_driver.pDrawPoint = ssd1306_DrawPixel;
    vlonGui_driver.pFresh = ssd1306_UpdateScreen;

    return &vlonGui_driver;
}