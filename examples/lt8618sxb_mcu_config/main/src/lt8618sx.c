/******************************************************************************
  * @project: LT8619C
  * @file: lt8618sx.c
  * @author: zll
  * @company: LONTIUM COPYRIGHT and CONFIDENTIAL
  * @date: 2018.5.30
/******************************************************************************/

#include "lt8618sx.h"
#include "hdmi_io.h"
#include <string.h>
#include "log.h"
/*note phease_offset should be setted according to TTL timing.*/

/*************************************
   Resolution			HDMI_VIC
   --------------------------------------
   640x480		1
   720x480P 60Hz		2
   720x480i 60Hz		6

   720x576P 50Hz		17
   720x576i 50Hz		21

   1280x720P 24Hz		60
   1280x720P 25Hz		61
   1280x720P 30Hz		62
   1280x720P 50Hz		19
   1280x720P 60Hz		4

   1920x1080P 24Hz	32
   1920x1080P 25Hz	33
   1920x1080P 30Hz	34

   1920x1080i 50Hz		20
   1920x1080i 60Hz		5

   1920x1080P 50Hz	31
   1920x1080P 60Hz	16

   3840x2160P 24Hz	207
   3840x2160P 25Hz	206
   3840x2160P 30Hz	205

   Other resolution	0(default)

 **************************************/
void hdmi_lt8618sx_init(hdmi_lt8618sx_t *lt86x)
{
	memset(lt86x, 0, sizeof(hdmi_lt8618sx_t));
	lt86x->vid_chk_flag = 1;
}

enum
{
	_32KHz = 0,
	_44d1KHz,
	_48KHz,
	_88d2KHz,
	_96KHz,
	_176Khz,
	_196KHz
};

u16 IIS_N[] =
	{
		4096,  // 32K
		6272,  // 44.1K
		6144,  // 48K
		12544, // 88.2K
		12288, // 96K
		25088, // 176K
		24576  // 196K
};

u16 Sample_Freq[] =
	{
		0x30, // 32K
		0x00, // 44.1K
		0x20, // 48K
		0x80, // 88.2K
		0xa0, // 96K
		0xc0, // 176K
		0xe0  //  196K
};

enum
{
	_16bit = 1,
	_20bit,
	_24_32bit
};

#define Sampling_rate _48KHz
#define Sampling_Size _24_32bit

// 												hfp  hs  hbp  hact  htotal  vfp  vs  vbp  vact  vtotal vic vpol hpol;
const struct video_timing video_640x480_60Hz = {16, 96, 48, 640, 800, 10, 2, 33, 480, 525};
const struct video_timing video_720x480_60Hz = {16, 62, 60, 720, 858, 9, 6, 30, 480, 525};
const struct video_timing video_800x600_60Hz = {16, 62, 60, 800, 1056, 9, 6, 30, 600, 628};
const struct video_timing video_1280x720_60Hz = {110, 40, 220, 1280, 1650, 5, 5, 20, 720, 750};
const struct video_timing video_1280x720_30Hz = {736, 40, 1244, 1280, 3300, 5, 5, 20, 720, 750};
const struct video_timing video_1920x1080_60Hz = {88, 44, 148, 1920, 2200, 4, 5, 36, 1080, 1125};
const struct video_timing video_1920x1080_50Hz = {528, 44, 148, 1920, 2640, 4, 5, 36, 1080, 1125};
const struct video_timing video_1920x1080_30Hz = {528, 44, 148, 1920, 2640, 4, 5, 36, 1080, 1125};
const struct video_timing video_1280x1024_60Hz = {48, 112, 248, 1280, 1688, 1, 3, 38, 1024, 1066};
const struct video_timing video_1024x768_60Hz = {24, 136, 160, 1024, 1344, 3, 6, 29, 768, 806};
const struct video_timing video_3840x2160_30Hz = {176, 88, 296, 3840, 4400, 8, 10, 72, 2160, 2250};
// const struct video_timing video_3840x2160_24Hz   = {1276,88, 296,3840,  5500,  8,  10, 72, 2160, 2250};
// const struct video_timing video_3840x2160_25Hz   = {1056,88, 296,3840,  5280,  8,  10, 72, 2160, 2250};

/*-------------------------io inface-------------------------*/

__attribute__((weak)) void RESET_Lt8618SX(void)
{
}
__attribute__((weak)) void HDMI_I2C_INIT(void)
{
}
__attribute__((weak)) u8 HDMI_WriteI2C_Byte(u8 RegAddr, u8 d)
{
	return 0x00;
}
__attribute__((weak)) u8 HDMI_ReadI2C_Byte(u8 RegAddr)
{
	return 0x00;
}
__attribute__((weak)) void HDMI_I2C_DEINIT(void)
{
}
__attribute__((weak)) void Delay_ms(int ms)
{
}
/*-----------------------------------------------------------*/

void LT8618SX_Chip_ID(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	HDMI_WriteI2C_Byte(0xFF, 0x80);
	HDMI_WriteI2C_Byte(0xee, 0x01);
	lt86x->chip_id[0] = HDMI_ReadI2C_Byte(0x00);
	lt86x->chip_id[1] = HDMI_ReadI2C_Byte(0x01);
	lt86x->chip_id[2] = HDMI_ReadI2C_Byte(0x02);
	log_info("LT8618SX Chip ID = %02x %02x %02x", lt86x->chip_id[0], lt86x->chip_id[1], lt86x->chip_id[2]);
}

void LT8618SX_RST_PD_Init(void)
{
	log_trace("%s", __FUNCTION__);
	HDMI_WriteI2C_Byte(0xff, 0x80);
	HDMI_WriteI2C_Byte(0xee, 0x01);
	HDMI_WriteI2C_Byte(0x11, 0x00); // reset MIPI Rx logic
	HDMI_WriteI2C_Byte(0x13, 0xf1);
	HDMI_WriteI2C_Byte(0x13, 0xf9); // reset TTL video process
	HDMI_WriteI2C_Byte(0x0a, 0x80);
}

