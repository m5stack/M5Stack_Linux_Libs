/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include "lvgl/lvgl.h"

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <stdlib.h>
#include <time.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static void cursor_set_hidden(bool en);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_port_disp_init(void);
static void hal_init(void);
static void lv_port_indev_init(bool show_cursor);
static void cursor_set_hidden(bool en);
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    lv_obj_t * ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t * ui_Label17 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Label17, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label17, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Label17, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label17, "hello world");

    lv_obj_t * ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_Screen1);
}


int main(int argc, char **argv)
{
    /*Initialize LVGL*/
    lv_init();

    /*Initialize the HAL (display, input devices, tick) for LVGL*/
    hal_init();





    while (1)
    {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
        usleep(5 * 1000);
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for LVGL
 */

static void hal_init(void)
{
    lv_port_disp_init();
    lv_port_indev_init(1);
}

static void lv_port_disp_init(void)
{
    /*Linux frame buffer device init*/

    fbdev_init();

    uint32_t width, height, dpi;
    fbdev_get_sizes(&width, &height, &dpi);

    printf("fbdev: %" PRIu32 " x %" PRIu32 " DPI: %" PRIu32 "\n", width, height, dpi);
    /*A buffer for LittlevGL to draw the screen's content*/
    uint32_t buf_size = width * height;

#if USE_DIRECT_MODE
    lv_color_t *buf = fbdev_get_fbp();
    LV_ASSERT_NULL(buf);
#else
    lv_color_t *buf = malloc(buf_size * sizeof(lv_color_t));
    LV_ASSERT_MALLOC(buf);
#endif

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, buf_size);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
#if USE_DIRECT_MODE
    disp_drv.flush_cb = fbdev_flush_direct;
    disp_drv.direct_mode = 1;
#else
    disp_drv.flush_cb = fbdev_flush;
#endif
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    lv_disp_drv_register(&disp_drv);

    lv_timer_set_period(_lv_disp_get_refr_timer(lv_disp_get_default()), 16);
    lv_timer_set_period(lv_anim_get_timer(), 16);
}

static void lv_port_indev_init(bool show_cursor)
{
    evdev_init();

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);

    if (show_cursor)
    {
        /*Set a cursor for the mouse*/
        LV_IMG_DECLARE(mouse_cursor_icon);                  /*Declare the image file.*/
        lv_obj_t *cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
        lv_img_set_src(cursor_obj, &mouse_cursor_icon);     /*Set the image source*/
        lv_indev_set_cursor(indev, cursor_obj);             /*Connect the image  object to the driver*/
    }

    lv_timer_set_period(lv_indev_get_read_timer(indev), 16);
}

uint32_t linux_millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return tick;
}
static void cursor_set_hidden(bool en)
{
    // hide cursor echo -e "\033[?25l"
    // show cursor echo -e "\033[?25h"
    int ret = system(en ? "echo -e \"\033[?25l\"" : "echo -e \"\033[?25h\"");
    LV_LOG_USER(ret == 0 ? " OK" : " ERROR");
}
