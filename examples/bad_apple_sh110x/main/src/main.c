#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "m5_sh1107/m5_sh1107.h"
#include "imlib.h"


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
				if (imlib_get_pixel(tmpp, i * 8 + j, 63 - p))
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
				if (imlib_get_pixel(tmpp, 64 + i * 8 + j, 63 - p))
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

	int num = 0;

	while (1)
	{
		char file_n[50];
		sprintf(file_n, "/root/dist/output_bitmap_%d.bmp", num);
		printf("read file:%s\n", file_n);
		tmpp = imlib_image_create(128, 64, PIXFORMAT_BINARY, 0, NULL, 0);

		bmp_read(tmpp, file_n);

		display_show();

		imlib_image_destroy(&tmpp);
		num += 3;
		if (num >= 6571)
		{
			break;
		}
	}

	m5_sh1107_dev_deinit();

	imlib_image_destroy(&tmpp);

	return 0;
}