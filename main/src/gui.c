#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"

#include "stdio.h"
#include "vlonGui.h"
#include "vlonGui_window.h"
#include "vlonGui_input.h"
#include "vlonGui_port.h"
#include "vlonGui_menu.h"
#include "vlonGui_input.h"
#include "sleep.h"

#include "gui.h"

#include "rightArrow.h"
#include "leftArrow.h"
#include "btIcon.h"

#define TAG     "GUI"

struct vlonGui_t screen;
struct vlonGui_window_t *mainWin;
MessageBufferHandle_t gui_message;

static uint8_t g_battery_level = 0;
static uint8_t battery_charging = 0;

static void
menuBarDrawCb(struct vlonGui_window_t *win, void *arg)
{
    vlonGui_windowClear(win);
    vlonGui_drawRectangle(win, 110, 1, 12, 7, VLGUI_COLOR_WHITE);
    vlonGui_drawLine(win, 109, 3, 109, 5, 2, VLGUI_COLOR_WHITE);

    for (uint8_t i = 0; i < g_battery_level; i++) {
        vlonGui_drawLine(win, 118 - (i * 3), 3, 
                         118 - (i * 3), 5, 2, VLGUI_COLOR_WHITE);
    }

    for (uint8_t i = 0; i < 3; i++) {
        vlonGui_drawLine(win, 100 + (i * 2), 6 - (i * 2), 
                         100 + (i * 2), 7, 
                         1, VLGUI_COLOR_WHITE);
    }
}

static void 
mainWindowDrawCb(struct vlonGui_window_t *win, void *arg)
{
    vlonGui_windowClear(win);

    vlonGui_drawBitmap(win, 0, 10, 20, 30, leftArrow);
    vlonGui_drawBitmap(win, 108, 10, 20, 30, rightArrow);
    vlonGui_drawBitmap(win, 49, 0, 30, 30, btIcon);

    vlonGui_setFont(vlonGui_wenquan_9pt);
    vlonGui_drawString(win, 40, 35, "À¶ÑÀÉèÖÃ", VLGUI_COLOR_WHITE);
}

int 
mainWindowProcessKeyCb(struct vlonGui_window_t *win, uint8_t key)
{
    struct vlonGui_menu_t *menu;

    switch (key)
    {
    case VLGUI_KEY_OK:
        menu = vlonGui_menuCreate(win, 0, 0, win->win_width, win->win_height);
        printf("menu:%p\n", menu);
        if (!menu) {
            break;
        }
        vlonGui_menuSetFont(menu, vlonGui_wenquan_9pt);
        vlonGui_menuAddEntry(menu, 0, 0, "À¶ÑÀÒôÏì");
        vlonGui_menuAddEntry(menu, 1, 1, "¿ªÆô");
        vlonGui_menuAddEntry(menu, 2, 1, "¹Ø±Õ");
        vlonGui_menuAddEntry(menu, 3, 0, "À¶ÑÀÒ£¿ØÆ÷");
        vlonGui_menuAddEntry(menu, 4, 1, "¿ªÆô");
        vlonGui_menuAddEntry(menu, 5, 1, "¹Ø±Õ");
        printf("done menu\n");
        break;
    case VLGUI_KEY_RIGHT:
        vlonGui_turnOnOff(&screen, 0);
        deep_sleep_enter();
        vlonGui_turnOnOff(&screen, 1);
        break;
    default:
        break;
    }
    return 0;
}

void 
gui_set_battery_level(uint8_t lvl)
{
    if (lvl >= 3) {
        g_battery_level = 3;
    } else {
        g_battery_level = lvl;
    }
}

void 
gui_task_entry(void *arg)
{
    struct vlonGui_window_t *win;
    uint8_t message[GUI_MESSAGE_SIZE_MAX];
    size_t message_len;

    gui_message = xMessageBufferCreate(200);
    if (!gui_message) {
        ESP_LOGE(TAG, "Failed to create message");
        vTaskDelete(NULL);
        return;
    }

    memset(&screen, 0, sizeof(screen));

    vlonGui_inputInit();

    vlonGui_screen_init(&screen, 128, 64);
    vlonGui_register_driver(&screen, vlonGui_portGetDriver());

    mainWin = vlinGui_getMainWindow(&screen);
    vlonGui_windowSetDrawCb(mainWin, menuBarDrawCb);

    win = vlonGui_windowCreate(mainWin, 0, 10, mainWin->win_width, mainWin->win_height, 0);
    vlonGui_windowSetDrawCb(win, mainWindowDrawCb);
    vlonGui_windowSetKeyCb(win, mainWindowProcessKeyCb);

    while (true) {
        vlonGui_refresh();
        message_len = xMessageBufferReceive(gui_message, message, sizeof(message), pdMS_TO_TICKS(20));
        if (!message_len) {
            continue;
        }

        switch (message[0]) {
        case GUI_MESSAGE_BATTERY:
            g_battery_level = message[1];
            battery_charging = message[2];
            vlonGui_windowSetRefresh(mainWin);
            printf("new levle:%d\n", g_battery_level);
            break;
        
        default:
            break;
        }
    }
}