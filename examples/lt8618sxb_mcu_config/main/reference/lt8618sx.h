#ifndef _LT8618SX_H
#define _LT8618SX_H

#define USE_DDRCLK  0
#define  _DEBUG_MODE
//#define _HDCP
//#define _READ_EDID
//#define _PATTERN_TEST

enum
{
	_Less_than_50M = 0x00,
	_Bound_50_100M,
	_Greater_than_100M
};

#define INPUT_IDCK_CLK _Greater_than_100M

typedef enum TX_OUT_MODE{
	DVI_OUT=0x00,
  HDMI_OUT=0x80
};
#define Tx_Out_Mode   HDMI_OUT

typedef enum LT8618SX_INPUTMODE_ENUM
{
    Input_RGB888,//yes
    Input_RGB_12BIT,
    Input_RGB565,
    Input_YCbCr444,//yes
    Input_YCbCr422_16BIT,//yes
    Input_YCbCr422_20BIT,//no
    Input_YCbCr422_24BIT,//no
    Input_BT1120_16BIT,//ok
    Input_BT1120_20BIT,//no
    Input_BT1120_24BIT,//no
    Input_BT656_8BIT,//OK
    Input_BT656_10BIT,//no use
    Input_BT656_12BIT,//no use
	  Input_BT601_8BIT//OK
};
#define Video_Input_Mode    Input_RGB888



typedef enum AUDIO_INPUTMODE_ENUM
{
    I2S_2CH,
	  I2S_8CH,
	  SPDIF
};
#define Audio_Input_Mode I2S_2CH

typedef enum VIDEO_OUTPUTMODE_ENUM
{
    Output_RGB888,
    Output_YCbCr444,
    Output_YCbCr422,
    Output_YCbCr422_16BIT,
    Output_YCbCr422_20BIT,
    Output_YCbCr422_24BIT
}
_Video_Output_Mode;

#define Video_Output_Mode  Output_RGB888

typedef struct video_timing{
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
	  u8  vic;
	  u8  vpol;
	  u8  hpol;
};

enum VideoFormat
{
    video_none,
    video_720x480i_60Hz_43=6,     
    video_720x480i_60Hz_169=7,    
	  video_1920x1080i_60Hz_169=5,  
	
    video_640x480_60Hz_vic1,      
    video_720x480_60Hz_vic3,     
	
	  video_1280x720_24Hz_vic60,
    video_1280x720_25Hz_vic61,   
	  video_1280x720_30Hz_vic62,
	  video_1280x720_50Hz_vic19,
    video_1280x720_60Hz_vic4,   

    video_720x240P_60Hz_43=8,    
    video_720x240P_60Hz_169=9 ,   
	
	  video_1920x1080_30Hz_vic34,
	  video_1920x1080_50Hz_vic31,
    video_1920x1080_60Hz_vic16,    
	
	  video_3840x2160_24Hz_vic207,
	  video_3840x2160_25Hz_vic206,
	  video_3840x2160_30Hz_vic205
};

void LT8618SX_Init(void);
void RESET_Lt8618SX(void);
void LT8618SX_Chip_ID(void);
void LT8618SX_RST_PD_Init(void);
void LT8618SX_TTL_Input_Analog(void);
bool LT8618SX_PLL(void);
bool LT8618SX_PLL_Version_U2(void);
bool LT8618SX_PLL_Version_U3(void);
void LT8618SX_Audio_Init(void);
void LT8618SX_IRQ_Init(void);
u32 LT8618SX_CLK_Det(void);
void IrqInit(void);
void LT8618SX_Task(void);
bool LT8618SX_Phase_Config(void);
void LT8618SX_HDCP_Disable(void);
void LT8618SX_HDCP_Enable(bool HDCP_EN);
void LT8618SX_HDCP_Init(void);
void LT8618SX_test_clk(void);
void LT8618sx_System_Init(void);
void LT8618SX_INTB(void);

u8 LT8618SX_Phase(void);

#endif
