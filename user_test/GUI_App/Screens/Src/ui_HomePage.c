#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_HomePage.h"
#include "../Inc/ui_MenuPage.h"
#include "../Inc/ui_HRPage.h"
#include "../Inc/ui_SetPage.h"

#include "../../../Func/Inc/HWDataAccess.h"


///////////////////// VARIABLES ////////////////////
//home page
lv_obj_t * ui_HomePage;
lv_obj_t * ui_TimeHourLabel;
lv_obj_t * ui_TimeColonLabel;
lv_obj_t * ui_TimeMinuteLabel;
lv_obj_t * ui_BatArc;
lv_obj_t * ui_BaticonLabel;
lv_obj_t * ui_BatNumLabel;
lv_obj_t * ui_DateLabel;
lv_obj_t * ui_DayLabel;
lv_obj_t * ui_StepiconLabel;
lv_obj_t * ui_StepCnLabel;
lv_obj_t * ui_StepNumLabel;
lv_obj_t * ui_StepNumBar;
lv_obj_t * ui_TempArc;
lv_obj_t * ui_TempiconLabel;
lv_obj_t * ui_TempNumLabel;
lv_obj_t * ui_HumiiconLabel;
lv_obj_t * ui_HumiArc;
lv_obj_t * ui_HumiNumLabel;
lv_obj_t * ui_HRArc;
lv_obj_t * ui_HRiconLabel;
lv_obj_t * ui_HRNumLabel;
//drop down: 显示在底部的菜单栏
lv_obj_t * ui_DropDownPanel;
lv_obj_t * ui_UpBGPanel;
lv_obj_t * ui_NFCButton;
lv_obj_t * ui_NFCLabel;
lv_obj_t * ui_BLEButton;
lv_obj_t * ui_BLELabel;
lv_obj_t * ui_PowerButton;
lv_obj_t * ui_PowerLabel;
lv_obj_t * ui_SetButton;
lv_obj_t * ui_SetLabel;
lv_obj_t * ui_LightSlider;
lv_obj_t * ui_LightLabel;
lv_obj_t * ui_DownBGPanel;
//power slider
lv_obj_t * ui_PowerPage;
lv_obj_t * ui_PowerSlider;
lv_obj_t * ui_PowerDownLabel;

lv_timer_t * ui_HomePageTimer;

// variables
uint8_t ui_TimeHourValue = 11;
uint8_t ui_TimeMinuteValue = 59;
const char * ui_Days[7] = {"Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.", "Sun."};
uint8_t ui_DateMonthValue = 11;
uint8_t ui_DateDayValue = 8;
uint8_t ui_DataWeekdayValue = 2;

uint8_t ui_BatArcValue = 70;
uint16_t ui_StepNumValue = 0;
uint8_t ui_LightSliderValue = 50;

uint8_t ui_HomePageBLEEN = 0;
uint8_t ui_HomePageNFCEN = 0;

int8_t ui_HomePageTempValue = 25;
int8_t ui_HomePageHumiValue = 67;

static u_int8_t flag = 126;

///////////////////// Page Manager //////////////////
// Page_t 结构体，用于轻松管理页面的初始化销毁以及对象调用
Page_t Page_Home = {ui_HomePage_screen_init, ui_HomePage_screen_deinit, &ui_HomePage};
Page_t Page_Power = {ui_PowerPage_screen_init, ui_PowerPage_screen_deinit, &ui_PowerPage};

