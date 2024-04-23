
#include <stdio.h>
#include "imlib.h"
// #include "lib2_private.h"  // We can't include lib2_private.h for it's compoent2's private include dir

// /home/nihao/work/lin/minisyd/module/imlib/assets/image/565.bmp
int main()
{
    image_t img_ts;
    imlib_init_all();
    imlib_image_init(&img_ts, 240, 240, PIXFORMAT_RGB888, 0, NULL);
    bmp_read(&img_ts, "./lin.bmp");

    imlib_draw_line(&img_ts, 10, 10, 200, 200, COLOR_R8_G8_B8_TO_RGB888(0x00, 0x00, 0xff), 3);
    imlib_draw_line(&img_ts, 100, 10, 10, 100, COLOR_R8_G8_B8_TO_RGB888(0xff, 0x00, 0x00), 3);



    bmp_write_subimg(&img_ts, "./lin.bmp",NULL);
    imlib_deinit_all();
    printf("hello\n");
    imlib_printf(5, "nihao imlib\n");
    return 0;
}


// 42 4D 
// 38 A3 02 00 
// 00 00 
// 00 00 
// 36 00 00 00 


// 28 00 00 00 
// F0 00 00 00 
// F0 00 00 00 
// 01 00 
// 18 00 
// 00 00 00 00 
// 02 A3 02 00 
// 12 0B 00 00 
// 12 0B 00 00 
// 00 00 00 00 
// 00 00 00 00 
