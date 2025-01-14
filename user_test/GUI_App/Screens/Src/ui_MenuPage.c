#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_HomePage.h"
#include "../Inc/ui_MenuPage.h"
#include "../Inc/ui_Megboxes.h"
#include "../Inc/ui_CalendarPage.h"
#include "../Inc/ui_ComputerPage.h"
#include "../Inc/ui_TimerPage.h"
#include "../Inc/ui_NFCCardPage.h"
// #include "ui_SprotsPage.h"
#include "../Inc/ui_HRPage.h"
#include "../Inc/ui_SPO2Page.h"
#include "../Inc/ui_EnvPage.h"
#include "../Inc/ui_CompassPage.h"
#include "../Inc/ui_GameSelectPage.h"
#include "../Inc/ui_SetPage.h"
#include "../Inc/ui_AboutPage.h"
#include <stdio.h>
#include <stdlib.h>
#define MENU_ITEMS_COUNT 12
///////////////////// Page Manager //////////////////
Page_t Page_Menu = {ui_MenuPage_screen_init, ui_MenuPage_screen_deinit, &ui_MenuPage};

///////////////////// VARIABLES ////////////////////
// menu page
lv_obj_t *ui_MenuPage;
// scroll y
int16_t ui_MenuScrollY = 0;
int16_t src_idx = -1;
MenuItem drag_obj;
void static animate_menu_item(lv_obj_t *obj, lv_coord_t start_y, lv_coord_t end_y)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_y, end_y);
    lv_anim_set_time(&a, 300); // 300ms动画时长
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}
///////////////////// FUNCTIONS ////////////////////
/**
 * 处理菜单页面的用户界面事件。
 *
 * 本函数根据事件类型，执行不同的操作。当用户从左向右滑动时，它会触发返回上一页的操作；
 * 当页面滚动结束时，它会更新菜单页面的滚动位置。
 *
 * @param e 指向事件对象的指针，包含事件相关的信息。
 */
void ui_event_MenuPage(lv_event_t *e)
{
    // 获取事件类型
    lv_event_code_t event_code = lv_event_get_code(e);
    // 获取事件目标对象
    lv_obj_t *target = lv_event_get_target(e);
    // 检查事件类型是否为手势
    if (event_code == LV_EVENT_GESTURE)
    {
        // 如果检测到从左向右的手势，则调用Page_Back函数返回上一页
        // if(lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
        // {
        //     Page_Back();
        // }
        if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
        {
            // 等待触摸释放，防止误触发其他事件
            lv_indev_wait_release(lv_indev_get_act());
            Page_Back();
        }
    }
    // 检查事件类型是否为滚动结束
    if (event_code == LV_EVENT_SCROLL_END)
    {
        // 更新菜单页面的滚动位置
        ui_MenuScrollY = lv_obj_get_scroll_y(ui_MenuPage);
    }
}

void ui_event_MenuCalPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if CALENDAR_PAGE_EN
        Page_Load(&Page_Calender);
#else

#endif
    }
}

void ui_event_MenuComPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if COMPUTER_PAGE_EN
        Page_Load(&Page_Computer);
#else

#endif
    }
}

void ui_event_MenuTimPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if TIMER_PAGE_EN
        Page_Load(&Page_Timer);
#else

#endif
    }
}

void ui_event_MenuCardPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if CARD_PAGE_EN
        Page_Load(&Page_NFCCard);
#else

#endif
    }
}

u_int16_t found_overlap_ = 0;