/////////////////// private timer ///////////////////
// need to be destroyed when the page is destroyed
// 主页展示数据定时更新
static void HomePage_timer_cb(lv_timer_t * timer)
{
  uint8_t value_strbuf[10];// 用于临时存储格式化后的字符串值。例如，在更新时间、日期、电池电量等信息
  if(Page_Get_NowPage()->page_obj  == &ui_HomePage)
	{
			/*
			lv_obj_set_style_text_opa(ui_TimeColonLabel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
			osDelay(500);
			lv_obj_set_style_text_opa(ui_TimeColonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
			*/
      // 获取当前的日期和时间
			HW_DateTimeTypeDef DateTime;
      HWInterface.RealTimeClock.GetTimeDate(&DateTime);

      // 校对分钟
			if(ui_TimeMinuteValue != DateTime.Minutes)
			{
				ui_TimeMinuteValue = DateTime.Minutes;
				sprintf(value_strbuf,"%02d",ui_TimeMinuteValue);
				lv_label_set_text(ui_TimeMinuteLabel, value_strbuf);
			}

      // 校对小时
			if(ui_TimeHourValue != DateTime.Hours)
			{
				ui_TimeHourValue = DateTime.Hours;
				sprintf(value_strbuf,"%2d",ui_TimeHourValue);
				lv_label_set_text(ui_TimeHourLabel, value_strbuf);
			}

      // 校对日期
			if(ui_DateDayValue != DateTime.Date)
			{
				ui_DateDayValue = DateTime.Date;
				ui_DataWeekdayValue = DateTime.WeekDay;
				sprintf(value_strbuf,"%2d-%02d",ui_DateMonthValue,ui_DateDayValue);
				lv_label_set_text(ui_DateLabel, value_strbuf);
				lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue-1]);

			}

      // 校对月份
			if(ui_DateMonthValue != DateTime.Month)
			{
				ui_DateMonthValue = DateTime.Month;
				ui_DateDayValue = DateTime.Date;
				ui_DataWeekdayValue = DateTime.WeekDay;
				sprintf(value_strbuf,"%2d-%02d",ui_DateMonthValue,ui_DateDayValue);
				lv_label_set_text(ui_DateLabel, value_strbuf);
				lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue-1]);
			}

      // 校对电池电量
      if(ui_BatArcValue != HWInterface.Power.power_remain)
      {
        ui_BatArcValue = HWInterface.Power.power_remain;
        lv_arc_set_value(ui_BatArc, ui_BatArcValue);
				sprintf(value_strbuf,"%2d%%",ui_BatArcValue);
				lv_label_set_text(ui_BatNumLabel, value_strbuf);
      }

      // 校对步数
      if(ui_StepNumValue != HWInterface.IMU.Steps)
      {
        ui_StepNumValue = HWInterface.IMU.Steps;
        sprintf(value_strbuf,"%d",ui_StepNumValue);
				lv_label_set_text(ui_StepNumLabel, value_strbuf);
      }
      // 校对温度
      if( ui_HomePageTempValue != (int8_t)HWInterface.AHT21.temperature )
      {
        ui_HomePageTempValue = (int8_t)HWInterface.AHT21.temperature;
        lv_arc_set_value(ui_TempArc, ui_HomePageTempValue);
        sprintf(value_strbuf,"%d",ui_HomePageTempValue);
				lv_label_set_text(ui_TempNumLabel, value_strbuf);
      }
      // 校对湿度
      if( ui_HomePageHumiValue != (int8_t)HWInterface.AHT21.humidity )
      {
        ui_HomePageHumiValue = (int8_t)HWInterface.AHT21.humidity;
        lv_arc_set_value(ui_HumiArc, ui_HomePageHumiValue);
				sprintf(value_strbuf,"%d",ui_HomePageHumiValue);
				lv_label_set_text(ui_HumiNumLabel, value_strbuf);
      }

	}
}


///////////////////// FUNCTIONS ////////////////////

/**
 * 处理首页的UI事件
 * 
 * 本函数旨在响应和处理由LVGL库触发的UI事件，特备是手势事件
 * 当检测到从左向右的手势时，将加载菜单页面
 * 
 * @param e 指向lv_event_t结构体的指针，包含事件的相关信息
 */