void LT8618SX_TTL_Input_Analog(void)
{
	log_trace("%s", __FUNCTION__);
	// TTL mode
	// HDMI_WriteI2C_Byte(0xff, 0x81);
	HDMI_WriteI2C_Byte(0x02, 0x66);
	HDMI_WriteI2C_Byte(0x0a, 0x06); // 0x06
	HDMI_WriteI2C_Byte(0x15, 0x06);

	HDMI_WriteI2C_Byte(0x4e, 0xa8);

	HDMI_WriteI2C_Byte(0xff, 0x82); // HDMI_WriteI2C_Byte(0xff, 0x82); // ring
	HDMI_WriteI2C_Byte(0x1b, 0x77);
	HDMI_WriteI2C_Byte(0x1c, 0xec);
}

void LT8618SX_TTL_Input_Digital(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	switch (lt86x->inmode)
	{
	// TTL mode
	case Input_RGB888:
	{
		log_info("Video_Input_Mode=Input_RGB888");
		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0x45, 0x00); // RGB channel swap
		if (lt86x->ddrclk)
		{
			HDMI_WriteI2C_Byte(0x4f, 0xc0); // 0x80;  0xc0: invert dclk
		}
		else
		{
			HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x00;  0x40: invert dclk
		}
		HDMI_WriteI2C_Byte(0x50, 0x00);
		HDMI_WriteI2C_Byte(0x47, 0x07);
	}
	break;
	case Input_RGB_12BIT:
	{
		log_info("Video_Input_Mode=Input_RGB_12BIT");
		HDMI_WriteI2C_Byte(0xff, 0x80);
		HDMI_WriteI2C_Byte(0x0a, 0x80);

		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0x45, 0x70); // RGB channel swap

		HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x00;  0x40: invert dclk

		HDMI_WriteI2C_Byte(0x50, 0x00);
		HDMI_WriteI2C_Byte(0x51, 0x30);
		HDMI_WriteI2C_Byte(0x40, 0x00);
		HDMI_WriteI2C_Byte(0x41, 0xcd);
	}
	break;
	case Input_YCbCr444:
	{
		log_info("Video_Input_Mode=Input_YCbCr444");
		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0x45, 0x70); // RGB channel swap
		HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x00;  0x40: dclk
	}
	break;
	case Input_YCbCr422_16BIT:
	{
		log_info("Video_Input_Mode=Input_YCbCr422_16BIT");
		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0x45, 0x00); // RGB channel swap
		if (lt86x->ddrclk)
		{
			HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x80;  0xc0: invert dclk
		}
		else
		{
			HDMI_WriteI2C_Byte(0x4f, 0x00); // 0x00;  0x40: invert dclk
		}
	}
	break;
	case Input_BT1120_16BIT:
	{
		log_info("Video_Input_Mode=Input_BT1120_16BIT");
		HDMI_WriteI2C_Byte(0xff, 0x82);
		// HDMI_WriteI2C_Byte(0x45,0x30); // D0 ~ D7 Y ; D8 ~ D15 C
		HDMI_WriteI2C_Byte(0x45, 0x70); // D8 ~ D15 Y ; D16 ~ D23 C
		// HDMI_WriteI2C_Byte(0x45,0x00); // D0 ~ D7 C ; D8 ~ D15 Y
		// HDMI_WriteI2C_Byte(0x45,0x60); // D8 ~ D15 C ; D16 ~ D23 Y
		if (lt86x->ddrclk)
		{
			HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x80;  0xc0: invert dclk
		}
		else
		{
			HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x00;  0x40: invert dclk
		}
		HDMI_WriteI2C_Byte(0x48, 0x08); // 0x1c  0x08 Embedded sync mode input enable.
		HDMI_WriteI2C_Byte(0x51, 0x42); // 0x02 0x43 0x34 embedded DE mode input edable.
		HDMI_WriteI2C_Byte(0x47, 0x37);
	}
	break;
	case Input_BT656_8BIT:
	{
		log_info("Video_Input_Mode=Input_BT656_8BIT");
		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0x45, 0x00); // RGB channel swap
		if (lt86x->ddrclk)
		{
			HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x80;  0xc0: dclk
			HDMI_WriteI2C_Byte(0x48, 0x5c); // 0x50 0x5c 0x40 0x48
		}
		else
		{
			HDMI_WriteI2C_Byte(0x4f, 0x40); // 0x00;  0x40: dclk
			HDMI_WriteI2C_Byte(0x48, 0x48);
		}
		HDMI_WriteI2C_Byte(0x51, 0x42);
		HDMI_WriteI2C_Byte(0x47, 0x87);
	}
	break;
	case Input_BT601_8BIT:
	{
		log_info("Video_Input_Mode=Input_BT601_8BIT");
		HDMI_WriteI2C_Byte(0xff, 0x81);
		HDMI_WriteI2C_Byte(0x0a, 0x90);

		HDMI_WriteI2C_Byte(0xff, 0x81);
		HDMI_WriteI2C_Byte(0x4e, 0x02);

		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0x45, 0x00); // RGB channel swap
		if (lt86x->ddrclk)
		{
			HDMI_WriteI2C_Byte(0x4f, 0xc0); // 0x80;  0xc0: dclk
			HDMI_WriteI2C_Byte(0x48, 0x5c); // 0x50 0x5c 0x40 0x48
		}
		else
		{
			HDMI_WriteI2C_Byte(0x4f, 0xc0); // 0x00;  0x40: dclk
			HDMI_WriteI2C_Byte(0x48, 0x40);
		}
		HDMI_WriteI2C_Byte(0x51, 0x00);
		HDMI_WriteI2C_Byte(0x47, 0x87);
	}
	break;
	default:
		break;
	}
}

