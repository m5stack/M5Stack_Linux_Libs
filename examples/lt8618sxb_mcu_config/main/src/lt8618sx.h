/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#ifndef _LT8618SX_H
#define _LT8618SX_H

#define USE_DDRCLK 0
#define _DEBUG_MODE
// #define _HDCP
// #define _READ_EDID
// #define _PATTERN_TEST

#include"lttype.h"
enum
{
	_Less_than_50M = 0x00,
	_Bound_50_100M,
	_Greater_than_100M
};

#define INPUT_IDCK_CLK _Greater_than_100M

enum TX_OUT_MODE
{
	DVI_OUT = 0x00,
	HDMI_OUT = 0x8e
};

enum LT8618SX_INPUTMODE_ENUM
{
	Input_RGB888, // yes
	Input_RGB_12BIT,
	Input_RGB565,
	Input_YCbCr444,		  // yes
	Input_YCbCr422_16BIT, // yes
	Input_YCbCr422_20BIT, // no
	Input_YCbCr422_24BIT, // no
	Input_BT1120_16BIT,	  // ok
	Input_BT1120_20BIT,	  // no
	Input_BT1120_24BIT,	  // no
	Input_BT656_8BIT,	  // OK
	Input_BT656_10BIT,	  // no use
	Input_BT656_12BIT,	  // no use
	Input_BT601_8BIT	  // OK
};

enum AUDIO_INPUTMODE_ENUM
{
	I2S_2CH,
	I2S_8CH,
	SPDIF,
	AUDIO_NONE
};

typedef enum VIDEO_OUTPUTMODE_ENUM
{
	Output_RGB888,
	Output_YCbCr444,
	Output_YCbCr422,
	Output_YCbCr422_16BIT,
	Output_YCbCr422_20BIT,
	Output_YCbCr422_24BIT
} _Video_Output_Mode;


struct video_timing
{
	u16 hfp;
	u16 hs;
	u16 hbp;
	u16 hact;
	u16 htotal;
	u16 vfp;
	u16 vs;
	u16 vbp;
	u16 vact;
	u16 vtotal;
	u8 vic;
	u8 vpol;
	u8 hpol;
};

enum VideoFormat
{
	video_none,
	video_720x480i_60Hz_43 = 6,
	video_720x480i_60Hz_169 = 7,
	video_1920x1080i_60Hz_169 = 5,

	video_640x480_60Hz_vic1,
	video_720x480_60Hz_vic3,

	video_1280x720_24Hz_vic60,
	video_1280x720_25Hz_vic61,
	video_1280x720_30Hz_vic62,
	video_1280x720_50Hz_vic19,
	video_1280x720_60Hz_vic4,

	video_720x240P_60Hz_43 = 8,
	video_720x240P_60Hz_169 = 9,

	video_1920x1080_30Hz_vic34,
	video_1920x1080_50Hz_vic31,
	video_1920x1080_60Hz_vic16,

	video_3840x2160_24Hz_vic207,
	video_3840x2160_25Hz_vic206,
	video_3840x2160_30Hz_vic205
};


enum Sample_Freq
{
	Sample_Freq_32KHz = 0x30,
	Sample_Freq_44d1KHz = 0x00,
	// Sample_Freq_48KHz = 0x20,
	Sample_Freq_48KHz = 0x2b,
	Sample_Freq_88d2KHz = 0x80,
	Sample_Freq_96KHz = 0xa0,
	Sample_Freq_176Khz = 0xc0,
	Sample_Freq_196KHz = 0xe0
};
enum IIS
{
	IIS_32KHz = 4096,
	IIS_44d1KHz = 6272,
	IIS_48KHz = 6144,
	IIS_88d2KHz = 12544,
	IIS_96KHz = 12288,
	IIS_176Khz = 25088,
	IIS_196KHz = 24576
};

typedef struct HDMI_LT8618SX
{
	u16 hfp;
	u16 hs_width;
	u16 hbp;
	u16 h_act;
	u16 h_tal;
	u16 vfp;
	u16 vs_width;
	u16 vbp;
	u16 v_act;
	u16 v_tal;
	u16 v_pol;
	u16 h_pol;
	struct video_timing video;
	enum VideoFormat video_format;
	struct video_timing video_bt;
	enum LT8618SX_INPUTMODE_ENUM inmode;
	int outmode;
	int au_inmode;
	int ddrclk;
	int hdcp_en;
	u8 hdmi_y;
	u8 hdmi_vic;
	u8 tx_hpd;
	u8 vid_chk_flag;
	u8 irq_flag0;
	u8 irq_flag1;
	u8 irq_flag3;
	u8 sink_edid[256];
	u8 phease_offset;
	u8 chip_id[3];
	enum TX_OUT_MODE Tx_Out_Mode;
	enum Sample_Freq sample_freq;
	enum IIS iis;
} hdmi_lt8618sx_t;












void hdmi_lt8618sx_init(hdmi_lt8618sx_t *lt86x);
void LT8618SX_Chip_ID(hdmi_lt8618sx_t *lt86x);
void LT8618SX_RST_PD_Init(void);
void LT8618SX_TTL_Input_Analog(void);
void LT8618SX_TTL_Input_Digital(hdmi_lt8618sx_t *lt86x);
int LT8618SX_PLL_Version_U2(hdmi_lt8618sx_t *lt86x);
int LT8618SX_PLL_Version_U3(hdmi_lt8618sx_t *lt86x);
int LT8618SX_PLL(hdmi_lt8618sx_t *lt86x);
void LT8618SX_Audio_Init(hdmi_lt8618sx_t *lt86x);
void LT8618SX_HDMI_Out_Enable(int en);
void LT8618SX_Print_Video_Inf(hdmi_lt8618sx_t *lt86x);
void LT8618SX_CSC(hdmi_lt8618sx_t *lt86x);
void LT8618SX_Output_Mode(hdmi_lt8618sx_t *lt86x);
void LT8618SX_HDMI_TX_Digital(hdmi_lt8618sx_t *lt86x);
void LT8618SX_HDMI_TX_Phy(void);
u8 LT8618SX_Video_Check(hdmi_lt8618sx_t *lt86x);
int LT8618SX_Init(hdmi_lt8618sx_t *lt86x);



#endif