void ui_event_HomePage(lv_event_t * e)
{
    // 获取事件类型
    lv_event_code_t event_code = lv_event_get_code(e);
    // 获取事件的目标对象
    lv_obj_t * target = lv_event_get_target(e);

    // 检查事件类型是否为手势事件
    if(event_code == LV_EVENT_GESTURE)
    {
        // 检查手势方向是否为从左到右
        if(lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT)
        {
            // 当检测到右手势时，加载菜单页面
            
            Page_Load(&Page_Menu);
        }
    }
}

/**
 * 处理NFC按钮的UI事件
 * 
 * @param e 指向事件对象的指针，包含事件相关的信息
 * 
 * 此函数根据NFC按钮的状态来更新UI和执行相应的操作当按钮的状态发生变化时，
 * 即触发LV_EVENT_VALUE_CHANGED事件，函数会检查按钮是否处于选中状态，并根据状态
 * 更新ui_HomePageNFCEN标志位这个标志位可以用于控制NFC功能的启用或禁用
 */
void ui_event_NFCButton(lv_event_t * e)
{
    // 获取事件的类型
    lv_event_code_t event_code = lv_event_get_code(e);
    
    // 获取事件的目标对象，即触发事件的UI元素
    lv_obj_t * target = lv_event_get_target(e);
    
    // 检查事件类型是否为值改变且目标对象处于选中状态
    if(event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        // 当按钮被选中时
        ui_HomePageNFCEN=1;
        //ICcard_Select(1);
    }
    
    // 检查事件类型是否为值改变且目标对象不处于选中状态
    if(event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        // 当按钮未被选中时
        ui_HomePageNFCEN=0;
        //ICcard_Select(0);
    }
}

void ui_event_BLEButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        //checked
        HWInterface.BLE.Enable();
        ui_HomePageBLEEN=1;

    }
    if(event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        //released
				HWInterface.BLE.Disable();
        ui_HomePageBLEEN=0;
    }
}

void ui_event_PowerButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
			//power slider
      Page_Load(&Page_Power);
			// ui_PowerPage_screen_init();
			// lv_scr_load_anim(ui_PowerPage,LV_SCR_LOAD_ANIM_MOVE_RIGHT,0,0,true);
			// user_Stack_Push(&ScrRenewStack,(long long int)&ui_PowerPage);
    }
}

void ui_event_PowerSlider(lv_event_t * e)
{
		lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED)
    {

    }
		if(event_code == LV_EVENT_CLICKED)
		{
			//power down if slider value >= 90
			if(lv_slider_get_value(ui_PowerSlider) >=90)
			{
        HWInterface.Power.Shutdown();
			}
		}
}

void ui_event_SetButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
			Page_Load(&Page_Set);
    }
}

void ui_event_LightSlider(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED)
    {
        ui_LightSliderValue = lv_slider_get_value(ui_LightSlider);
        HWInterface.LCD.SetLight(ui_LightSliderValue);
    }
}


static void scroll_event_cb(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e); // 获取事件目标对象
    lv_coord_t y = lv_obj_get_scroll_y(obj); // 获取Y轴的滚动值

    // 判断是否为 RELEASED 事件
    if (lv_event_get_code(e) == LV_EVENT_SCROLL_END) {
        if ((flag==126 & y-flag < -20)) {
            lv_obj_scroll_to_y(ui_DropDownPanel,0, LV_ANIM_ON); // 滚动到顶部
            flag = 0;
        } else if ((flag==0 & y-flag > 20)) {
            lv_obj_scroll_to_y(ui_DropDownPanel,126, LV_ANIM_ON); // 滚动到指定位置
            flag = 126;
        }
        else{
          lv_obj_scroll_to_y(ui_DropDownPanel,flag, LV_ANIM_OFF);
        }
    }
}