int LT8618SX_PLL_Version_U2(hdmi_lt8618sx_t *lt86x)
{
	// 	u8 read_val;
	// 	u8 j;
	// 	u8 cali_done;
	// 	u8 cali_val;
	// 	u8 lock;
	// 	u32 dclk;

	// 	dclk = LT8618SX_CLK_Det();

	// 	switch (lt86x->inmode)
	// 	{
	// 	case Input_RGB888:
	// 	case Input_YCbCr444:
	// 	case Input_YCbCr422_16BIT:
	// 	case Input_BT1120_16BIT:
	// 	{
	// 		HDMI_WriteI2C_Byte(0xff, 0x81);
	// 		HDMI_WriteI2C_Byte(0x23, 0x40);
	// 		HDMI_WriteI2C_Byte(0x24, 0x62); // icp set
	// 		HDMI_WriteI2C_Byte(0x26, 0x55);

	// 		if (dclk <= 25000)
	// 		{
	// 			HDMI_WriteI2C_Byte(0x25, 0x00);
	// 			// HDMI_WriteI2C_Byte( 0x2c, 0xA8 );
	// 			HDMI_WriteI2C_Byte(0x2c, 0x94);
	// 			HDMI_WriteI2C_Byte(0x2d, 0xaa);
	// 		}
	// 		else if ((dclk > 25000) && (dclk <= 50000))
	// 		{
	// 			HDMI_WriteI2C_Byte(0x25, 0x00);
	// 			HDMI_WriteI2C_Byte(0x2d, 0xaa);
	// 			HDMI_WriteI2C_Byte(0x2c, 0x94);
	// 		}
	// 		else if ((dclk > 50000) && (dclk <= 100000))
	// 		{
	// 			HDMI_WriteI2C_Byte(0x25, 0x01);
	// 			HDMI_WriteI2C_Byte(0x2d, 0x99);
	// 			HDMI_WriteI2C_Byte(0x2c, 0x94);

	// 			printf("\r\n50~100m");
	// 		}

	// 		else // if(dclk>100000)
	// 		{
	// 			HDMI_WriteI2C_Byte(0x25, 0x03);
	// 			HDMI_WriteI2C_Byte(0x2d, 0x88);
	// 			HDMI_WriteI2C_Byte(0x2c, 0x94);
	// 		}

	// 		if (USE_DDRCLK)
	// 		{
	// 			read_val = HDMI_ReadI2C_Byte(0x2c) & 0x7f;
	// 			read_val = read_val * 2 | 0x80;
	// 			HDMI_WriteI2C_Byte(0x2c, read_val);
	// 			printf("\r\n0x812c=%x", (u16)read_val);
	// 			HDMI_WriteI2C_Byte(0x4d, 0x05);
	// 			HDMI_WriteI2C_Byte(0x27, 0x66); // 0x60 //ddr 0x66
	// 			HDMI_WriteI2C_Byte(0x28, 0x88);
	// 			log_info("\r\n PLL DDR");
	// 		}
	// 		else
	// 		{
	// 			HDMI_WriteI2C_Byte(0x4d, 0x01);
	// 			HDMI_WriteI2C_Byte(0x27, 0x60); // 0x06                                              //0x60 //ddr 0x66
	// 			HDMI_WriteI2C_Byte(0x28, 0x88); // 0x88
	// 			log_info("\r\n PLL SDR");
	// 		}
	// 	}
	// 	break;
	// 	default:
	// 		break;
	// 	}

	// 	// as long as changing the resolution or changing the input clock,	You need to configure the following registers.
	// 	HDMI_WriteI2C_Byte(0xff, 0x81);
	// 	read_val = HDMI_ReadI2C_Byte(0x2b);
	// 	HDMI_WriteI2C_Byte(0x2b, read_val & 0xfd); // sw_en_txpll_cal_en
	// 	read_val = HDMI_ReadI2C_Byte(0x2e);
	// 	HDMI_WriteI2C_Byte(0x2e, read_val & 0xfe); // sw_en_txpll_iband_set

	// 	HDMI_WriteI2C_Byte(0xff, 0x82);
	// 	HDMI_WriteI2C_Byte(0xde, 0x00);
	// 	HDMI_WriteI2C_Byte(0xde, 0xc0);

	// 	HDMI_WriteI2C_Byte(0xff, 0x80);
	// 	HDMI_WriteI2C_Byte(0x16, 0xf1);
	// 	HDMI_WriteI2C_Byte(0x18, 0xdc); // txpll _sw_rst_n
	// 	HDMI_WriteI2C_Byte(0x18, 0xfc);
	// 	HDMI_WriteI2C_Byte(0x16, 0xf3);
	// 	for (j = 0; j < 0x05; j++)
	// 	{
	// 		Delay_ms(10);
	// 		HDMI_WriteI2C_Byte(0xff, 0x80);
	// 		HDMI_WriteI2C_Byte(0x16, 0xe3); /* pll lock logic reset */
	// 		HDMI_WriteI2C_Byte(0x16, 0xf3);

	// 		HDMI_WriteI2C_Byte(0xff, 0x82);
	// 		lock = 0x80 & HDMI_ReadI2C_Byte(0x15);
	// 		cali_val = HDMI_ReadI2C_Byte(0xea);
	// 		cali_done = 0x80 & HDMI_ReadI2C_Byte(0xeb);
	// 		if (lock && cali_done && (cali_val != 0xff))
	// 		{
	// #ifdef _DEBUG_MODE
	// 			HDMI_WriteI2C_Byte(0xff, 0x82);
	// 			printf("\r\n0x82ea=%bx", HDMI_ReadI2C_Byte(0xea));
	// 			printf("\r\n0x82eb=%bx", HDMI_ReadI2C_Byte(0xeb));
	// 			printf("\r\n0x82ec=%bx", HDMI_ReadI2C_Byte(0xec));
	// 			printf("\r\n0x82ed=%bx", HDMI_ReadI2C_Byte(0xed));
	// 			printf("\r\n0x82ee=%bx", HDMI_ReadI2C_Byte(0xee));
	// 			printf("\r\n0x82ef=%bx", HDMI_ReadI2C_Byte(0xef));
	// #endif
	// 			printf("\r\nTXPLL Lock");
	// 			log_trace();
	// 			return 1;
	// 		}
	// 		else
	// 		{
	// 			HDMI_WriteI2C_Byte(0xff, 0x80);
	// 			HDMI_WriteI2C_Byte(0x16, 0xf1);
	// 			HDMI_WriteI2C_Byte(0x18, 0xdc); // txpll _sw_rst_n
	// 			HDMI_WriteI2C_Byte(0x18, 0xfc);
	// 			HDMI_WriteI2C_Byte(0x16, 0xf3);
	// 			printf("\r\nTXPLL Reset");
	// 		}
	// 	}
	// 	printf("\r\nTXPLL Unlock");
	// 	log_trace();
	return 0;
}

