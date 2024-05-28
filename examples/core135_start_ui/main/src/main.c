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
#include "ui/ui.h"

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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#include <pthread.h>
pthread_mutex_t device_thread_mutex;

int startui = 0;


int main(int argc, char **argv)
{
    if(argc > 1)
    {
        startui = atoi(argv[1]);
    }
    pthread_mutex_init(&device_thread_mutex, NULL);
    /*Initialize LVGL*/
    lv_init();

    /*Initialize the HAL (display, input devices, tick) for LVGL*/
    hal_init();

    ui_init();
    lv_obj_set_tile_id(ui_Tileview1, startui, 0, LV_ANIM_OFF);
    while (1)
    {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        pthread_mutex_lock(&device_thread_mutex);
        lv_timer_handler();
        pthread_mutex_unlock(&device_thread_mutex);
        usleep(5 * 1000);
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#if USE_SDL
#include <SDL2/SDL.h>
#include "lv_drivers/sdl/sdl.h"
#endif
void lv_port_disp_init_____(void);
void lv_port_indev_init____(const char *path, bool show_cursor);
/**
 * Initialize the Hardware Abstraction Layer (HAL) for LVGL
 */
static void hal_init(void)
{


    lv_port_disp_init_____();
    lv_port_indev_init____("/dev/input/event0", 1);

    // #endif
}

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <stdlib.h>
void lv_port_disp_init_____(void)
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

void lv_port_indev_init____(const char *path, bool show_cursor)
{
    evdev_init();
    // evdev_set_file(path);

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

#include "time.h"
uint32_t ___millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return tick;
}
void cursor_set_hidden(bool en)
{
    // hide cursor echo -e "\033[?25l"
    // show cursor echo -e "\033[?25h"
    int ret = system(en ? "echo -e \"\033[?25l\"" : "echo -e \"\033[?25h\"");
    LV_LOG_USER(ret == 0 ? " OK" : " ERROR");
}