static void drag_event_handler_(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    static struct
    {
        lv_obj_t *current_hover_obj;
        lv_coord_t original_y;
        uint32_t hover_start_time;
    } drag_state = {0};

    // 常量定义
    enum
    {
        ITEM_WIDTH = 240,
        ITEM_HEIGHT = 70,
        SWAP_THRESHOLD = ITEM_HEIGHT / 3,
        SCROLL_THRESHOLD = 10,
        HOVER_DELAY_MS = 40 * 10, // 2秒延迟
    };

    const float ENLARGE_FACTOR = 1.05f;
    const float SHRINK_FACTOR = 0.95f;
    const uint16_t SCREEN_HEIGHT = lv_obj_get_height(ui_MenuPage);

    switch (code)
    {
    case LV_EVENT_LONG_PRESSED:
    {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        lv_obj_add_state(obj, LV_STATE_PRESSED);
        drag_state.original_y = lv_obj_get_y(obj);
        lv_obj_move_foreground(obj);
        lv_obj_clear_flag(ui_MenuPage, LV_OBJ_FLAG_SCROLLABLE);

        // 调整所有对象大小
        uint32_t child_count = lv_obj_get_child_cnt(ui_MenuPage);
        for (uint32_t i = 0; i < child_count; i++)
        {
            lv_obj_t *child = lv_obj_get_child(ui_MenuPage, i);
            if (child == obj)
            {
                lv_obj_set_size(child, ITEM_WIDTH * ENLARGE_FACTOR, ITEM_HEIGHT * ENLARGE_FACTOR);
            }
            else
            {
                lv_obj_set_size(child, ITEM_WIDTH * SHRINK_FACTOR, ITEM_HEIGHT * SHRINK_FACTOR);
            }
        }
        for (uint32_t i = 0; i < child_count; i++)
        {
            if (menu_items[i].panel == obj)
            {
                src_idx = i;
                drag_obj = menu_items[i];
            }
        }
    }
    break;

    case LV_EVENT_PRESSING:
    {
        if (lv_obj_has_flag(obj, LV_OBJ_FLAG_PRESS_LOCK))
            break;

        lv_indev_t *indev = lv_indev_get_act();
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        // 更新位置
        lv_coord_t new_y = lv_obj_get_y(obj) + vect.y;
        lv_coord_t parent_y = lv_obj_get_scroll_y(ui_MenuPage);
        lv_obj_set_y(obj, new_y);
        // 处理滚动
        if (new_y < parent_y + SCROLL_THRESHOLD)
        {
            // 保存当前相对位置
            lv_coord_t rel_y = new_y - parent_y;

            // 执行滚动
            if (0 <= parent_y - 10)
            {
                lv_obj_scroll_by(ui_MenuPage, 0, 10, LV_ANIM_OFF);

                // 更新对象位置，保持相对位置不变
                new_y = lv_obj_get_scroll_y(ui_MenuPage) + rel_y;
                lv_obj_set_y(obj, new_y);
            }
        }
        if (new_y > parent_y + SCREEN_HEIGHT - 70 - SCROLL_THRESHOLD)
        {
            // 保存当前相对位置
            lv_coord_t rel_y = new_y - parent_y;

            // 执行滚动
            if (parent_y + 10 <= (lv_obj_get_child_cnt(ui_MenuPage) - 4) * 70)
            {
                lv_obj_scroll_by(ui_MenuPage, 0, -10, LV_ANIM_OFF);

                // 更新对象位置，保持相对位置不变
                new_y = lv_obj_get_scroll_y(ui_MenuPage) + rel_y;
                lv_obj_set_y(obj, new_y);
            }
        }

        // 检测悬停对象
        uint32_t child_count = lv_obj_get_child_cnt(ui_MenuPage);
        lv_obj_t *hover_candidate = NULL;
        int obj_center = new_y + (ITEM_HEIGHT / 2);

        for (uint32_t i = 0; i < child_count; i++)
        {
            lv_obj_t *child = lv_obj_get_child(ui_MenuPage, i);
            if (child == obj)
            {
                continue;
            }
            else
            {
                int child_center = lv_obj_get_y(child) + (ITEM_HEIGHT / 2);
                if (abs(obj_center - child_center) < SWAP_THRESHOLD)
                {
                    hover_candidate = child;
                    break;
                }
            }
        }

        // 处理悬停和交换
        // hover_candidate：当前鼠标悬停的对象，current_hover_obj：当前记录的悬停对象
        if (hover_candidate != drag_state.current_hover_obj)
        {
            drag_state.current_hover_obj = hover_candidate;
            drag_state.hover_start_time = lv_tick_get();
        }
        else if (hover_candidate &&
                 hover_candidate == drag_state.current_hover_obj)
        {
            if (lv_tick_get() - drag_state.hover_start_time >= HOVER_DELAY_MS)
            {

                int16_t dst_idx = -1;

                // 找到源对象和目标对象的索引
                for (int i = 0; i < MENU_ITEMS_COUNT; i++)
                {
                    if (menu_items[i].panel == hover_candidate)
                    {
                        dst_idx = i;
                    }
                }

                // 确保找到了两个有效的索引
                if (src_idx == -1 || dst_idx == -1)
                {
                    return;
                }

                //  = menu_items[src_idx];

                // 向上移动
                if (src_idx > dst_idx)
                {

                    for (int i = src_idx; i > dst_idx; i--)
                    {
                        menu_items[i] = menu_items[i - 1];
                        animate_menu_item(menu_items[i - 1].panel, lv_obj_get_y(menu_items[i - 1].panel), lv_obj_get_y(menu_items[i - 1].panel) + ITEM_HEIGHT);
                    }
                    menu_items[dst_idx] = drag_obj;
                    src_idx = dst_idx;
                }
                // 向下移动
                else if (src_idx < dst_idx)
                {

                    for (int i = src_idx; i < dst_idx; i++)
                    {
                        menu_items[i] = menu_items[i + 1];
                        animate_menu_item(menu_items[i + 1].panel, lv_obj_get_y(menu_items[i + 1].panel), lv_obj_get_y(menu_items[i + 1].panel) - ITEM_HEIGHT);
                    }
                    menu_items[dst_idx] = drag_obj;
                    src_idx = dst_idx;
                }

                drag_state.hover_start_time = lv_tick_get();
            }
        }
    }
    break;

    case LV_EVENT_RELEASED:
    {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
        lv_obj_clear_state(obj, LV_STATE_PRESSED);
        lv_obj_add_flag(ui_MenuPage, LV_OBJ_FLAG_SCROLLABLE);

        // 恢复大小并收集对象
        for (uint32_t i = 0; i < MENU_ITEMS_COUNT; i++)
        {
            if (menu_items[i].panel != NULL)
            {
                if (menu_items[i].panel == drag_obj.panel)
                {
                    lv_anim_t a;
                    lv_anim_init(&a);
                    lv_anim_set_var(&a, menu_items[i].panel);
                    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
                    lv_anim_set_values(&a, lv_obj_get_y(menu_items[i].panel), i * ITEM_HEIGHT);
                    lv_anim_set_time(&a, 300);
                    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
                    lv_anim_start(&a);
                    lv_obj_set_x(menu_items[i].panel, 0);
                    lv_obj_move_to_index(menu_items[i].panel, i);
                }
                lv_obj_set_size(menu_items[i].panel, ITEM_WIDTH, ITEM_HEIGHT);
            }
        }
        // 重置状态
        memset(&drag_state, 0, sizeof(drag_state));
    }
    break;
    }
}