int LT8618SX_PLL_Version_U3(hdmi_lt8618sx_t *lt86x)
{
	uint8_t tmp_data;
	log_trace("%s", __FUNCTION__);
	u8 cali_done;
	u8 cali_val;
	u8 lock;
	// 	u32 dclk;
	// 	u8 read_val;
	// 	dclk = LT8618SX_CLK_Det();
	// 	if (lt86x->ddrclk)
	// 	{
	// 		dclk = dclk * 2;
	// 	}
	switch (lt86x->inmode)
	{
	case Input_RGB_12BIT:
	case Input_RGB888:
	case Input_YCbCr444:
	case Input_YCbCr422_16BIT:
	case Input_BT1120_16BIT:
	{
		HDMI_WriteI2C_Byte(0x25, 0x00); // prediv=div(n+1)
		HDMI_WriteI2C_Byte(0x2c, 0x9e);
		// 		// if(INPUT_IDCK_CLK==_Less_than_50M)
		// 		if (dclk < 50000)
		// 		{
		// 			HDMI_WriteI2C_Byte(0x2d, 0xaa); //[5:4]divx_set //[1:0]freq set
		// 			printf("\r\nPLL LOW");
		// 		}
		// 		// else if(INPUT_IDCK_CLK==_Bound_50_100M)
		// 		else if ((dclk > 50000) && (dclk < 100000))
		// 		{
		HDMI_WriteI2C_Byte(0x2d, 0x99); //[5:4] divx_set //[1:0]freq set
		// 			printf("\r\nPLL MID");
		// 		}
		// 		// else if(INPUT_IDCK_CLK==_Greater_than_100M)
		// 		else
		// 		{
		// 			HDMI_WriteI2C_Byte(0x2d, 0x88); //[5:4] divx_set //[1:0]freq set
		// 			printf("\r\nPLL HIGH");
		// 		}
		// 		HDMI_WriteI2C_Byte(0x26, 0x55);
		// 		HDMI_WriteI2C_Byte(0x27, 0x66); // phase selection for d_clk
		HDMI_WriteI2C_Byte(0x28, 0x88);

		// 		HDMI_WriteI2C_Byte(0x29, 0x04); // for U3 for U3 SDR/DDR fixed phase
	}
	break;
		// 	case Input_BT656_8BIT:
		// 	{
		// 		HDMI_WriteI2C_Byte(0xff, 0x81);
		// 		HDMI_WriteI2C_Byte(0x23, 0x40);
		// 		HDMI_WriteI2C_Byte(0x24, 0x61); // icp set
		// 		HDMI_WriteI2C_Byte(0x25, 0x00); // prediv=div(n+1)
		// 		HDMI_WriteI2C_Byte(0x2c, 0x9e);
		// 		// if(INPUT_IDCK_CLK==_Less_than_50M)
		// 		if (dclk < 50000)
		// 		{
		// 			HDMI_WriteI2C_Byte(0x2d, 0xaa); //[5:4]divx_set //[1:0]freq set
		// 			printf("\r\nPLL LOW");
		// 		}
		// 		// else if(INPUT_IDCK_CLK==_Bound_50_100M)
		// 		else if ((dclk > 50000) && (dclk < 100000))
		// 		{
		// 			HDMI_WriteI2C_Byte(0x2d, 0x99); //[5:4] divx_set //[1:0]freq set
		// 			printf("\r\nPLL MID");
		// 		}
		// 		// else if(INPUT_IDCK_CLK==_Greater_than_100M)
		// 		else
		// 		{
		// 			HDMI_WriteI2C_Byte(0x2d, 0x88); //[5:4] divx_set //[1:0]freq set
		// 			printf("\r\nPLL HIGH");
		// 		}
		// 		HDMI_WriteI2C_Byte(0x26, 0x55);
		// 		HDMI_WriteI2C_Byte(0x27, 0x66); // phase selection for d_clk
		// 		HDMI_WriteI2C_Byte(0x28, 0xa9);

		// 		HDMI_WriteI2C_Byte(0x29, 0x04); // for U3 for U3 SDR/DDR fixed phase
		// 	}
		// 	break;

	default:
		break;
	}
	// 	HDMI_WriteI2C_Byte(0xff, 0x81);
	// 	read_val = HDMI_ReadI2C_Byte(0x2b);
	// 	HDMI_WriteI2C_Byte(0x2b, read_val & 0xfd); // sw_en_txpll_cal_en
	// 	read_val = HDMI_ReadI2C_Byte(0x2e);
	// 	HDMI_WriteI2C_Byte(0x2e, read_val & 0xfe); // sw_en_txpll_iband_set

	// 	HDMI_WriteI2C_Byte(0xff, 0x82);
	// 	HDMI_WriteI2C_Byte(0xde, 0x00);
	// 	HDMI_WriteI2C_Byte(0xde, 0xc0);

	// 	HDMI_WriteI2C_Byte(0xff, 0x80);
	// 	HDMI_WriteI2C_Byte(0x16, 0xf1);
	// 	HDMI_WriteI2C_Byte(0x18, 0xdc); // txpll _sw_rst_n
	// 	HDMI_WriteI2C_Byte(0x18, 0xfc);
	// 	HDMI_WriteI2C_Byte(0x16, 0xf3);

	// 	if (lt86x->ddrclk)
	// 	{
	// 		HDMI_WriteI2C_Byte(0xff, 0x81);
	// 		HDMI_WriteI2C_Byte(0x27, 0x60); // phase selection for d_clk
	// 		HDMI_WriteI2C_Byte(0x4d, 0x05); //
	// 		HDMI_WriteI2C_Byte(0x2a, 0x10); //
	// 		HDMI_WriteI2C_Byte(0x2a, 0x30); // sync rest
	// 	}
	// 	else
	// 	{
	// 		HDMI_WriteI2C_Byte(0xff, 0x81);
	HDMI_WriteI2C_Byte(0x4d, 0x09);
	HDMI_WriteI2C_Byte(0x27, 0x66); // phase selection for d_clk
	HDMI_WriteI2C_Byte(0x2a, 0x00); //
	HDMI_WriteI2C_Byte(0x2a, 0x20); // sync rest
	// 	}
	for (int j = 0; j < 5; j++)
	{
		Delay_ms(10);
		HDMI_WriteI2C_Byte(0xff, 0x80);
		HDMI_WriteI2C_Byte(0x16, 0xf1); /* pll lock logic reset */
		HDMI_WriteI2C_Byte(0x18, 0xdc);
		HDMI_WriteI2C_Byte(0x18, 0xfc);
		HDMI_WriteI2C_Byte(0x16, 0xf3);
		HDMI_WriteI2C_Byte(0x16, 0xe3);
		HDMI_WriteI2C_Byte(0x16, 0xf3);
		HDMI_WriteI2C_Byte(0xff, 0x82);
		lock = 0x80 & HDMI_ReadI2C_Byte(0x15);
		cali_val = HDMI_ReadI2C_Byte(0xea);
		cali_done = 0x80 & HDMI_ReadI2C_Byte(0xeb);
		if (lock && cali_done && (cali_val != 0xff))
		{
			log_info("TXPLL Lock");
			LT8618SX_CSC(lt86x); // HDMI_WriteI2C_Byte(0xb9, 0x00);
			LT8618SX_HDMI_TX_Digital(lt86x);
			LT8618SX_HDMI_TX_Phy();
			return 0;
		}
	}
	log_error("TXPLL Unlock");
	return -1;
}

