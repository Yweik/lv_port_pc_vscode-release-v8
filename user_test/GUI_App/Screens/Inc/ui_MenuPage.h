#ifndef _UI_MENUPAGE_H
#define _UI_MENUPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

#define CALENDAR_PAGE_EN		1
#define COMPUTER_PAGE_EN		1
#define TIMER_PAGE_EN			1
#define CARD_PAGE_EN            1
#define SPROTS_PAGE_EN			0
#define HR_PAGE_EN				1
#define O2_PAGE_EN				1
#define ENVIRONMENT_PAGE_EN	    1
#define COMPASS_PAGE_EN			1
#define GAME_PAGE_EN            1
#define SET_PAGE_EN				1
#define ABOUT_PAGE_EN			1
// 定义菜单项结构体
typedef struct {
    lv_obj_t* panel;
    lv_obj_t* button;
    lv_obj_t* icon;
    lv_obj_t* label;
    lv_color_t button_color;
    const char* icon_text;
    const char* label_text;
    lv_font_t *icon_font;
    lv_event_cb_t event_cb;
} MenuItem;
extern lv_obj_t * ui_MenuPage;

extern int16_t ui_MenuScrollY;

extern Page_t Page_Menu;
extern MenuItem menu_items[];
void ui_MenuPage_screen_init(void);
void ui_MenuPage_screen_deinit(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
