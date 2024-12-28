#ifndef TEST_H
#define TEST_H

#include "lvgl/lvgl.h"

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 280

extern int icon_width;
extern int icon_height;
extern lv_obj_t *ui_Screen1;

void drag_event_handler(lv_event_t *e);
int get_icon_size(int min_size, int max_size, int ideal_size, int total_size);
void ui_Screen1_screen_init(void);

#endif // UI_H