int LT8618SX_PLL(hdmi_lt8618sx_t *lt86x)
{
	int ret = 0;
	uint8_t tmp_data;
	log_trace("%s", __FUNCTION__);
	tmp_data = HDMI_ReadI2C_Byte(0x2b);
	tmp_data = tmp_data & 0xfd;
	HDMI_WriteI2C_Byte(0x2b, tmp_data);

	tmp_data = HDMI_ReadI2C_Byte(0x2e);
	tmp_data = tmp_data & 0xfe;
	HDMI_WriteI2C_Byte(0x2e, tmp_data);

	switch (lt86x->chip_id[2]) // get IC Version
	{
	case 0xe1:
	{
		ret = LT8618SX_PLL_Version_U2(lt86x);
		printf("\r\nchip u2c");
	}
	break;
	case 0xe2:
	{
		ret = LT8618SX_PLL_Version_U3(lt86x);
		printf("\r\nchip u3c");
	}
	break;
	default:
		printf("\r\nfail version=%02x", lt86x->chip_id[2]);
		break;
	}
	return ret;
}

void LT8618SX_Audio_Init(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	switch (lt86x->au_inmode)
	{
	case I2S_2CH:
	{
		// IIS Input
		log_info("IIS Input");
		HDMI_WriteI2C_Byte(0xff, 0x82);				  // register bank
		HDMI_WriteI2C_Byte(0xD6, lt86x->Tx_Out_Mode); // bit7 = 0 : DVI output; bit7 = 1: HDMI output
		HDMI_WriteI2C_Byte(0xD7, 0x04);
		HDMI_WriteI2C_Byte(0xff, 0x84); // register bank
		HDMI_WriteI2C_Byte(0x06, 0x08);
		HDMI_WriteI2C_Byte(0x07, 0x10);
		HDMI_WriteI2C_Byte(0x09, 0x00);
		HDMI_WriteI2C_Byte(0x0F, lt86x->sample_freq); // Sample_Freq
		HDMI_WriteI2C_Byte(0x34, 0xD5);				  // CTS_N / 2; 32bit(24bit)

		HDMI_WriteI2C_Byte(0x35, (u8)(lt86x->iis / 0x10000));
		HDMI_WriteI2C_Byte(0x36, (u8)((lt86x->iis & 0x00FFFF) / 0x100));
		HDMI_WriteI2C_Byte(0x37, (u8)(lt86x->iis & 0x0000FF));
		HDMI_WriteI2C_Byte(0x3C, 0x21);

		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0xde, 0x00);
		HDMI_WriteI2C_Byte(0xde, 0xc0);
		HDMI_WriteI2C_Byte(0xff, 0x81);
		HDMI_WriteI2C_Byte(0x23, 0x40);
		HDMI_WriteI2C_Byte(0x24, 0x64);
		HDMI_WriteI2C_Byte(0x26, 0x55);
		HDMI_WriteI2C_Byte(0x29, 0x04);

		HDMI_WriteI2C_Byte(0x4d, 0x00);
		HDMI_WriteI2C_Byte(0x27, 0x60);
		HDMI_WriteI2C_Byte(0x28, 0x00);
		HDMI_WriteI2C_Byte(0x25, 0x01);
		HDMI_WriteI2C_Byte(0x2c, 0x94);
		HDMI_WriteI2C_Byte(0x2d, 0x99);
	}
	break;
	case SPDIF:
	{
		log_info("Audio inut = SPDIF\n");
		HDMI_WriteI2C_Byte(0xff, 0x84);
		HDMI_WriteI2C_Byte(0x06, 0x0c);
		HDMI_WriteI2C_Byte(0x07, 0x10);
		HDMI_WriteI2C_Byte(0x34, 0xd4); // CTS_N
	}
	break;
	case AUDIO_NONE:
	{
		log_info("Audio inut = AUDIO_NONE\n");
		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0xde, 0x00);
		HDMI_WriteI2C_Byte(0xde, 0xc0);
		HDMI_WriteI2C_Byte(0xff, 0x81);
		HDMI_WriteI2C_Byte(0x23, 0x40);
		HDMI_WriteI2C_Byte(0x24, 0x64);
		HDMI_WriteI2C_Byte(0x26, 0x55);
		HDMI_WriteI2C_Byte(0x29, 0x04);
		HDMI_WriteI2C_Byte(0x4d, 0x00);
		HDMI_WriteI2C_Byte(0x27, 0x60);
		HDMI_WriteI2C_Byte(0x28, 0x00);
		HDMI_WriteI2C_Byte(0x25, 0x01);
		HDMI_WriteI2C_Byte(0x2c, 0x94);
		HDMI_WriteI2C_Byte(0x2d, 0x99);
	}
	break;
	default:
		break;
	}
}

