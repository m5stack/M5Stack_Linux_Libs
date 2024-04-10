// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.2.1
// LVGL VERSION: 8.3.4
// PROJECT: core135

#include "ui.h"
#include "ui_helpers.h"
#include "ui_comp.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_main00;
void ui_event_Button1(lv_event_t * e);
lv_obj_t * ui_Button1;
lv_obj_t * ui_Label1;
lv_obj_t * ui_Label4;
lv_obj_t * ui_Label22;
lv_obj_t * ui_Image2;
lv_obj_t * ui_Image1;
void ui_event_Image4(lv_event_t * e);
lv_obj_t * ui_Image4;
void ui_event_Image6(lv_event_t * e);
lv_obj_t * ui_Image6;
lv_obj_t * ui_main10;
void ui_event_Button3(lv_event_t * e);
lv_obj_t * ui_Button3;
lv_obj_t * ui_Label3;
lv_obj_t * ui_Label6;
lv_obj_t * ui_Arc5;
lv_obj_t * ui_Label23;
lv_obj_t * ui_Label21;
void ui_event_Image3(lv_event_t * e);
lv_obj_t * ui_Image3;
void ui_event_Image5(lv_event_t * e);
lv_obj_t * ui_Image5;
void ui_event_main20(lv_event_t * e);
lv_obj_t * ui_main20;
lv_obj_t * ui_Button7;
lv_obj_t * ui_Label10;
lv_obj_t * ui_Label12;
lv_obj_t * ui_Arc2;
lv_obj_t * ui_Label16;
lv_obj_t * ui_Label18;
lv_obj_t * ui_Arc1;
lv_obj_t * ui_Label15;
lv_obj_t * ui_Label17;
lv_obj_t * ui_Arc3;
lv_obj_t * ui_Label19;
lv_obj_t * ui_Label20;
void ui_event_Image7(lv_event_t * e);
lv_obj_t * ui_Image7;
void ui_event_Image8(lv_event_t * e);
lv_obj_t * ui_Image8;
lv_obj_t * ui_main30;
void ui_event_Button5(lv_event_t * e);
lv_obj_t * ui_Button5;
lv_obj_t * ui_Label7;
lv_obj_t * ui_Label9;
lv_obj_t * ui_Checkbox1;
lv_obj_t * ui_Checkbox2;
lv_obj_t * ui_Checkbox3;
lv_obj_t * ui_Checkbox4;
lv_obj_t * ui_Checkbox9;
lv_obj_t * ui_Checkbox10;
lv_obj_t * ui_Checkbox11;
lv_obj_t * ui_Checkbox12;
lv_obj_t * ui_TextArea2;
void ui_event_Image9(lv_event_t * e);
lv_obj_t * ui_Image9;
void ui_event_Image10(lv_event_t * e);
lv_obj_t * ui_Image10;
lv_obj_t * ui_main40;
void ui_event_Button8(lv_event_t * e);
lv_obj_t * ui_Button8;
lv_obj_t * ui_Label11;
lv_obj_t * ui_Label13;
lv_obj_t * ui_TextArea1;
void ui_event_Image11(lv_event_t * e);
lv_obj_t * ui_Image11;
void ui_event_Image12(lv_event_t * e);
lv_obj_t * ui_Image12;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
void ui_event_Button1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        test_lcd_fun(e);
    }
}
void ui_event_Image4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_up(e);
    }
}
void ui_event_Image6(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_down(e);
    }
}
void ui_event_Button3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        char_test_fun(e);
    }
}
void ui_event_Image3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_up(e);
    }
}
void ui_event_Image5(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_down(e);
    }
}
void ui_event_main20(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        test_speed_fun(e);
    }
}
void ui_event_Image7(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_up(e);
    }
}
void ui_event_Image8(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_down(e);
    }
}
void ui_event_Button5(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        test_device_fun(e);
    }
}
void ui_event_Image9(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_up(e);
    }
}
void ui_event_Image10(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_down(e);
    }
}
void ui_event_Button8(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        test_rtc_fun(e);
    }
}
void ui_event_Image11(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_up(e);
    }
}
void ui_event_Image12(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        set_page_down(e);
    }
}


