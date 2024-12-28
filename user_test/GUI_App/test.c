#include "test.h"
#include <stdlib.h> 
int icon_width = 0;
int icon_height = 0;
lv_obj_t *ui_Screen1;
static void released_event_handler(lv_event_t * e);

static void released_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_indev_t * indev = lv_indev_get_act();
    int child_count = lv_obj_get_child_cnt(ui_Screen1);
    int offset_x = 0;
    int offset_y = 0;
    int i = 0;
    for(i=0;i<child_count;i++)
    {
        lv_obj_t *child_obj = lv_obj_get_child(ui_Screen1,i);
        if(child_obj == obj)  continue;
        lv_coord_t x = lv_obj_get_x(child_obj);
        lv_coord_t y = lv_obj_get_y(child_obj);
        if(x != offset_x) break;
        offset_x += icon_width; 
        if(offset_x == DISPLAY_WIDTH) //todo >=?
        {
            offset_x = 0;
            offset_y += icon_height; 
        }
    }
    lv_obj_move_foreground(obj);
    lv_obj_set_pos(obj,offset_x, offset_y);
    lv_obj_move_to_index(obj,i);
} 

int found_overlap = 0; // 假设没有重叠
void drag_event_handler(lv_event_t *e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t vect;
    int i,j = 0;
    lv_indev_get_vect(indev, &vect);
    if((vect.x) || (vect.y)) 
    {
        lv_point_t p;
        found_overlap = 0;
        lv_indev_get_point(indev,&p);
        int x = lv_obj_get_x(obj)+icon_width;
        int y = lv_obj_get_y(obj)+(icon_height/2);
        int child_count = lv_obj_get_child_cnt(ui_Screen1);
        for(i = 0;i<child_count;i++)
        {
            lv_obj_t *child_obj = lv_obj_get_child(ui_Screen1,i);
            if(child_obj == obj)  continue;
            
                int x1 = lv_obj_get_x(child_obj);
                int y1 = lv_obj_get_y(child_obj);
                int x2 = x1 + icon_width;
                int y2 = y1 + icon_height;
                if((x1 < x && x < x2 && y1 < y && y < y2))
                {
                    found_overlap = 1;
                    break;
                }
        }
        if(!found_overlap)
        {
            lv_obj_move_foreground(obj);
            lv_obj_set_pos(obj, lv_obj_get_x(obj) + vect.x, lv_obj_get_y(obj) + vect.y);
            return;
        }
        int offset_x = 0;
        int offset_y = 0;
        for(j=0;j<child_count;j++)
        {
            lv_obj_t *child = lv_obj_get_child(ui_Screen1,j);
            if (offset_x == DISPLAY_WIDTH)//todo >=?
            {
                offset_x = 0;
                offset_y += icon_height;
            }
            if(i == j) 
            {
                offset_x += icon_width;
            }
            if (offset_x == DISPLAY_WIDTH)//todo >=?
            {
                offset_x = 0;
                offset_y += icon_height;
            }
            if (child != obj)
            {
                lv_obj_set_pos(child, offset_x, offset_y);
                lv_obj_move_to_index(child,j);
            }
            offset_x += icon_width; 
        }
        lv_obj_move_foreground(obj);
        lv_obj_set_pos(obj, lv_obj_get_x(obj) + vect.x, lv_obj_get_y(obj) + vect.y); 
    }
}

int get_icon_size(int min_size, int max_size, int ideal_size, int total_size) {
    int sizes[100]; 
    int sizes_count = 0; 

    for (int i = min_size; i <= max_size; i++) {
        if (total_size % i != 0) {
            continue;
        }
        sizes_count++;
        sizes[sizes_count - 1] = i;
    }

    int found_size = total_size;

    for (int i = 0; i < sizes_count; i++) {
        int diff1 = abs(sizes[i] - ideal_size);
        int diff2 = abs(found_size - ideal_size);

        if (diff1 < diff2) {
            found_size = sizes[i];
        }
    }

    

    return found_size;
}
void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0x0B0101), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_obj_set_style_bg_opa(ui_Screen1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_Screen1,0,0);
    
    int offset_x = 0;
    int offset_y = 0;

    icon_width = 240;
    icon_height = 70;
    int icon_count = (DISPLAY_WIDTH / icon_width) * (DISPLAY_HEIGHT / icon_height);
    for(int i=0;i<icon_count;i++)
    {
        lv_obj_t *obj = lv_obj_create(ui_Screen1);
        lv_obj_set_size(obj,icon_width,icon_height);
        if (offset_x == DISPLAY_WIDTH)
        {
            offset_x = 0;
            offset_y += icon_height;
        }
        lv_obj_set_pos(obj,offset_x,offset_y);
        lv_obj_t *btn = lv_btn_create(obj);
        lv_obj_set_size(btn, 50, 50); // 设置按钮的大小
        lv_obj_center(btn); // 将按钮居中于 obj
        // obj滚动禁止使用
        lv_obj_set_scroll_dir(obj, LV_DIR_NONE);

        lv_obj_t *label = lv_label_create(obj);
        lv_label_set_text_fmt(label,"%d",i);
        lv_obj_center(label);

        lv_obj_add_event_cb(obj, drag_event_handler, LV_EVENT_PRESSING, NULL);
        lv_obj_add_event_cb(obj, released_event_handler, LV_EVENT_RELEASED, NULL);

        offset_x += icon_width;
    }
    lv_disp_load_scr(ui_Screen1);
}