void LT8618SX_HDMI_Out_Enable(int en)
{
	log_trace("%s", __FUNCTION__);
	HDMI_WriteI2C_Byte(0xff, 0x81);
	if (en)
	{
		HDMI_WriteI2C_Byte(0x30, 0xea);
		log_info("HDMI output Enable");
	}
	else
	{
		HDMI_WriteI2C_Byte(0x30, 0x00);
		log_info("HDMI output Disable");
	}
}

void LT8618SX_Print_Video_Inf(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	log_info("##########################LT8618SX Input Infor#####################");
	log_info("h_tal = %d", lt86x->h_tal);
	log_info("h_act = %d", lt86x->h_act);
	log_info("hfp = %d", lt86x->hfp);
	log_info("hs_width = %d", lt86x->hs_width);
	log_info("hbp = %d", lt86x->hbp);

	log_info("v_tal = %d", lt86x->v_tal);
	log_info("v_act = %d", lt86x->v_act);
	log_info("vfp = %d", lt86x->vfp);
	log_info("vs_width = %d", lt86x->vs_width);
	log_info("vbp = %d", lt86x->vbp);

	log_info("---------------------------------------------------------");
}

void LT8618SX_CSC(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	switch (lt86x->outmode)
	{
	case Output_RGB888:
	{
		lt86x->hdmi_y = 0;
		// HDMI_WriteI2C_Byte(0xff, 0x82);
		if (lt86x->inmode == Input_YCbCr444)
		{
			HDMI_WriteI2C_Byte(0xb9, 0x08); // YCbCr to RGB
		}
		else if (lt86x->inmode == Input_YCbCr422_16BIT ||
				 lt86x->inmode == Input_BT1120_16BIT ||
				 lt86x->inmode == Input_BT1120_20BIT ||
				 lt86x->inmode == Input_BT1120_24BIT ||
				 lt86x->inmode == Input_BT656_8BIT ||
				 lt86x->inmode == Input_BT656_10BIT ||
				 lt86x->inmode == Input_BT656_12BIT ||
				 lt86x->inmode == Input_BT601_8BIT)
		{
			HDMI_WriteI2C_Byte(0xb9, 0x18); // YCbCr to RGB,YCbCr 422 convert to YCbCr 444
		}
		else
		{
			HDMI_WriteI2C_Byte(0xb9, 0x00); // No csc
		}
	}
	break;
	case Output_YCbCr444:
	{
		lt86x->hdmi_y = 1;
	}
	break;
	case Output_YCbCr422:
	{
		lt86x->hdmi_y = 2;
	}
	break;
	default:
		break;
	}
}

void LT8618SX_Output_Mode(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	HDMI_WriteI2C_Byte(0xff, 0x82);
	if (lt86x->sink_edid[126] == 0x01) // set hdmi mode
	{
		HDMI_WriteI2C_Byte(0xd6, 0x8e);
		printf("\r\nHDMI Mode");
	}
	else // set dvi mode
	{
		HDMI_WriteI2C_Byte(0xd6, 0x0e);
		printf("\r\nDVI Mode");
	}
}

