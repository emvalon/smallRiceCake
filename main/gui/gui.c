#include <string.h>
#include <stdio.h>
/* The header files from ESP IDF */
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
/* The header files from vlGui */
#include "vlGui.h"
#include "vlGui_input.h"
/* The header files from Gui Application */
#include "gui.h"
#include "ui.h"

#define TAG                     "GUI"
#define GUI_STATUS_BAR_H        (10)

struct vlGui_t screen;
MessageBufferHandle_t gui_message;

static uint8_t g_battery_level = 0;
static uint8_t battery_charging = 0;

static void
menuBarDrawCb(vlGui_window_t *win, void *arg)
{
    vlGui_drawBlock(win, 0, 0, win->win_width, GUI_STATUS_BAR_H, 
                    VLGUI_COLOR_BLACK);
    vlGui_drawRectangle(win, 110, 1, 12, 7, VLGUI_COLOR_WHITE);
    vlGui_drawLine(win, 109, 3, 109, 5, 2, VLGUI_COLOR_WHITE);

    for (uint8_t i = 0; i < g_battery_level; i++) {
        vlGui_drawLine(win, 118 - (i * 3), 3, 
                         118 - (i * 3), 5, 2, VLGUI_COLOR_WHITE);
    }

    for (uint8_t i = 0; i < 3; i++) {
        vlGui_drawLine(win, 100 + (i * 2), 6 - (i * 2), 
                         100 + (i * 2), 7, 
                         1, VLGUI_COLOR_WHITE);
    }
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
    vlGui_window_t *mainWin;
    uint8_t message[GUI_MESSAGE_SIZE_MAX];
    size_t message_len;

    gui_message = xMessageBufferCreate(200);
    if (!gui_message) {
        ESP_LOGE(TAG, "Failed to create message");
        vTaskDelete(NULL);
        return;
    }

    memset(&screen, 0, sizeof(screen));
    vlGui_inputInit();
    vlGui_screen_init(&screen, 128, 64);
    vlGui_register_driver(&screen, vlGui_portGetDriver());

    mainWin = vlinGui_getMainWindow(&screen);
    vlGui_windowSetDrawCb(mainWin, menuBarDrawCb);
    ui_homeWinCreate(mainWin, 0, GUI_STATUS_BAR_H, mainWin->win_width, 
                     mainWin->win_height - GUI_STATUS_BAR_H);

    while (true) {
        vlGui_refresh();
        message_len = xMessageBufferReceive(gui_message, message, 
                                            sizeof(message), pdMS_TO_TICKS(20));
        if (!message_len) {
            continue;
        }

        switch (message[0]) {
        case GUI_MESSAGE_BATTERY:
            g_battery_level = message[1];
            battery_charging = message[2];
            vlGui_windowSetRefresh(mainWin);
            break;
        
        default:
            break;
        }
    }
}