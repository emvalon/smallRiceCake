#ifndef _GUI_H_
#define _GUI_H_


#include "freertos/FreeRTOS.h"
#include "freertos/message_buffer.h"

#define GUI_MESSAGE_SIZE_MAX        (20)

#define GUI_MESSAGE_BATTERY         (1)

extern MessageBufferHandle_t gui_message;

void  gui_set_battery_level(uint8_t lvl);

void gui_task_entry(void *arg);

#endif // _GUI_H_