typedef struct {
    lv_obj_t obj; // 继承自基础对象
    // 如果需要，这里可以添加额外的状态或数据
} custom_circle_t;
static void custom_circle_event_cb(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_PRESSED) {
        // 按下时，设置圆的颜色为红色
        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFF0000), 0);
    } else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        // 释放时，恢复圆的颜色为绿色
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x00FF00), 0);
    }
}
static void custom_circle_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj) {
    LV_UNUSED(class_p);
    
    // 设置默认样式：绿色圆圈
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0); // 设置为圆形
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00FF00), 0); // 设置背景颜色为绿色

    // 添加事件处理函数
    lv_obj_add_event_cb(obj, custom_circle_event_cb, LV_EVENT_ALL, NULL);
}

static lv_obj_class_t custom_circle_class = {
    .base_class = &lv_obj_class,
    .instance_size = sizeof(custom_circle_t),
    .constructor_cb = custom_circle_constructor,
};

lv_obj_t * custom_circle_create(lv_obj_t * parent) {
    lv_obj_t * obj = lv_obj_class_create_obj(&custom_circle_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}


















///////////////////// SCREENS ////////////////////
void ui_main00_screen_init(void)
{
    ui_main00 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_main00, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Button1 = lv_btn_create(ui_main00);
    lv_obj_set_width(ui_Button1, 100);
    lv_obj_set_height(ui_Button1, 50);
    lv_obj_set_align(ui_Button1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label1 = lv_label_create(ui_Button1);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "TEST LCD");

    ui_Label4 = lv_label_create(ui_main00);
    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label4, 0);
    lv_obj_set_y(ui_Label4, -100);
    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label4, "LCD TEST");

    ui_Label22 = lv_label_create(ui_main00);
    lv_obj_set_width(ui_Label22, lv_pct(80));
    lv_obj_set_height(ui_Label22, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label22, -3);
    lv_obj_set_y(ui_Label22, 65);
    lv_obj_set_align(ui_Label22, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label22, "Enter the pure color screen, please click the screen to continue");

    ui_Image2 = lv_img_create(ui_main00);
    lv_img_set_src(ui_Image2, &ui_img_487478209);
    lv_obj_set_width(ui_Image2, 20);
    lv_obj_set_height(ui_Image2, 20);
    lv_obj_set_x(ui_Image2, -146);
    lv_obj_set_y(ui_Image2, -106);
    lv_obj_set_align(ui_Image2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image1 = lv_img_create(ui_main00);
    lv_img_set_src(ui_Image1, &ui_img_1265052084);
    lv_obj_set_width(ui_Image1, 100);
    lv_obj_set_height(ui_Image1, 100);
    lv_obj_set_x(ui_Image1, 43);
    lv_obj_set_y(ui_Image1, 136);
    lv_obj_set_align(ui_Image1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image4 = lv_img_create(ui_main00);
    lv_img_set_src(ui_Image4, &ui_img_up_png);
    lv_obj_set_width(ui_Image4, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image4, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image4, -145);
    lv_obj_set_y(ui_Image4, -105);
    lv_obj_set_align(ui_Image4, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image4, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image6 = lv_img_create(ui_main00);
    lv_img_set_src(ui_Image6, &ui_img_down_png);
    lv_obj_set_width(ui_Image6, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image6, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image6, 145);
    lv_obj_set_y(ui_Image6, -105);
    lv_obj_set_align(ui_Image6, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image6, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image6, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_add_event_cb(ui_Button1, ui_event_Button1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image4, ui_event_Image4, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image6, ui_event_Image6, LV_EVENT_ALL, NULL);

    lv_obj_t * parent = lv_scr_act(); // 获取当前活动的屏幕
    lv_obj_t * circle = custom_circle_create(parent);
    lv_obj_set_size(circle, 100, 100); // 设置圆的大小
    lv_obj_center(circle); // 将圆居中显示



}
void ui_main10_screen_init(void)
{
    ui_main10 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_main10, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Button3 = lv_btn_create(ui_main10);
    lv_obj_set_width(ui_Button3, 100);
    lv_obj_set_height(ui_Button3, 50);
    lv_obj_set_x(ui_Button3, 0);
    lv_obj_set_y(ui_Button3, 80);
    lv_obj_set_align(ui_Button3, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label3 = lv_label_create(ui_Button3);
    lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label3, "CHAR TEST");

    ui_Label6 = lv_label_create(ui_main10);
    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label6, 0);
    lv_obj_set_y(ui_Label6, -100);
    lv_obj_set_align(ui_Label6, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label6, "POWER TEST");

    ui_Arc5 = lv_arc_create(ui_main10);
    lv_obj_set_width(ui_Arc5, 120);
    lv_obj_set_height(ui_Arc5, 135);
    lv_obj_set_x(ui_Arc5, 0);
    lv_obj_set_y(ui_Arc5, -20);
    lv_obj_set_align(ui_Arc5, LV_ALIGN_CENTER);

    ui_Label23 = lv_label_create(ui_Arc5);
    lv_obj_set_width(ui_Label23, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label23, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label23, 0);
    lv_obj_set_y(ui_Label23, -10);
    lv_obj_set_align(ui_Label23, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label23, "0 ma");

    ui_Label21 = lv_label_create(ui_Arc5);
    lv_obj_set_width(ui_Label21, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label21, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label21, 0);
    lv_obj_set_y(ui_Label21, 50);
    lv_obj_set_align(ui_Label21, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label21, "battery");

    ui_Image3 = lv_img_create(ui_main10);
    lv_img_set_src(ui_Image3, &ui_img_up_png);
    lv_obj_set_width(ui_Image3, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image3, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image3, -145);
    lv_obj_set_y(ui_Image3, -105);
    lv_obj_set_align(ui_Image3, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image3, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image5 = lv_img_create(ui_main10);
    lv_img_set_src(ui_Image5, &ui_img_down_png);
    lv_obj_set_width(ui_Image5, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image5, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image5, 145);
    lv_obj_set_y(ui_Image5, -105);
    lv_obj_set_align(ui_Image5, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image5, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image5, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_add_event_cb(ui_Button3, ui_event_Button3, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image3, ui_event_Image3, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image5, ui_event_Image5, LV_EVENT_ALL, NULL);

}
void ui_main20_screen_init(void)
{
    ui_main20 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_main20, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Button7 = lv_btn_create(ui_main20);
    lv_obj_set_width(ui_Button7, 100);
    lv_obj_set_height(ui_Button7, 50);
    lv_obj_set_x(ui_Button7, 0);
    lv_obj_set_y(ui_Button7, 80);
    lv_obj_set_align(ui_Button7, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button7, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button7, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label10 = lv_label_create(ui_Button7);
    lv_obj_set_width(ui_Label10, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label10, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label10, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label10, "test speed");

    ui_Label12 = lv_label_create(ui_main20);
    lv_obj_set_width(ui_Label12, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label12, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label12, 0);
    lv_obj_set_y(ui_Label12, -100);
    lv_obj_set_align(ui_Label12, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label12, "DEVICE TEST");

    ui_Arc2 = lv_arc_create(ui_main20);
    lv_obj_set_width(ui_Arc2, 100);
    lv_obj_set_height(ui_Arc2, 130);
    lv_obj_set_x(ui_Arc2, -105);
    lv_obj_set_y(ui_Arc2, -20);
    lv_obj_set_align(ui_Arc2, LV_ALIGN_CENTER);

    ui_Label16 = lv_label_create(ui_Arc2);
    lv_obj_set_width(ui_Label16, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label16, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label16, 0);
    lv_obj_set_y(ui_Label16, -15);
    lv_obj_set_align(ui_Label16, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label16, "0 kb/s");

    ui_Label18 = lv_label_create(ui_Arc2);
    lv_obj_set_width(ui_Label18, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label18, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label18, 0);
    lv_obj_set_y(ui_Label18, 35);
    lv_obj_set_align(ui_Label18, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label18, "eth0");

    ui_Arc1 = lv_arc_create(ui_main20);
    lv_obj_set_width(ui_Arc1, 100);
    lv_obj_set_height(ui_Arc1, 130);
    lv_obj_set_x(ui_Arc1, 0);
    lv_obj_set_y(ui_Arc1, -20);
    lv_obj_set_align(ui_Arc1, LV_ALIGN_CENTER);

    ui_Label15 = lv_label_create(ui_Arc1);
    lv_obj_set_width(ui_Label15, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label15, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label15, 0);
    lv_obj_set_y(ui_Label15, -15);
    lv_obj_set_align(ui_Label15, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label15, "0 kb/s");

    ui_Label17 = lv_label_create(ui_Arc1);
    lv_obj_set_width(ui_Label17, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label17, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label17, 0);
    lv_obj_set_y(ui_Label17, 35);
    lv_obj_set_align(ui_Label17, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label17, "eth1");

    ui_Arc3 = lv_arc_create(ui_main20);
    lv_obj_set_width(ui_Arc3, 100);
    lv_obj_set_height(ui_Arc3, 130);
    lv_obj_set_x(ui_Arc3, 105);
    lv_obj_set_y(ui_Arc3, -20);
    lv_obj_set_align(ui_Arc3, LV_ALIGN_CENTER);

    ui_Label19 = lv_label_create(ui_Arc3);
    lv_obj_set_width(ui_Label19, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label19, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label19, 0);
    lv_obj_set_y(ui_Label19, -15);
    lv_obj_set_align(ui_Label19, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label19, "0 kb/s");

    ui_Label20 = lv_label_create(ui_Arc3);
    lv_obj_set_width(ui_Label20, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label20, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label20, 0);
    lv_obj_set_y(ui_Label20, 35);
    lv_obj_set_align(ui_Label20, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label20, "wifi");

    ui_Image7 = lv_img_create(ui_main20);
    lv_img_set_src(ui_Image7, &ui_img_up_png);
    lv_obj_set_width(ui_Image7, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image7, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image7, -145);
    lv_obj_set_y(ui_Image7, -105);
    lv_obj_set_align(ui_Image7, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image7, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image7, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image8 = lv_img_create(ui_main20);
    lv_img_set_src(ui_Image8, &ui_img_down_png);
    lv_obj_set_width(ui_Image8, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image8, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image8, 145);
    lv_obj_set_y(ui_Image8, -105);
    lv_obj_set_align(ui_Image8, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image8, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image8, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_add_event_cb(ui_Image7, ui_event_Image7, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image8, ui_event_Image8, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_main20, ui_event_main20, LV_EVENT_ALL, NULL);

}
void ui_main30_screen_init(void)
{
    ui_main30 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_main30, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Button5 = lv_btn_create(ui_main30);
    lv_obj_set_width(ui_Button5, 100);
    lv_obj_set_height(ui_Button5, 50);
    lv_obj_set_x(ui_Button5, 0);
    lv_obj_set_y(ui_Button5, 80);
    lv_obj_set_align(ui_Button5, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button5, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button5, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label7 = lv_label_create(ui_Button5);
    lv_obj_set_width(ui_Label7, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label7, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label7, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label7, "test device");

    ui_Label9 = lv_label_create(ui_main30);
    lv_obj_set_width(ui_Label9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label9, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label9, 0);
    lv_obj_set_y(ui_Label9, -100);
    lv_obj_set_align(ui_Label9, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label9, "DEVICE TEST");

    ui_Checkbox1 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox1, "usb");
    lv_obj_set_width(ui_Checkbox1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox1, 15);
    lv_obj_set_y(ui_Checkbox1, 35);
    lv_obj_add_flag(ui_Checkbox1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_Checkbox2 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox2, "otg");
    lv_obj_set_width(ui_Checkbox2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox2, 15);
    lv_obj_set_y(ui_Checkbox2, 60);
    lv_obj_add_flag(ui_Checkbox2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_Checkbox3 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox3, "lan");
    lv_obj_set_width(ui_Checkbox3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox3, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox3, 15);
    lv_obj_set_y(ui_Checkbox3, 85);
    lv_obj_add_flag(ui_Checkbox3, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_Checkbox4 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox4, "i2c");
    lv_obj_set_width(ui_Checkbox4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox4, 15);
    lv_obj_set_y(ui_Checkbox4, 110);
    lv_obj_add_flag(ui_Checkbox4, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_Checkbox9 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox9, "uart");
    lv_obj_set_width(ui_Checkbox9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox9, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox9, 90);
    lv_obj_set_y(ui_Checkbox9, 35);
    lv_obj_add_flag(ui_Checkbox9, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_Checkbox10 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox10, "485");
    lv_obj_set_width(ui_Checkbox10, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox10, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox10, 90);
    lv_obj_set_y(ui_Checkbox10, 60);
    lv_obj_add_flag(ui_Checkbox10, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_Checkbox11 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox11, "can");
    lv_obj_set_width(ui_Checkbox11, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox11, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox11, 90);
    lv_obj_set_y(ui_Checkbox11, 85);
    lv_obj_add_flag(ui_Checkbox11, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_Checkbox12 = lv_checkbox_create(ui_main30);
    lv_checkbox_set_text(ui_Checkbox12, "bus");
    lv_obj_set_width(ui_Checkbox12, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Checkbox12, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Checkbox12, 90);
    lv_obj_set_y(ui_Checkbox12, 110);
    lv_obj_add_flag(ui_Checkbox12, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags

    ui_TextArea2 = lv_textarea_create(ui_main30);
    lv_obj_set_width(ui_TextArea2, 150);
    lv_obj_set_height(ui_TextArea2, 95);
    lv_obj_set_x(ui_TextArea2, 160);
    lv_obj_set_y(ui_TextArea2, 35);
    lv_textarea_set_placeholder_text(ui_TextArea2, "Placeholder...");

    ui_Image9 = lv_img_create(ui_main30);
    lv_img_set_src(ui_Image9, &ui_img_up_png);
    lv_obj_set_width(ui_Image9, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image9, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image9, -145);
    lv_obj_set_y(ui_Image9, -105);
    lv_obj_set_align(ui_Image9, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image9, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image9, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image10 = lv_img_create(ui_main30);
    lv_img_set_src(ui_Image10, &ui_img_down_png);
    lv_obj_set_width(ui_Image10, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image10, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image10, 145);
    lv_obj_set_y(ui_Image10, -105);
    lv_obj_set_align(ui_Image10, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image10, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image10, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_add_event_cb(ui_Button5, ui_event_Button5, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image9, ui_event_Image9, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image10, ui_event_Image10, LV_EVENT_ALL, NULL);

}
void ui_main40_screen_init(void)
{
    ui_main40 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_main40, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Button8 = lv_btn_create(ui_main40);
    lv_obj_set_width(ui_Button8, 100);
    lv_obj_set_height(ui_Button8, 50);
    lv_obj_set_align(ui_Button8, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button8, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button8, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label11 = lv_label_create(ui_Button8);
    lv_obj_set_width(ui_Label11, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label11, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label11, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label11, "test rtc");

    ui_Label13 = lv_label_create(ui_main40);
    lv_obj_set_width(ui_Label13, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label13, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label13, 0);
    lv_obj_set_y(ui_Label13, -100);
    lv_obj_set_align(ui_Label13, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label13, "DEVICE TEST");

    ui_TextArea1 = lv_textarea_create(ui_main40);
    lv_obj_set_width(ui_TextArea1, 315);
    lv_obj_set_height(ui_TextArea1, 100);
    lv_obj_set_x(ui_TextArea1, 1);
    lv_obj_set_y(ui_TextArea1, 81);
    lv_obj_set_align(ui_TextArea1, LV_ALIGN_CENTER);
    lv_textarea_set_placeholder_text(ui_TextArea1, "Placeholder...");

    ui_Image11 = lv_img_create(ui_main40);
    lv_img_set_src(ui_Image11, &ui_img_up_png);
    lv_obj_set_width(ui_Image11, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image11, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image11, -145);
    lv_obj_set_y(ui_Image11, -105);
    lv_obj_set_align(ui_Image11, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image11, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image11, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Image12 = lv_img_create(ui_main40);
    lv_img_set_src(ui_Image12, &ui_img_down_png);
    lv_obj_set_width(ui_Image12, LV_SIZE_CONTENT);   /// 20
    lv_obj_set_height(ui_Image12, LV_SIZE_CONTENT);    /// 20
    lv_obj_set_x(ui_Image12, 145);
    lv_obj_set_y(ui_Image12, -105);
    lv_obj_set_align(ui_Image12, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image12, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image12, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_add_event_cb(ui_Button8, ui_event_Button8, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image11, ui_event_Image11, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Image12, ui_event_Image12, LV_EVENT_ALL, NULL);

}

void ui_init(void)
{
    LV_EVENT_GET_COMP_CHILD = lv_event_register_id();

    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_main00_screen_init();
    ui_main10_screen_init();
    ui_main20_screen_init();
    ui_main30_screen_init();
    ui_main40_screen_init();
    lv_disp_load_scr(ui_main00);
}
