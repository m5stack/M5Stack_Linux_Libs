
#include <stdio.h>
#include "imlib.h"

// assets/image/565.bmp
// assets/image/888.bmp
int main()
{
    image_t img_ts;
    imlib_init_all();
    memset(&img_ts, 0, sizeof(image_t));
    bmp_read(&img_ts, "../../assets/image/888.bmp");

    imlib_draw_line(&img_ts, 10, 10, 10, 100, COLOR_R8_G8_B8_TO_RGB888(0xff, 0x00, 0x00), 4);
    imlib_draw_line(&img_ts, 20, 10, 20, 100, COLOR_R8_G8_B8_TO_RGB888(0x00, 0xff, 0x00), 4);
    imlib_draw_line(&img_ts, 30, 10, 30, 100, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 4);

    imlib_draw_rectangle(&img_ts, 10, 120, 14, 100, COLOR_R8_G8_B8_TO_RGB888(0xff, 0x00, 0x00), 2, 0);
    imlib_draw_rectangle(&img_ts, 26, 120, 14, 100, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 1, 1);

    imlib_draw_circle(&img_ts, 70, 30, 20, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 2, false);
    imlib_draw_circle(&img_ts, 70, 100, 20, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 2, true);

    imlib_draw_string(&img_ts, 70, 150, "nihao", COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 3.0, 0, 0, 1, 0, 0, 0, 0, 0, 0);





    bmp_write_subimg(&img_ts, "./tmp.bmp",NULL);
    imlib_deinit_all();
    printf("hello\n");
    imlib_printf(5, "nihao imlib\n");
    return 0;
}

