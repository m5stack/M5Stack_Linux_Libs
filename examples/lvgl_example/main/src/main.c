/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

#define USE_DIRECT_MODE 0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_port_disp_init(void);
static void lv_port_indev_init(const char* path, bool show_cursor);
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

int main(int argc, char** argv)
{
    /*Initialize LVGL*/
    lv_init();

    /*Initialize the HAL (display, input devices, tick) for LVGL*/
    lv_port_disp_init();
    cursor_set_hidden(false);

    const char* devpath = argv[1];
    if (devpath) {
        LV_LOG_USER("evdev path: %s", devpath);
        lv_port_indev_init(devpath, true);
    } else {
        LV_LOG_WARN("evdev path is empty");
    }

    lv_demo_benchmark(LV_DEMO_BENCHMARK_MODE_RENDER_AND_DRIVER);

    while (1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        uint32_t sleep_ms = lv_timer_handler();
        usleep(sleep_ms * 1000);
    }

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

uint32_t millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return tick;
}

#if USE_DIRECT_MODE
static void fbdev_flush_direct(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p)
{
    lv_disp_flush_ready(drv);
}
#endif

static void lv_port_disp_init(void)
{
    /*Linux frame buffer device init*/
    fbdev_init();

    uint32_t width, height, dpi;
    fbdev_get_sizes(&width, &height, &dpi);

    LV_LOG_USER("fbdev: %" PRIu32 " x %" PRIu32 " DPI: %" PRIu32, width, height, dpi);

    /*A buffer for LittlevGL to draw the screen's content*/
    uint32_t buf_size = width * height;

#if USE_DIRECT_MODE
    lv_color_t* buf = fbdev_get_fbp();
    LV_ASSERT_NULL(buf);
#else
    lv_color_t* buf = malloc(buf_size * sizeof(lv_color_t));
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

static void lv_port_indev_init(const char* path, bool show_cursor)
{
    evdev_init();
    evdev_set_file(path);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    lv_indev_t* indev = lv_indev_drv_register(&indev_drv);

    if (show_cursor) {
        /*Set a cursor for the mouse*/
        LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
        lv_obj_t* cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
        lv_img_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
        lv_indev_set_cursor(indev, cursor_obj); /*Connect the image  object to the driver*/
    }

    lv_timer_set_period(lv_indev_get_read_timer(indev), 16);
}

static void cursor_set_hidden(bool en)
{
    // hide cursor echo -e "\033[?25l"
    // show cursor echo -e "\033[?25h"
    int ret = system(en ? "echo -e \"\033[?25l\"" : "echo -e \"\033[?25h\"");
    LV_LOG_USER(ret == 0 ? " OK" : " ERROR");
}