///////////////////// SCREEN init ////////////////////
void ui_HomePage_screen_init(void)
{
		// ui_MenuScrollY = 0;
    uint8_t value_strbuf[10];

    ui_HomePage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_HomePage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 创建和配置一个显示时间小时部分的标签
    ui_TimeHourLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TimeHourLabel, LV_SIZE_CONTENT);   /// 10
    lv_obj_set_height(ui_TimeHourLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TimeHourLabel, -105);
    lv_obj_set_y(ui_TimeHourLabel, -70);
    lv_obj_set_align(ui_TimeHourLabel, LV_ALIGN_RIGHT_MID);
    sprintf(value_strbuf,"%2d",ui_TimeHourValue);
    lv_label_set_text(ui_TimeHourLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_TimeHourLabel, &ui_font_Cuyuan48, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TimeColonLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TimeColonLabel, LV_SIZE_CONTENT);   /// 10
    lv_obj_set_height(ui_TimeColonLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TimeColonLabel, -82);
    lv_obj_set_y(ui_TimeColonLabel, -70);
    lv_obj_set_align(ui_TimeColonLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_TimeColonLabel, ":");
    lv_obj_set_style_text_color(ui_TimeColonLabel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TimeColonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TimeColonLabel, &ui_font_Cuyuan48, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TimeMinuteLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TimeMinuteLabel, LV_SIZE_CONTENT);   /// 10
    lv_obj_set_height(ui_TimeMinuteLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TimeMinuteLabel, -20);
    lv_obj_set_y(ui_TimeMinuteLabel, -70);
    lv_obj_set_align(ui_TimeMinuteLabel, LV_ALIGN_RIGHT_MID);
    sprintf(value_strbuf,"%02d",ui_TimeMinuteValue);
    lv_label_set_text(ui_TimeMinuteLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_TimeMinuteLabel, &ui_font_Cuyuan48, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // 一个显示电池电量的圆弧
    ui_BatArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_BatArc, 50);
    lv_obj_set_height(ui_BatArc, 50);
    lv_obj_set_x(ui_BatArc, -75);
    lv_obj_set_y(ui_BatArc, -68);
    lv_obj_set_align(ui_BatArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_BatArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_value(ui_BatArc, ui_BatArcValue);
    lv_obj_set_style_arc_width(ui_BatArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_BatArc, lv_color_hex(0x19C819), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_BatArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_BatArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_BatArc, lv_color_hex(0x19C819), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BatArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    // 一个显示电池电量的图标
    ui_BaticonLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_BaticonLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_BaticonLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_BaticonLabel, -75);
    lv_obj_set_y(ui_BaticonLabel, -50);
    lv_obj_set_align(ui_BaticonLabel, LV_ALIGN_CENTER);
    // lv_label_set_text(ui_BaticonLabel, "");
    lv_label_set_text(ui_BaticonLabel, "\uE68F");
    
    lv_obj_set_style_text_color(ui_BaticonLabel, lv_color_hex(0x19C819), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_BaticonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_BaticonLabel, &ui_font_iconfont16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示电池电量的数字
    ui_BatNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_BatNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_BatNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_BatNumLabel, -75);
    lv_obj_set_y(ui_BatNumLabel, -68);
    lv_obj_set_align(ui_BatNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%2d%%",ui_BatArcValue);
    lv_label_set_text(ui_BatNumLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_BatNumLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示日期的标签
    ui_DateLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_DateLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DateLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_DateLabel, -75);
    lv_obj_set_y(ui_DateLabel, -110);
    lv_obj_set_align(ui_DateLabel, LV_ALIGN_RIGHT_MID);
    sprintf(value_strbuf,"%2d-%02d",ui_DateMonthValue,ui_DateDayValue);
    lv_label_set_text(ui_DateLabel, value_strbuf);
    lv_obj_set_style_text_color(ui_DateLabel, lv_color_hex(0xFF461E), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_DateLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_DateLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // 一个显示星期的标签
    ui_DayLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_DayLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DayLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_DayLabel, -20);
    lv_obj_set_y(ui_DayLabel, -110);
    lv_obj_set_align(ui_DayLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue-1]);
    lv_obj_set_style_text_color(ui_DayLabel, lv_color_hex(0xFF461E), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_DayLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_DayLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示步数的图标
    ui_StepiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_StepiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_StepiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_StepiconLabel, -90);
    lv_obj_set_y(ui_StepiconLabel, -5);
    lv_obj_set_align(ui_StepiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_StepiconLabel, "");
    lv_obj_set_style_text_color(ui_StepiconLabel, lv_color_hex(0x3278FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_StepiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_StepiconLabel, &ui_font_iconfont24, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // 一个显示步数的中文标签
    ui_StepCnLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_StepCnLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_StepCnLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_StepCnLabel, -40);
    lv_obj_set_y(ui_StepCnLabel, -5);
    lv_obj_set_align(ui_StepCnLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_StepCnLabel, "今日步数");
    lv_obj_set_style_text_color(ui_StepCnLabel, lv_color_hex(0x3278FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_StepCnLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_StepCnLabel, &ui_font_Cuyuan18, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示步数的数字
    ui_StepNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_StepNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_StepNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_StepNumLabel, -80);
    lv_obj_set_y(ui_StepNumLabel, 20);
    lv_obj_set_align(ui_StepNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d",ui_StepNumValue);
    lv_label_set_text(ui_StepNumLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_StepNumLabel, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示步数的进度条
    ui_StepNumBar = lv_bar_create(ui_HomePage);
    lv_bar_set_value(ui_StepNumBar, 40, LV_ANIM_OFF);
    lv_obj_set_width(ui_StepNumBar, 200);
    lv_obj_set_height(ui_StepNumBar, 10);
    lv_obj_set_x(ui_StepNumBar, 0);
    lv_obj_set_y(ui_StepNumBar, 40);
    lv_obj_set_align(ui_StepNumBar, LV_ALIGN_CENTER);

    lv_obj_set_style_bg_color(ui_StepNumBar, lv_color_hex(0x3278FF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_StepNumBar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // 一个显示温度的图标
    ui_TempArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_TempArc, 50);
    lv_obj_set_height(ui_TempArc, 50);
    lv_obj_set_x(ui_TempArc, -75);
    lv_obj_set_y(ui_TempArc, 90);
    lv_obj_set_align(ui_TempArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_TempArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_range(ui_TempArc, -20, 60);
    lv_arc_set_value(ui_TempArc, ui_HomePageTempValue);
    lv_obj_set_style_arc_width(ui_TempArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_TempArc, lv_color_hex(0xF5A73A), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_TempArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_TempArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_TempArc, lv_color_hex(0xF5A73A), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TempArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    // 一个显示温度的图标
    ui_TempiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TempiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TempiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_TempiconLabel, -75);
    lv_obj_set_y(ui_TempiconLabel, 90);
    lv_obj_set_align(ui_TempiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_TempiconLabel, "");
    lv_obj_set_style_text_color(ui_TempiconLabel, lv_color_hex(0xF5A73A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TempiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TempiconLabel, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示温度的数字
    ui_TempNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TempNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TempNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_TempNumLabel, -73);
    lv_obj_set_y(ui_TempNumLabel, 125);
    lv_obj_set_align(ui_TempNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d",ui_HomePageTempValue);
    lv_label_set_text(ui_TempNumLabel, value_strbuf);

    // 一个显示湿度的图标
    ui_HumiiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HumiiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HumiiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HumiiconLabel, 0);
    lv_obj_set_y(ui_HumiiconLabel, 89);
    lv_obj_set_align(ui_HumiiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_HumiiconLabel, "");
    lv_obj_set_style_text_color(ui_HumiiconLabel, lv_color_hex(0x14C8E1), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_HumiiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_HumiiconLabel, &ui_font_iconfont28, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示湿度的进度条
    ui_HumiArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_HumiArc, 50);
    lv_obj_set_height(ui_HumiArc, 50);
    lv_obj_set_x(ui_HumiArc, 0);
    lv_obj_set_y(ui_HumiArc, 90);
    lv_obj_set_align(ui_HumiArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_HumiArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_value(ui_HumiArc, ui_HomePageHumiValue);
    lv_arc_set_bg_angles(ui_HumiArc, 270, 620);
    lv_obj_set_style_arc_width(ui_HumiArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_HumiArc, lv_color_hex(0x14C8E1), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_HumiArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_HumiArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_HumiArc, lv_color_hex(0x14C8E1), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_HumiArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    // 一个显示湿度的数字
    ui_HumiNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HumiNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HumiNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HumiNumLabel, 0);
    lv_obj_set_y(ui_HumiNumLabel, 125);
    lv_obj_set_align(ui_HumiNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d",ui_HomePageHumiValue);
    lv_label_set_text(ui_HumiNumLabel, value_strbuf);
    
    // 一个显示心率的图标
		ui_HRArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_HRArc, 50);
    lv_obj_set_height(ui_HRArc, 50);
    lv_obj_set_x(ui_HRArc, 70);
    lv_obj_set_y(ui_HRArc, 90);
    lv_obj_set_align(ui_HRArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_HRArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_value(ui_HRArc, 100);
    lv_arc_set_bg_angles(ui_HRArc, 270, 620);
    lv_obj_set_style_arc_width(ui_HRArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_HRArc, lv_color_hex(0xE11432), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_HRArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_HRArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_HRArc, lv_color_hex(0xE11432), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_HRArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    // 一个显示心率的图标
    ui_HRiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HRiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HRiconLabel, 70);
    lv_obj_set_y(ui_HRiconLabel, 90);
    lv_obj_set_align(ui_HRiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_HRiconLabel, "");
    lv_obj_set_style_text_color(ui_HRiconLabel, lv_color_hex(0xE11432), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_HRiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_HRiconLabel, &ui_font_iconfont32, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个显示心率的数字
    ui_HRNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HRNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HRNumLabel, 70);
    lv_obj_set_y(ui_HRNumLabel, 125);
    lv_obj_set_align(ui_HRNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d", HWInterface.HR_meter.HrRate);
    lv_label_set_text(ui_HRNumLabel, value_strbuf);

    // 下拉窗口
    ui_DropDownPanel = lv_obj_create(ui_HomePage);
    lv_obj_set_width(ui_DropDownPanel, 240);
    lv_obj_set_height(ui_DropDownPanel, 420);
    lv_obj_set_x(ui_DropDownPanel, 0);
    lv_obj_set_y(ui_DropDownPanel, -10);
    lv_obj_set_align(ui_DropDownPanel, LV_ALIGN_TOP_MID);
    // lv_obj_scroll_by(ui_DropDownPanel,0,-130,LV_ANIM_OFF);
    // lv_obj_scroll_to_y(ui_DropDownPanel,126, LV_ANIM_OFF);

    lv_obj_clear_flag(ui_DropDownPanel, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_CHAIN);      /// Flags
    //lv_obj_clear_flag(ui_DropDownPanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_CHAIN);      /// Flags
    
    lv_obj_set_scroll_dir(ui_DropDownPanel, LV_DIR_VER);// 垂直滚动
    lv_obj_set_style_bg_color(ui_DropDownPanel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DropDownPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);// 背景透明
    lv_obj_set_style_border_color(ui_DropDownPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_DropDownPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DropDownPanel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DropDownPanel, 1, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    

    // 一个背景
    ui_UpBGPanel = lv_obj_create(ui_DropDownPanel);
    lv_obj_set_width(ui_UpBGPanel, 240);
    lv_obj_set_height(ui_UpBGPanel, 130);
    lv_obj_set_x(ui_UpBGPanel, 0);
    lv_obj_set_y(ui_UpBGPanel, -10);
    lv_obj_set_align(ui_UpBGPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_UpBGPanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_UpBGPanel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_UpBGPanel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    

    // 一个NFC开关
    // ui_NFCButton = lv_btn_create(ui_DropDownPanel);
    ui_NFCButton = lv_btn_create(ui_UpBGPanel);
    lv_obj_set_width(ui_NFCButton, 50);
    lv_obj_set_height(ui_NFCButton, 50);
		lv_obj_set_x(ui_NFCButton, 0);
    lv_obj_set_y(ui_NFCButton, -2);
    lv_obj_add_flag(ui_NFCButton, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_NFCButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_NFCButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_NFCButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_NFCButton, lv_color_hex(0x3264C8), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_NFCButton, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    if(ui_HomePageNFCEN)
    {lv_obj_add_state(ui_NFCButton, LV_STATE_CHECKED);}

    // 一个NFC图标
    ui_NFCLabel = lv_label_create(ui_NFCButton);
    lv_obj_set_width(ui_NFCLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_NFCLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_NFCLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_NFCLabel, "");
    lv_obj_set_style_text_color(ui_NFCLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_NFCLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_NFCLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个BLE开关
    ui_BLEButton = lv_btn_create(ui_DropDownPanel);
    lv_obj_set_width(ui_BLEButton, 50);
    lv_obj_set_height(ui_BLEButton, 50);
    lv_obj_set_x(ui_BLEButton, 0);
    lv_obj_set_y(ui_BLEButton, 65);
    lv_obj_add_flag(ui_BLEButton, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_BLEButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_BLEButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BLEButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BLEButton, lv_color_hex(0x3264C8), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_BLEButton, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    if(ui_HomePageBLEEN)
    {lv_obj_add_state(ui_BLEButton, LV_STATE_CHECKED);}

    // 一个BLE图标
    ui_BLELabel = lv_label_create(ui_BLEButton);
    lv_obj_set_width(ui_BLELabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_BLELabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_BLELabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_BLELabel, "");
    lv_obj_set_style_text_color(ui_BLELabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_BLELabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_BLELabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个Power开关
    ui_PowerButton = lv_btn_create(ui_DropDownPanel);
    lv_obj_set_width(ui_PowerButton, 50);
    lv_obj_set_height(ui_PowerButton, 50);
	  lv_obj_set_x(ui_PowerButton, 0);
    lv_obj_set_y(ui_PowerButton, 5);
    lv_obj_set_align(ui_PowerButton, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_PowerButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_PowerButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_PowerButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PowerButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个Power图标
    ui_PowerLabel = lv_label_create(ui_PowerButton);
    lv_obj_set_width(ui_PowerLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_PowerLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_PowerLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_PowerLabel, "");
    lv_obj_set_style_text_color(ui_PowerLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_PowerLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_PowerLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个设置开关
    ui_SetButton = lv_btn_create(ui_DropDownPanel);
    lv_obj_set_width(ui_SetButton, 50);
    lv_obj_set_height(ui_SetButton, 50);
    lv_obj_set_x(ui_SetButton, 0);
    lv_obj_set_y(ui_SetButton, 65);
    lv_obj_set_align(ui_SetButton, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_SetButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_SetButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_SetButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_SetButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个设置图标
    ui_SetLabel = lv_label_create(ui_SetButton);
    lv_obj_set_width(ui_SetLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_SetLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_SetLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_SetLabel, "");
    lv_obj_set_style_text_color(ui_SetLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SetLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SetLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个亮度滑块
    ui_LightSlider = lv_slider_create(ui_DropDownPanel);
    lv_slider_set_value(ui_LightSlider, ui_LightSliderValue, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_LightSlider) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_LightSlider, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_LightSlider, 50);
    lv_obj_set_height(ui_LightSlider, 110);
	  lv_obj_set_x(ui_LightSlider, 0);
    lv_obj_set_y(ui_LightSlider, 5);
    lv_obj_set_align(ui_LightSlider, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_radius(ui_LightSlider, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LightSlider, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LightSlider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_LightSlider, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LightSlider, lv_color_hex(0x3264C8), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LightSlider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_LightSlider, lv_color_hex(0x000000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LightSlider, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    // 一个亮度图标
    ui_LightLabel = lv_label_create(ui_LightSlider);
    lv_obj_set_width(ui_LightLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LightLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LightLabel, 0);
    lv_obj_set_y(ui_LightLabel, 30);
    lv_obj_set_align(ui_LightLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LightLabel, "");
    lv_obj_set_style_text_color(ui_LightLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LightLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LightLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 一个背景面板
    ui_DownBGPanel = lv_obj_create(ui_DropDownPanel);
    lv_obj_set_width(ui_DownBGPanel, 240);
    lv_obj_set_height(ui_DownBGPanel, 130);
    lv_obj_set_x(ui_DownBGPanel, 0);
    lv_obj_set_y(ui_DownBGPanel, 420);
    lv_obj_set_align(ui_DownBGPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_DownBGPanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_DownBGPanel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DownBGPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_DownBGPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_DownBGPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_scroll_by(ui_DropDownPanel,0,-126,LV_ANIM_OFF);
    

		//events
		lv_obj_add_event_cb(ui_HomePage, ui_event_HomePage, LV_EVENT_ALL, NULL); //添加手势事件回调函数
    lv_obj_add_event_cb(ui_NFCButton, ui_event_NFCButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_BLEButton, ui_event_BLEButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_PowerButton, ui_event_PowerButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_SetButton, ui_event_SetButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_LightSlider,ui_event_LightSlider, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_DropDownPanel, scroll_event_cb, LV_EVENT_SCROLL_END, NULL);
    //timer
    ui_HomePageTimer = lv_timer_create(HomePage_timer_cb, 500,  NULL);


    lv_obj_scroll_to_y(ui_DropDownPanel,126, LV_ANIM_OFF);

}


/**
 * 初始化电源页面界面
 * 
 * 本函数负责创建电源页面的主要UI元素，包括页面对象、滑动条和标签，并设置它们的属性和样式
 * 此外，还为滑动条添加了事件回调函数，以处理用户的交互操作
 */
void ui_PowerPage_screen_init(void)
{
    // 创建电源页面的主对象
    ui_PowerPage = lv_obj_create(NULL);
    // 设置电源页面对象为不可滚动
    lv_obj_clear_flag(ui_PowerPage, LV_OBJ_FLAG_SCROLLABLE);      

    // 创建滑动条对象，并设置其基本属性和样式
    ui_PowerSlider = lv_slider_create(ui_PowerPage);
    lv_obj_set_width(ui_PowerSlider, 220);
    lv_obj_set_height(ui_PowerSlider, 50);
    lv_obj_set_align(ui_PowerSlider, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_PowerSlider, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PowerSlider, 128, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 设置滑动条指示器部分的样式
    lv_obj_set_style_bg_color(ui_PowerSlider, lv_color_hex(0x800000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PowerSlider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // 设置滑动条旋钮部分的样式
    lv_obj_set_style_bg_color(ui_PowerSlider, lv_color_hex(0x800000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PowerSlider, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    // 创建并设置滑动条上的标签
    ui_PowerDownLabel = lv_label_create(ui_PowerSlider);
    lv_obj_set_width(ui_PowerDownLabel, LV_SIZE_CONTENT);   
    lv_obj_set_height(ui_PowerDownLabel, LV_SIZE_CONTENT);    
    lv_obj_set_align(ui_PowerDownLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_PowerDownLabel, "滑动关机");
    lv_obj_set_style_text_color(ui_PowerDownLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_PowerDownLabel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_PowerDownLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 为滑动条添加事件回调函数，以处理所有类型的事件
    lv_obj_add_event_cb(ui_PowerSlider, ui_event_PowerSlider, LV_EVENT_ALL, NULL);
}

/////////////////// SCREEN deinit ////////////////////
void ui_HomePage_screen_deinit(void)
{
  lv_timer_del(ui_HomePageTimer);
}

void ui_PowerPage_screen_deinit(void)
{}