void ui_event_MenuSprPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if SPROTS_PAGE_EN

#else

#endif
    }
}

void ui_event_MenuHRPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if HR_PAGE_EN
        Page_Load(&Page_HR);
        // send the sensor wakeup message

#else

#endif
    }
}

void ui_event_MenuO2Panel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if O2_PAGE_EN
        Page_Load(&Page_SPO2);
#else

#endif
    }
}

void ui_event_MenuEnvPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if ENVIRONMENT_PAGE_EN
        Page_Load(&Page_Env);
#else

#endif
    }
}

void ui_event_MenuCPPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if COMPASS_PAGE_EN
        Page_Load(&Page_Compass);
#else

#endif
    }
}

void ui_event_MenuGamePanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if GAME_PAGE_EN
        Page_Load(&Page_GameSelect);
#else

#endif
    }
}

void ui_event_MenuSetPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if SET_PAGE_EN
        Page_Load(&Page_Set);
#else

#endif
    }
}

void ui_event_MenuAbPanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_SHORT_CLICKED)
    {
#if ABOUT_PAGE_EN
        Page_Load(&Page_About);
#else

#endif
    }
}

// 初始化菜单项
void init_menu_item(MenuItem *item, lv_obj_t *parent, int y_offset)
{
    // 创建面板
    item->panel = lv_obj_create(parent);
    lv_obj_set_size(item->panel, 240, 70);
    lv_obj_set_pos(item->panel, 0, y_offset);
    lv_obj_clear_flag(item->panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(item->panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item->panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(item->panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(item->panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(item->panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item->panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(item->panel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    // 为面板添加长按事件回调
    lv_obj_add_event_cb(item->panel, drag_event_handler_, LV_EVENT_LONG_PRESSED, NULL);
    lv_obj_add_event_cb(item->panel, drag_event_handler_, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(item->panel, drag_event_handler_, LV_EVENT_RELEASED, NULL);

    // 创建按钮
    item->button = lv_btn_create(item->panel);
    lv_obj_set_size(item->button, 40, 40);
    lv_obj_set_align(item->button, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(item->button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(item->button, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(item->button, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item->button, item->button_color, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(item->button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建图标
    item->icon = lv_label_create(item->button);
    lv_obj_set_size(item->icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(item->icon, LV_ALIGN_CENTER);
    lv_label_set_text(item->icon, item->icon_text);
    lv_obj_set_style_text_font(item->icon, item->icon_font, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建标签
    item->label = lv_label_create(item->panel);
    lv_obj_set_size(item->label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_pos(item->label, 60, 0);
    lv_obj_set_align(item->label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(item->label, item->label_text);
    lv_obj_set_style_text_font(item->label, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 添加点击事件回调
    if (item->event_cb)
    {
        lv_obj_add_event_cb(item->panel, item->event_cb, LV_EVENT_SHORT_CLICKED, NULL);
    }
}

// 定义菜单项数组
MenuItem menu_items[] = {
    {.button_color = {0xFF, 0x80, 0x80}, // 直接使用RGB值
     .icon_text = "",
     .label_text = "日 历",
     .icon_font = &ui_font_iconfont30,
     .event_cb = ui_event_MenuCalPanel},
    {.button_color = {0xC8, 0x64, 0x00},
     .icon_text = "",
     .label_text = "计 算 器",
     .icon_font = &ui_font_iconfont28,
     .event_cb = ui_event_MenuComPanel},
    {.button_color = {0xDC, 0x80, 0xE6},
     .icon_text = "",
     .label_text = "秒 表",
     .icon_font = &ui_font_iconfont34,
     .event_cb = ui_event_MenuTimPanel},
    {.button_color = {0x80, 0x64, 0x1E},
     .icon_text = "",
     .label_text = "卡 包",
     .icon_font = &ui_font_iconfont32,
     .event_cb = ui_event_MenuCardPanel},
    {.button_color = {0x00, 0x96, 0x80},
     .icon_text = "",
     .label_text = "运 动",
     .icon_font = &ui_font_iconfont30,
     .event_cb = ui_event_MenuSprPanel},
    {.button_color = {0xC8, 0x00, 0x00},
     .icon_text = "",
     .label_text = "心 率",
     .icon_font = &ui_font_iconfont34,
     .event_cb = ui_event_MenuHRPanel},
    {.button_color = {0x00, 0x80, 0xFF},
     .icon_text = "",
     .label_text = "血 氧",
     .icon_font = &ui_font_iconfont34,
     .event_cb = ui_event_MenuO2Panel},
    {.button_color = {0x00, 0x96, 0x32},
     .icon_text = "",
     .label_text = "环 境",
     .icon_font = &ui_font_iconfont34,
     .event_cb = ui_event_MenuEnvPanel},
    {.button_color = {0x80, 0x00, 0x80},
     .icon_text = "",
     .label_text = "指 南 针",
     .icon_font = &ui_font_iconfont34,
     .event_cb = ui_event_MenuCPPanel},
    {.button_color = {0xC0, 0x70, 0x10},
     .icon_text = "",
     .label_text = "游 戏",
     .icon_font = &ui_font_iconfont30,
     .event_cb = ui_event_MenuGamePanel},
    {.button_color = {0x80, 0x80, 0x80},
     .icon_text = "",
     .label_text = "设 置",
     .icon_font = &ui_font_iconfont30,
     .event_cb = ui_event_MenuSetPanel},
    {.button_color = {0x64, 0x64, 0x64},
     .icon_text = "",
     .label_text = "关 于",
     .icon_font = &ui_font_iconfont30,
     .event_cb = ui_event_MenuAbPanel}};

///////////////////// SCREEN init ////////////////////
void ui_MenuPage_screen_init(void)
{
    ui_MenuPage = lv_obj_create(NULL);
    // lv_obj_clear_flag(ui_MenuPage, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(ui_MenuPage, LV_OBJ_FLAG_EVENT_BUBBLE);
    // lv_scr_load_anim(ui_MenuPage, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);

    // 初始化所有菜单项
    for (int i = 0; i < MENU_ITEMS_COUNT; i++)
    {
        init_menu_item(&menu_items[i], ui_MenuPage, i * 70); // 动态计算 y_offset
    }

    // scroll back
    lv_obj_scroll_to(ui_MenuPage, 0, ui_MenuScrollY, LV_ANIM_OFF);
    lv_obj_clear_flag(ui_MenuPage, LV_OBJ_FLAG_SCROLL_CHAIN);

    // events
    lv_obj_add_event_cb(ui_MenuPage, ui_event_MenuPage, LV_EVENT_ALL, NULL);
}

/////////////////// SCREEN deinit ////////////////////
void ui_MenuPage_screen_deinit(void)
{
}