void LT8618SX_HDMI_TX_Digital(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	// AVI
	u8 AVI_PB0 = 0x00;
	u8 AVI_PB1 = 0x00;
	u8 AVI_PB2 = 0x00;

	AVI_PB1 = (lt86x->hdmi_y << 5) + 0x10;

	AVI_PB2 = 0x2a; // 16:9
					//	AVI_PB2 = 0x19;// 4:3

	// AVI_PB0 = ((AVI_PB1 + AVI_PB2 + lt86x->hdmi_vic) <= 0x6f) ? (0x6f - AVI_PB1 - AVI_PB2 - lt86x->hdmi_vic) : (0x16f - AVI_PB1 - AVI_PB2 - lt86x->hdmi_vic);
	AVI_PB0 = 0x31;
	AVI_PB1 = 0x10;
	lt86x->hdmi_vic = 0x04;
	HDMI_WriteI2C_Byte(0xff, 0x84);
	HDMI_WriteI2C_Byte(0x43, AVI_PB0);		   // AVI_PB0
	HDMI_WriteI2C_Byte(0x44, AVI_PB1);		   // AVI_PB1
	HDMI_WriteI2C_Byte(0x45, AVI_PB2);		   // AVI_PB2
	HDMI_WriteI2C_Byte(0x47, lt86x->hdmi_vic); // AVI_PB4

	HDMI_WriteI2C_Byte(0x10, 0x2c); // data iland
	HDMI_WriteI2C_Byte(0x12, 0x64); // act_h_blank

	// VS_IF, 4k 30hz need send VS_IF packet. Please refer to hdmi1.4 spec 8.2.3
	HDMI_WriteI2C_Byte(0x3d, 0x0a); // UD1 infoframe disable

	// AVI_audio
	HDMI_WriteI2C_Byte(0xff, 0x80);
	HDMI_WriteI2C_Byte(0x11, 0x00);
	HDMI_WriteI2C_Byte(0x13, 0xf1);
	HDMI_WriteI2C_Byte(0x13, 0xf9);
}
void LT8618SX_HDMI_TX_Phy(void)
{
	log_trace("%s", __FUNCTION__);
	LT8618SX_HDMI_Out_Enable(1);
	HDMI_WriteI2C_Byte(0x31, 0x44); // DC: 0x44, AC:0x73
	HDMI_WriteI2C_Byte(0x32, 0x4a);
	HDMI_WriteI2C_Byte(0x33, 0x0b);
	HDMI_WriteI2C_Byte(0x34, 0x00); // d0 pre emphasis
	HDMI_WriteI2C_Byte(0x35, 0x00); // d1 pre emphasis
	HDMI_WriteI2C_Byte(0x36, 0x00); // d2 pre emphasis
	HDMI_WriteI2C_Byte(0x37, 0x44);
	HDMI_WriteI2C_Byte(0x3f, 0x0f);
	HDMI_WriteI2C_Byte(0x40, 0xa0); // clk swing
	HDMI_WriteI2C_Byte(0x41, 0xa0); // d0 swing
	HDMI_WriteI2C_Byte(0x42, 0xa0); // d1 swing
	HDMI_WriteI2C_Byte(0x43, 0xa0); // d2 swing
	HDMI_WriteI2C_Byte(0x44, 0x0a);
}

