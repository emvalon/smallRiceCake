/**
 * @file ui_home.c
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-07
 * 
 * Copyright 2021 - 2023 Weilong Shen (valonshen@foxmail.com)
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
#include <stdio.h>
#include "vlGui.h"
#include "vlGui_clock.h"
#include "ui.h"

#define UI_HOME_BLESSING_WIDTH      (58)

static const uint16_t str_blessing1_cn[] = {0xc6bd, 0xb0b2, 0xcfb2, 0xC0D6, 0};
static const uint16_t str_blessing2_cn[] = {0xD0D2, 0xB8A3, 0xCFE0, 0xCBE6, 0};

static void 
ui_homeDrawCb(vlGui_window_t *win, uint8_t flag)
{
    int16_t x0,y0, lineX;
    const struct vlGui_font_t *font;

    if (flag != VLGUI_WIN_DRAW_INIT) {    
        return;
    }
    font = vlGui_wenquan_9pt;
    vlGui_windowClear(win);

    lineX = win->win_width - UI_HOME_BLESSING_WIDTH;
    x0 = VLGUI_STR_CENTER_X(UI_HOME_BLESSING_WIDTH - 2, font, 4);
    x0 = win->win_width - UI_HOME_BLESSING_WIDTH + x0 + 2;
    y0 = VLGUI_STR_CENTER_Y(win->win_height, font, 2) - 3;

    vlGui_setFont(font);
    vlGui_drawLine(win, lineX, y0 - 2, lineX, y0 + (2 * font->fontHeight) + 2,
                   2, VLGUI_COLOR_WHITE);
    vlGui_drawString(win, x0, y0, (char *)str_blessing1_cn, VLGUI_COLOR_WHITE);
    vlGui_drawString(win, x0, y0 + font->fontHeight + 2, 
                     (char *)str_blessing2_cn, VLGUI_COLOR_WHITE);
}

static int 
ui_homeProcessKeyCb(vlGui_window_t *win, uint8_t key)
{
    VLGUI_UNUSED(win);
    VLGUI_UNUSED(key);

    switch (key)
    {
    case VLGUI_KEY_OK:
        ui_menuWinCreate(win, 0, 0, win->win_width, win->win_height);
        break;
    
    default:
        break;
    }

    return 0;
}

void
ui_homeWinCreate(vlGui_window_t *parent, int16_t x, int16_t y, int16_t width, 
                 int16_t height)
{
    vlGui_window_t *win;
    struct vlGui_clock_t *clk;
    struct vlGui_clock_time_t time;

    win = vlGui_windowCreate(parent, x, y, width, height, 0);
    vlGui_windowSetDrawCb(win, ui_homeDrawCb);
    vlGui_windowSetKeyCb(win, ui_homeProcessKeyCb);

    clk = vlGui_clockCreate(parent, x, y, width - UI_HOME_BLESSING_WIDTH, 
                            height);

    time.hour = 12;
    time.min = 10;
    time.sec = 56;
    vlGui_clockSetTime(clk, &time);
    vlGui_windowBlurEnable(&clk->win, false);
}