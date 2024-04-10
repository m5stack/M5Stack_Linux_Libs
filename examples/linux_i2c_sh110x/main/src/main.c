#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "m5_sh1107/m5_sh1107.h"
#include "imlib.h"
// #include "run_time.h"


image_t *tmpp;



void display_show()
{
	uint8_t buff[8];
	// run_time_start();
	for (int p = 0; p < 64; p++)
	{
		
		memset(buff, 0, 8);
		for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if(imlib_get_pixel(tmpp, 63 - p, i * 8 + j))
					{
						buff[i] |= 0x01 << j; 
					}
				}
			}
		
		m5_sh1107_dev_set_img(0, p, buff, 8);
	}
	// print_run_time_ms();
	for (int p = 0; p < 64; p++)
	{
		memset(buff, 0, 8);
		for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if(imlib_get_pixel(tmpp, 63 - p, 64 + i * 8 + j))
					{
						buff[i] |= 0x01 << j; 
					}
				}
			}
		m5_sh1107_dev_set_img(8, p, buff, 8);
	}
}





int main()
{
    m5_sh1107_dev_init(1);
	
	tmpp = imlib_image_create(64, 128, PIXFORMAT_BINARY, 0, NULL, 1);
	
	memset(tmpp->data, 0, tmpp->size);


	
	imlib_draw_circle(tmpp, 32, 32, 5, 1, 1, 1);
	imlib_draw_circle(tmpp, 32, 96, 5, 1, 1, 1);
	imlib_draw_line(tmpp, 48, 59, 48, 69, 1, 1);
	imlib_draw_string(	tmpp, 			// image_t
						0, 			// x_off
						125, 			// y_off
						"ip:192.168.12.1", 		// str
						1, 				// c
						1.0, 			// scale
						0, 				// x_spacing
						0, 				// y_spacing
						1, 				// mono_space
						0, 				// char_rotation
						0, 				// char_hmirror
						0, 				// char_vflip
						270, 				// string_rotation
						0, 				// string_hmirror
						0				// string_vflip
						);
	
	display_show();
	


	m5_sh1107_dev_deinit();
	
	imlib_image_destroy(&tmpp);
	
    return 0;
}