u8 LT8618SX_Video_Check(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	u8 temp;
	// HDMI_WriteI2C_Byte(0xff, 0x80);
	// HDMI_WriteI2C_Byte(0x13, 0xf1); // ttl video process reset
	// HDMI_WriteI2C_Byte(0x12, 0xfb); // video check reset
	// Delay_ms(1);
	// HDMI_WriteI2C_Byte(0x12, 0xff);
	// HDMI_WriteI2C_Byte(0x13, 0xf9);

	Delay_ms(100);
	switch (lt86x->inmode)
	{
	case Input_BT601_8BIT:
	case Input_RGB888:
	case Input_YCbCr422_16BIT:
	case Input_YCbCr444:
	{
		// HDMI_WriteI2C_Byte(0xff, 0x82);
		// HDMI_WriteI2C_Byte(0x51, 0x00);
		HDMI_WriteI2C_Byte(0xff, 0x82); // video check
		temp = HDMI_ReadI2C_Byte(0x70); // hs vs polarity
		if (temp & 0x02)
		{
			printf("\r\n vs_pol is 1");
		}
		else
		{
			printf("\r\n vs_pol is 0");
		}
		if (temp & 0x01)
		{
			printf("\r\n hs_pol is 1");
		}
		else
		{
			printf("\r\n hs_pol is 0");
		}

		lt86x->vs_width = HDMI_ReadI2C_Byte(0x71);
		lt86x->hs_width = HDMI_ReadI2C_Byte(0x72);
		lt86x->hs_width = ((lt86x->hs_width & 0x0f) << 8) + HDMI_ReadI2C_Byte(0x73);
		lt86x->vbp = HDMI_ReadI2C_Byte(0x74);
		lt86x->vfp = HDMI_ReadI2C_Byte(0x75);
		lt86x->hbp = HDMI_ReadI2C_Byte(0x76);
		lt86x->hbp = ((lt86x->hbp & 0x0f) << 8) + HDMI_ReadI2C_Byte(0x77);
		lt86x->hfp = HDMI_ReadI2C_Byte(0x78);
		lt86x->hfp = ((lt86x->hfp & 0x0f) << 8) + HDMI_ReadI2C_Byte(0x79);
		lt86x->v_tal = HDMI_ReadI2C_Byte(0x7a);
		lt86x->v_tal = (lt86x->v_tal << 8) + HDMI_ReadI2C_Byte(0x7b);
		lt86x->h_tal = HDMI_ReadI2C_Byte(0x7c);
		lt86x->h_tal = (lt86x->h_tal << 8) + HDMI_ReadI2C_Byte(0x7d);
		lt86x->v_act = HDMI_ReadI2C_Byte(0x7e);
		lt86x->v_act = (lt86x->v_act << 8) + HDMI_ReadI2C_Byte(0x7f);
		lt86x->h_act = HDMI_ReadI2C_Byte(0x80);
		lt86x->h_act = (lt86x->h_act << 8) + HDMI_ReadI2C_Byte(0x81);
	}
	break;
	case Input_BT1120_16BIT:
	case Input_BT656_8BIT:
	{ /*embbedded sync */
		HDMI_WriteI2C_Byte(0xff, 0x82);
		HDMI_WriteI2C_Byte(0x51, 0x42);
		lt86x->v_act = HDMI_ReadI2C_Byte(0x8b);
		lt86x->v_act = (lt86x->v_act << 8) + HDMI_ReadI2C_Byte(0x8c);

		lt86x->h_act = HDMI_ReadI2C_Byte(0x8d);
		lt86x->h_act = (lt86x->h_act << 8) + HDMI_ReadI2C_Byte(0x8e) - 0x04; /*note -0x04*/

		lt86x->h_tal = HDMI_ReadI2C_Byte(0x8f);
		lt86x->h_tal = (lt86x->h_tal << 8) + HDMI_ReadI2C_Byte(0x90);
	}
	break;
	default:
		break;
	}

	if (lt86x->inmode == Input_BT601_8BIT || lt86x->inmode == Input_BT656_8BIT)
	{
		lt86x->hs_width /= 2;
		lt86x->hbp /= 2;
		lt86x->hfp /= 2;
		lt86x->h_tal /= 2;
		lt86x->h_act /= 2;
	}

	LT8618SX_Print_Video_Inf(lt86x);

	if ((lt86x->h_act == video_640x480_60Hz.hact) &&
		(lt86x->v_act == video_640x480_60Hz.vact) &&
		(lt86x->h_tal == video_640x480_60Hz.htotal))
	{
		log_info("Video_Check = video_640x480_60Hz ");
		lt86x->video_format = video_640x480_60Hz_vic1;
		lt86x->hdmi_vic = 1;
		lt86x->video_bt = video_640x480_60Hz;
	}
	else if ((lt86x->h_act == video_720x480_60Hz.hact) &&
			 (lt86x->v_act == video_720x480_60Hz.vact) &&
			 (lt86x->h_tal == video_720x480_60Hz.htotal))
	{
		log_info("Video_Check = video_720x480_60Hz ");
		lt86x->video_format = video_720x480_60Hz_vic3;
		lt86x->hdmi_vic = 3;
		lt86x->video_bt = video_720x480_60Hz;
	}
	else if ((lt86x->h_act == video_1280x720_30Hz.hact) &&
			 (lt86x->v_act == video_1280x720_30Hz.vact) &&
			 (lt86x->h_tal == video_1280x720_30Hz.htotal))
	{
		log_info("Video_Check = video_1280x720_30Hz ");
		lt86x->video_format = video_1280x720_30Hz_vic62;
		lt86x->hdmi_vic = 0;
		lt86x->video_bt = video_1280x720_30Hz;
	}
	else if ((lt86x->h_act == video_1280x720_60Hz.hact) &&
			 (lt86x->v_act == video_1280x720_60Hz.vact) &&
			 (lt86x->h_tal == video_1280x720_60Hz.htotal))
	{
		log_info("Video_Check = video_1280x720_60Hz \r\n");
		lt86x->video_format = video_1280x720_60Hz_vic4;
		lt86x->hdmi_vic = 4;
		lt86x->video_bt = video_1280x720_60Hz;
	}
	else if ((lt86x->h_act == video_1920x1080_30Hz.hact) &&
			 (lt86x->v_act == video_1920x1080_30Hz.vact) &&
			 (lt86x->h_tal == video_1920x1080_30Hz.htotal))
	{
		log_info("Video_Check = video_1920x1080_30Hz \r\n");
		lt86x->video_format = video_1920x1080_30Hz_vic34;
		lt86x->hdmi_vic = 34;
		lt86x->video_bt = video_1920x1080_30Hz;
	}
	else if ((lt86x->h_act == video_1920x1080_50Hz.hact) &&
			 (lt86x->v_act == video_1920x1080_50Hz.vact) &&
			 (lt86x->h_tal == video_1920x1080_50Hz.htotal))
	{
		log_info("Video_Check = video_1920x1080_50Hz ");
		lt86x->video_format = video_1920x1080_50Hz_vic31;
		lt86x->hdmi_vic = 31;
		lt86x->video_bt = video_1920x1080_50Hz;
	}
	else if ((lt86x->h_act == video_1920x1080_60Hz.hact) &&
			 (lt86x->v_act == video_1920x1080_60Hz.vact) &&
			 (lt86x->h_tal == video_1920x1080_60Hz.htotal))
	{
		log_info("Video_Check = video_1920x1080_60Hz ");
		lt86x->video_format = video_1920x1080_60Hz_vic16;
		lt86x->hdmi_vic = 16;
		lt86x->video_bt = video_1920x1080_60Hz;
	}
	else if ((lt86x->h_act == video_3840x2160_30Hz.hact) &&
			 (lt86x->v_act == video_3840x2160_30Hz.vact) &&
			 (lt86x->h_tal == video_3840x2160_30Hz.htotal))
	{
		log_info("Video_Check = video_3840x2160_30Hz ");
		lt86x->video_format = video_3840x2160_30Hz_vic205;
		lt86x->hdmi_vic = 16;
		lt86x->video_bt = video_3840x2160_30Hz;
	}
	else
	{
		lt86x->video_format = video_720x480_60Hz_vic3;
		lt86x->hdmi_vic = 3;
		lt86x->video_bt = video_720x480_60Hz;
	}
	return lt86x->video_format;
}

int LT8618SX_Init(hdmi_lt8618sx_t *lt86x)
{
	log_trace("%s", __FUNCTION__);
	hdmi_lt8618sx_init(lt86x);
	lt86x->inmode = Input_RGB888;
	lt86x->outmode = Output_RGB888;
	lt86x->Tx_Out_Mode = HDMI_OUT;
	lt86x->sample_freq = Sample_Freq_48KHz;
	lt86x->iis = IIS_48KHz;
	lt86x->au_inmode = I2S_2CH;
	RESET_Lt8618SX();
	LT8618SX_Chip_ID(lt86x);
	if ((!(lt86x->chip_id[0] != lt86x->chip_id[1]) &&
		 (lt86x->chip_id[1] != lt86x->chip_id[2]) &&
		 (lt86x->chip_id[2] != lt86x->chip_id[0])))
	{
		log_error("not prod device!");
		return -1;
	}
	LT8618SX_HDMI_Out_Enable(0);
	LT8618SX_TTL_Input_Analog();
	LT8618SX_RST_PD_Init();
	LT8618SX_TTL_Input_Digital(lt86x);
	LT8618SX_Audio_Init(lt86x);
	LT8618SX_PLL(lt86x);
	LT8618SX_Video_Check(lt86x);
	return 0;
}
