/******************************************************************************
  * @project: LT8619C
  * @file: lt8618sx.c
  * @author: zll
  * @company: LONTIUM COPYRIGHT and CONFIDENTIAL
  * @date: 2018.5.30
/******************************************************************************/

#include   "include.h"

bool Tx_HPD=0;
bool vid_chk_flag=1;
bool intb_flag=0;
	u8 irq_flag3=0;
	u8 irq_flag1=0;
	u8 irq_flag0=0;
	
enum VideoFormat Video_Format;
u8 HDMI_VIC=0x00;
u8 HDMI_Y=0x00;
struct video_timing video_bt={0};

u8 Sink_EDID[256];

u16 hfp,hs_width,hbp,h_act,h_tal,vfp,vs_width,vbp,v_act,v_tal,v_pol,h_pol;

u8 phease_offset=0;
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
 
enum {
	_32KHz = 0,
	_44d1KHz,
	_48KHz,

	_88d2KHz,
	_96KHz,
	_176Khz,
	_196KHz
};

code u16 IIS_N[] =
{
	4096,   // 32K
	6272,   // 44.1K
	6144,   // 48K
	12544,  // 88.2K
	12288,  // 96K
	25088,  // 176K
	24576   // 196K
};

code u16 Sample_Freq[] =
{
	0x30,   // 32K
	0x00,   // 44.1K
	0x20,   // 48K
	0x80,   // 88.2K
	0xa0,   // 96K
	0xc0,   // 176K
	0xe0    //  196K
};


enum {
	_16bit = 1,
	_20bit,
	_24_32bit
};

#define Sampling_rate	_48KHz
#define Sampling_Size	_24_32bit

// hfp, hs, hbp,hact,htotal,vfp, vs, vbp,vact,vtotal,vic,vpol,hpol,
const struct video_timing video_640x480_60Hz     = { 16, 96,  48, 640,   800, 10,  2,  33, 480,   525,};
const struct video_timing video_720x480_60Hz     = {16, 62,  60, 720,   858,  9,  6,  30, 480,   525};
const struct video_timing video_800x600_60Hz     = {16, 62,  60, 800,   1056,  9,  6,  30, 600,   628};
const struct video_timing video_1280x720_60Hz    = {110,40, 220,1280,  1650,  6,  5,  19, 720,   750};
const struct video_timing video_1280x720_30Hz   = {736, 40, 1244,1280,  3300, 5,  5,  20, 720,   750};
const struct video_timing video_1920x1080_60Hz   = {88,44, 148,1920,  2200,  4,  5, 36, 1080,1125};
const struct video_timing video_1920x1080_50Hz   = {528,44, 148,1920,  2640,  4,  5, 36, 1080,1125};
const struct video_timing video_1920x1080_30Hz   = {528,44, 148,1920,  2640,  4,  5, 36, 1080,1125};
const struct video_timing video_1280x1024_60Hz   = {48,112, 248,1280,  1688,  1,  3, 38, 1024, 1066};
const struct video_timing video_1024x768_60Hz   = {24,136, 160,1024, 1344,  3,  6, 29, 768, 806};
//const struct video_timing video_3840x2160_24Hz   = {1276,88, 296,3840,  5500,  8,  10, 72, 2160, 2250};
//const struct video_timing video_3840x2160_25Hz   = {1056,88, 296,3840,  5280,  8,  10, 72, 2160, 2250};
const struct video_timing video_3840x2160_30Hz   = {176,88, 296,3840,  4400,  8,  10, 72, 2160, 2250};




void RESET_Lt8618SX(void)
{
    clr_P03;
    Delay_ms(50);
    set_P03;  
    Delay_ms(100);
}

void LT8618SX_Chip_ID(void)
{
    HDMI_WriteI2C_Byte(0xFF,0x80);
    HDMI_WriteI2C_Byte(0xee,0x01);
    Debug_DispStrNum("\r\nLT8618SX Chip ID = ",HDMI_ReadI2C_Byte(0x00));
    Debug_DispStrNum(" ",HDMI_ReadI2C_Byte(0x01));
	  Debug_DispStrNum(" ",HDMI_ReadI2C_Byte(0x02));
}

void LT8618SX_RST_PD_Init(void)
{
		HDMI_WriteI2C_Byte(0xff,0x80);
		HDMI_WriteI2C_Byte(0x11,0x00); //reset MIPI Rx logic
		HDMI_WriteI2C_Byte(0x13,0xf1);
		HDMI_WriteI2C_Byte(0x13,0xf9); //reset TTL video process
}

void LT8618SX_TTL_Input_Analog(void)
{
	u8 tmpreg=0;
	//TTL mode
	HDMI_WriteI2C_Byte(0xff,0x81);
	HDMI_WriteI2C_Byte(0x02,0x66);
	HDMI_WriteI2C_Byte(0x0a,0x06);//0x06
	HDMI_WriteI2C_Byte(0x15,0x06);
	HDMI_WriteI2C_Byte(0x4e,0x00);//
	
	HDMI_WriteI2C_Byte( 0xff,0x82);// ring
	HDMI_WriteI2C_Byte( 0x1b,0x77);
	HDMI_WriteI2C_Byte( 0x1c,0xec); 


}

void LT8618SX_TTL_Input_Digital(void)
{
	//TTL mode
	if(Video_Input_Mode == Input_RGB888)
	{
			Debug_Printf("\r\nVideo_Input_Mode=Input_RGB888");
			HDMI_WriteI2C_Byte(0xff,0x82);
			HDMI_WriteI2C_Byte(0x45,0x00); //RGB channel swap
			if(USE_DDRCLK == 1)
			{
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x80;  0xc0: invert dclk 
			}
			else
			{
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x00;  0x40: invert dclk 
			}
			HDMI_WriteI2C_Byte(0x50,0x00);
			HDMI_WriteI2C_Byte(0x51,0x00);
	}
	else if(Video_Input_Mode == Input_RGB_12BIT)
	{
			Debug_Printf("\r\nVideo_Input_Mode=Input_RGB_12BIT");
		  HDMI_WriteI2C_Byte(0xff,0x80);
			HDMI_WriteI2C_Byte(0x0a,0x80);
		
			HDMI_WriteI2C_Byte(0xff,0x82);
			HDMI_WriteI2C_Byte(0x45,0x70); //RGB channel swap
			
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x00;  0x40: invert dclk 
			
			HDMI_WriteI2C_Byte(0x50,0x00);
			HDMI_WriteI2C_Byte(0x51,0x30);
			HDMI_WriteI2C_Byte(0x40,0x00);
			HDMI_WriteI2C_Byte(0x41,0xcd);
	}
	else if(Video_Input_Mode == Input_YCbCr444)
	{
			Debug_Printf("\r\nVideo_Input_Mode=Input_YCbCr444");
			HDMI_WriteI2C_Byte(0xff,0x82);
			HDMI_WriteI2C_Byte(0x45,0x70); //RGB channel swap
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x00;  0x40: dclk 
	}
	else if(Video_Input_Mode==Input_YCbCr422_16BIT)
	{
			Debug_Printf("\r\nVideo_Input_Mode=Input_YCbCr422_16BIT");
			HDMI_WriteI2C_Byte(0xff,0x82);
			HDMI_WriteI2C_Byte(0x45,0x00); //RGB channel swap
			if(USE_DDRCLK == 1)
			{
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x80;  0xc0: invert dclk 
			}
			else
			{
			HDMI_WriteI2C_Byte(0x4f,0x00); //0x00;  0x40: invert dclk 
			}
	}
	
	else if(Video_Input_Mode==Input_BT1120_16BIT)
	{
			Debug_Printf("\r\nVideo_Input_Mode=Input_BT1120_16BIT");
			HDMI_WriteI2C_Byte(0xff,0x82);
			//HDMI_WriteI2C_Byte(0x45,0x30); // D0 ~ D7 Y ; D8 ~ D15 C
			HDMI_WriteI2C_Byte(0x45,0x70); // D8 ~ D15 Y ; D16 ~ D23 C
			//HDMI_WriteI2C_Byte(0x45,0x00); // D0 ~ D7 C ; D8 ~ D15 Y
			//HDMI_WriteI2C_Byte(0x45,0x60); // D8 ~ D15 C ; D16 ~ D23 Y
			if(USE_DDRCLK == 1)
			{
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x80;  0xc0: invert dclk 
			}
			else
			{
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x00;  0x40: invert dclk 
			}
			HDMI_WriteI2C_Byte(0x48,0x08); //0x1c  0x08 Embedded sync mode input enable.
			HDMI_WriteI2C_Byte(0x51,0x42); //0x02 0x43 0x34 embedded DE mode input edable.	
		  HDMI_WriteI2C_Byte(0x47,0x37);
	}
	else if(Video_Input_Mode==Input_BT656_8BIT)
	{
			Debug_Printf("\r\nVideo_Input_Mode=Input_BT656_8BIT");
			HDMI_WriteI2C_Byte(0xff,0x82);
			HDMI_WriteI2C_Byte(0x45,0x00); //RGB channel swap
			if(USE_DDRCLK == 1)
			{
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x80;  0xc0: dclk
			HDMI_WriteI2C_Byte(0x48,0x5c);//0x50 0x5c 0x40 0x48 
			}
			else
			{
			HDMI_WriteI2C_Byte(0x4f,0x40); //0x00;  0x40: dclk 
			HDMI_WriteI2C_Byte(0x48,0x48);
			}
			HDMI_WriteI2C_Byte(0x51,0x42); 
			HDMI_WriteI2C_Byte(0x47,0x87); 
	}		
	else if(Video_Input_Mode==Input_BT601_8BIT)
	{
			Debug_Printf("\r\nVideo_Input_Mode=Input_BT601_8BIT");
		  HDMI_WriteI2C_Byte(0xff,0x81);
			HDMI_WriteI2C_Byte(0x0a,0x90);
		
		  HDMI_WriteI2C_Byte(0xff,0x81);
			HDMI_WriteI2C_Byte(0x4e,0x02);
		
			HDMI_WriteI2C_Byte(0xff,0x82);
			HDMI_WriteI2C_Byte(0x45,0x00); //RGB channel swap
			if(USE_DDRCLK == 1)
			{
			HDMI_WriteI2C_Byte(0x4f,0xc0); //0x80;  0xc0: dclk
			HDMI_WriteI2C_Byte(0x48,0x5c);//0x50 0x5c 0x40 0x48 
			}
			else
			{
			HDMI_WriteI2C_Byte(0x4f,0xc0); //0x00;  0x40: dclk 
			HDMI_WriteI2C_Byte(0x48,0x40);
			}
			HDMI_WriteI2C_Byte(0x51,0x00); 
			HDMI_WriteI2C_Byte(0x47,0x87); 
	}		
}
bool LT8618SX_PLL(void)
{
	u8 read_val=0;
	HDMI_WriteI2C_Byte(0xff,0x80);
	read_val=HDMI_ReadI2C_Byte(0x02);//get IC Version
  if(read_val==0xe1)
	{
	 LT8618SX_PLL_Version_U2();
	 printf("\r\nchip u2c");
	}
	else if(read_val==0xe2)
	{
	 LT8618SX_PLL_Version_U3();
	 printf("\r\nchip u3c");
	}
	else
	{
	 printf("\r\nfail version=%x",read_val);
	}
}
#if 0
bool LT8618SX_PLL_Version_U2(void)
{
	  u8 read_val;
	  u8 j;
	  u8 cali_done;
	  u8 cali_val;
	  u8 lock;
	  u32 dclk;
	
	  dclk=LT8618SX_CLK_Det();
	
    if(Video_Input_Mode==Input_RGB888 || Video_Input_Mode==Input_YCbCr444||Video_Input_Mode==Input_YCbCr422_16BIT||Video_Input_Mode==Input_BT1120_16BIT)
			{
			HDMI_WriteI2C_Byte( 0xff, 0x81 );
			HDMI_WriteI2C_Byte( 0x23, 0x40 );
			HDMI_WriteI2C_Byte( 0x24, 0x62 );               //icp set
			HDMI_WriteI2C_Byte( 0x26, 0x55 );

			if(dclk<=25000)
			{
				HDMI_WriteI2C_Byte( 0x25, 0x00 );
				//HDMI_WriteI2C_Byte( 0x2c, 0xA8 );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
				HDMI_WriteI2C_Byte( 0x2d, 0xaa );
			}
			else if((dclk>25000)&&(dclk<=50000))
			{
				HDMI_WriteI2C_Byte( 0x25, 0x00 );
				HDMI_WriteI2C_Byte( 0x2d, 0xaa );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
			}
			else if((dclk>50000)&&(dclk<=100000))
			{
				HDMI_WriteI2C_Byte( 0x25, 0x01 );
				HDMI_WriteI2C_Byte( 0x2d, 0x99 );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
				printf("\r\n50~100m");
			}

			else //if(dclk>100000)
			{
				HDMI_WriteI2C_Byte( 0x25, 0x03 );
				HDMI_WriteI2C_Byte( 0x2d, 0x88 );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
			}

			if( USE_DDRCLK )
			{
				read_val=HDMI_ReadI2C_Byte(0x2c) &0x7f;
				read_val=read_val*2|0x80;
				HDMI_WriteI2C_Byte( 0x2c, read_val );
				printf("\r\n0x812c=%x",(u16)read_val);
				HDMI_WriteI2C_Byte( 0x4d, 0x05 );
				HDMI_WriteI2C_Byte( 0x27, 0x66 );                                               //0x60 //ddr 0x66
				HDMI_WriteI2C_Byte( 0x28, 0x88 ); 
				Debug_Printf("\r\n PLL DDR" );				 
			}
			else
			{
				HDMI_WriteI2C_Byte( 0x4d, 0x01 );
				HDMI_WriteI2C_Byte( 0x27, 0x60 ); //0x06                                              //0x60 //ddr 0x66
				HDMI_WriteI2C_Byte( 0x28, 0x88 );                                               // 0x88
				Debug_Printf("\r\n PLL SDR" );
			}
			}
	
    else if(Video_Input_Mode==Input_BT601_8BIT||Video_Input_Mode==Input_BT656_8BIT||Video_Input_Mode==Input_BT656_10BIT||Video_Input_Mode==Input_BT656_12BIT)
				{
				Debug_Printf("\r\nTx_PLL==Input_BT656");
				HDMI_WriteI2C_Byte(0xff,0x81);	
				HDMI_WriteI2C_Byte(0x23,0x40);
				HDMI_WriteI2C_Byte(0x24,0x64); //icp set
				HDMI_WriteI2C_Byte(0x26,0x55);
				HDMI_WriteI2C_Byte(0x4d,0x04);
        if(dclk<=50000)
			{
				HDMI_WriteI2C_Byte( 0x25, 0x00 );
				HDMI_WriteI2C_Byte( 0x2c, 0xab );
				HDMI_WriteI2C_Byte( 0x2d, 0x94 );
			}
			else if((dclk>50000)&&(dclk<=100000))
			{
				HDMI_WriteI2C_Byte( 0x25, 0x01 );
				HDMI_WriteI2C_Byte( 0x2d, 0x9a );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
				printf("\r\n50~100m");
			}

			else //if(dclk>100000)
			{
				HDMI_WriteI2C_Byte( 0x25, 0x03 );
				HDMI_WriteI2C_Byte( 0x2d, 0x89 );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
			}
				if(USE_DDRCLK == 1)
				{
				Debug_Printf("\r\nTx_PLL==Input_RGB_DDR");
				HDMI_WriteI2C_Byte(0x25,0x03);////0x05 //pre-divider3 ddr 02
				HDMI_WriteI2C_Byte(0x27,0x46);//0x60 //ddr 0x66
				}
				else
				{
				//Debug_Printf("\r\nTx_PLL==Input_RGB_SDR");
				HDMI_WriteI2C_Byte(0x25,0x01);////0x05 //pre-divider3 ddr 02
				HDMI_WriteI2C_Byte(0x27,0x60);//0x60 //ddr 0x66
				}
				HDMI_WriteI2C_Byte(0x26,0x55);
				HDMI_WriteI2C_Byte(0x28,0x99);//BT656
				}
			  
  	    // as long as changing the resolution or changing the input clock,	You need to configure the following registers.
				HDMI_WriteI2C_Byte( 0xff, 0x81 );
				read_val=HDMI_ReadI2C_Byte(0x2b);
				HDMI_WriteI2C_Byte( 0x2b, read_val&0xfd );// sw_en_txpll_cal_en
				read_val=HDMI_ReadI2C_Byte(0x2e);
				HDMI_WriteI2C_Byte( 0x2e, read_val&0xfe );//sw_en_txpll_iband_set
				
				HDMI_WriteI2C_Byte( 0xff, 0x82 );
				HDMI_WriteI2C_Byte( 0xde, 0x00 );
				HDMI_WriteI2C_Byte( 0xde, 0xc0 );
				
        HDMI_WriteI2C_Byte( 0xff, 0x80 );
				HDMI_WriteI2C_Byte( 0x16, 0xf1 );
				HDMI_WriteI2C_Byte( 0x18, 0xdc );//txpll _sw_rst_n
				HDMI_WriteI2C_Byte( 0x18, 0xfc );
				HDMI_WriteI2C_Byte( 0x16, 0xf3 );	
					  for(j=0;j<0x05;j++)
				    {
									Delay_ms(10);
							    HDMI_WriteI2C_Byte(0xff,0x80);	
			            HDMI_WriteI2C_Byte(0x16,0xe3); /* pll lock logic reset */
			            HDMI_WriteI2C_Byte(0x16,0xf3);
							
							    HDMI_WriteI2C_Byte( 0xff, 0x82 );
									lock=0x80&HDMI_ReadI2C_Byte(0x15);	
							    cali_val=HDMI_ReadI2C_Byte(0xea);	
							    cali_done=0x80&HDMI_ReadI2C_Byte(0xeb);
									if(lock&&cali_done&&(cali_val!=0xff))
									{	
										#ifdef _DEBUG_MODE
                  HDMI_WriteI2C_Byte( 0xff, 0x82 );
									printf("\r\n0x82ea=%bx",HDMI_ReadI2C_Byte(0xea));	
									printf("\r\n0x82eb=%bx",HDMI_ReadI2C_Byte(0xeb));	
									printf("\r\n0x82ec=%bx",HDMI_ReadI2C_Byte(0xec));	
									printf("\r\n0x82ed=%bx",HDMI_ReadI2C_Byte(0xed));	
									printf("\r\n0x82ee=%bx",HDMI_ReadI2C_Byte(0xee));	
									printf("\r\n0x82ef=%bx",HDMI_ReadI2C_Byte(0xef));	
                    #endif										
									  printf("\r\nTXPLL Lock");
									return 1;
									}
									else
									{
									HDMI_WriteI2C_Byte( 0xff, 0x80 );
									HDMI_WriteI2C_Byte( 0x16, 0xf1 );
									HDMI_WriteI2C_Byte( 0x18, 0xdc );//txpll _sw_rst_n
									HDMI_WriteI2C_Byte( 0x18, 0xfc );
									HDMI_WriteI2C_Byte( 0x16, 0xf3 );	
									printf("\r\nTXPLL Reset");
									}
					  }
				printf("\r\nTXPLL Unlock");
				return 0;
}
#endif

bool LT8618SX_PLL_Version_U3(void)
{
	  u8 read_val;
	  u8 j;
	  u8 cali_done;
	  u8 cali_val;
	  u8 lock;
	  u32 dclk;
	
	  dclk=LT8618SX_CLK_Det();
	
	  if(USE_DDRCLK == 1)
				{
				 dclk=dclk*2;
				}
	
    if(Video_Input_Mode==Input_RGB_12BIT||Video_Input_Mode==Input_RGB888 || Video_Input_Mode==Input_YCbCr444||Video_Input_Mode==Input_YCbCr422_16BIT||Video_Input_Mode==Input_BT1120_16BIT)
    {
	      HDMI_WriteI2C_Byte( 0xff, 0x81 );
				HDMI_WriteI2C_Byte( 0x23, 0x40 );
				HDMI_WriteI2C_Byte( 0x24, 0x62 );         //0x62(u3) ,0x64 icp set
			  HDMI_WriteI2C_Byte( 0x25, 0x00 );         //prediv=div(n+1)
			   HDMI_WriteI2C_Byte( 0x2c, 0x9e );
			  //if(INPUT_IDCK_CLK==_Less_than_50M)
			  if(dclk<50000)
				{
					HDMI_WriteI2C_Byte( 0x2d, 0xaa );       //[5:4]divx_set //[1:0]freq set
				 printf("\r\nPLL LOW");
				} 
				//else if(INPUT_IDCK_CLK==_Bound_50_100M)
				else if((dclk>50000)&&(dclk<100000))
				{
				 HDMI_WriteI2C_Byte( 0x2d, 0x99 );       //[5:4] divx_set //[1:0]freq set
				 printf("\r\nPLL MID");
				}
				//else if(INPUT_IDCK_CLK==_Greater_than_100M)
				else
				{
				 HDMI_WriteI2C_Byte( 0x2d, 0x88 );       //[5:4] divx_set //[1:0]freq set
				 printf("\r\nPLL HIGH");
				}
			  HDMI_WriteI2C_Byte( 0x26, 0x55 );
			  HDMI_WriteI2C_Byte( 0x27, 0x66 );   //phase selection for d_clk
				HDMI_WriteI2C_Byte( 0x28, 0x88 );
			
			  HDMI_WriteI2C_Byte( 0x29, 0x04 );   //for U3 for U3 SDR/DDR fixed phase
			   
    }
		else if(Video_Input_Mode==Input_BT656_8BIT)
    {
      HDMI_WriteI2C_Byte( 0xff, 0x81 );
				HDMI_WriteI2C_Byte( 0x23, 0x40 );
				HDMI_WriteI2C_Byte( 0x24, 0x61 );         //icp set
			  HDMI_WriteI2C_Byte( 0x25, 0x00 );         //prediv=div(n+1)
			   HDMI_WriteI2C_Byte( 0x2c, 0x9e );
			  //if(INPUT_IDCK_CLK==_Less_than_50M)
			  if(dclk<50000)
				{
					HDMI_WriteI2C_Byte( 0x2d, 0xaa );       //[5:4]divx_set //[1:0]freq set
				 printf("\r\nPLL LOW");
				} 
				//else if(INPUT_IDCK_CLK==_Bound_50_100M)
				else if((dclk>50000)&&(dclk<100000))
				{
				 HDMI_WriteI2C_Byte( 0x2d, 0x99 );       //[5:4] divx_set //[1:0]freq set
				 printf("\r\nPLL MID");
				}
				//else if(INPUT_IDCK_CLK==_Greater_than_100M)
				else
				{
				 HDMI_WriteI2C_Byte( 0x2d, 0x88 );       //[5:4] divx_set //[1:0]freq set
				 printf("\r\nPLL HIGH");
				}
			  HDMI_WriteI2C_Byte( 0x26, 0x55 );
			  HDMI_WriteI2C_Byte( 0x27, 0x66 );   //phase selection for d_clk
				HDMI_WriteI2C_Byte( 0x28, 0xa9 );
			
			  HDMI_WriteI2C_Byte( 0x29, 0x04 );   //for U3 for U3 SDR/DDR fixed phase
    }
		
		
				HDMI_WriteI2C_Byte( 0xff, 0x81 );
				read_val=HDMI_ReadI2C_Byte(0x2b);
				HDMI_WriteI2C_Byte( 0x2b, read_val&0xfd );// sw_en_txpll_cal_en
				read_val=HDMI_ReadI2C_Byte(0x2e);
				HDMI_WriteI2C_Byte( 0x2e, read_val&0xfe );//sw_en_txpll_iband_set
				
				HDMI_WriteI2C_Byte( 0xff, 0x82 );
				HDMI_WriteI2C_Byte( 0xde, 0x00 );
				HDMI_WriteI2C_Byte( 0xde, 0xc0 );
				
        HDMI_WriteI2C_Byte( 0xff, 0x80 );
				HDMI_WriteI2C_Byte( 0x16, 0xf1 );
				HDMI_WriteI2C_Byte( 0x18, 0xdc );//txpll _sw_rst_n
				HDMI_WriteI2C_Byte( 0x18, 0xfc );
				HDMI_WriteI2C_Byte( 0x16, 0xf3 );	
		 
				if(USE_DDRCLK == 1)
				{
					HDMI_WriteI2C_Byte( 0xff, 0x81 );
					HDMI_WriteI2C_Byte( 0x27, 0x60 );   //phase selection for d_clk
					HDMI_WriteI2C_Byte( 0x4d, 0x05 );//
					HDMI_WriteI2C_Byte( 0x2a, 0x10 );//
					HDMI_WriteI2C_Byte( 0x2a, 0x30 );//sync rest
				}
				else
				{
					HDMI_WriteI2C_Byte( 0xff, 0x81 );
					HDMI_WriteI2C_Byte( 0x27, 0x66 );   //phase selection for d_clk
					HDMI_WriteI2C_Byte( 0x2a, 0x00 );//
					HDMI_WriteI2C_Byte( 0x2a, 0x20 );//sync rest
				}
					  for(j=0;j<0x05;j++)
				    {
									Delay_ms(10);
							    HDMI_WriteI2C_Byte(0xff,0x80);	
			            HDMI_WriteI2C_Byte(0x16,0xe3); /* pll lock logic reset */
			            HDMI_WriteI2C_Byte(0x16,0xf3);
							
							    HDMI_WriteI2C_Byte( 0xff, 0x82 );
									lock=0x80&HDMI_ReadI2C_Byte(0x15);	
							    cali_val=HDMI_ReadI2C_Byte(0xea);	
							    cali_done=0x80&HDMI_ReadI2C_Byte(0xeb);
									if(lock&&cali_done&&(cali_val!=0xff))
									{	
										#ifdef _DEBUG_MODE
                  HDMI_WriteI2C_Byte( 0xff, 0x82 );
										printf("\r\n0x8215=%bx",HDMI_ReadI2C_Byte(0x15));
										printf("\r\n0x82e6=%bx",HDMI_ReadI2C_Byte(0xe6));	
										printf("\r\n0x82e7=%bx",HDMI_ReadI2C_Byte(0xe7));	
										printf("\r\n0x82e8=%bx",HDMI_ReadI2C_Byte(0xe8));	
										printf("\r\n0x82e9=%bx",HDMI_ReadI2C_Byte(0xe9));	
										printf("\r\n0x82ea=%bx",HDMI_ReadI2C_Byte(0xea));	
										printf("\r\n0x82eb=%bx",HDMI_ReadI2C_Byte(0xeb));	
										printf("\r\n0x82ec=%bx",HDMI_ReadI2C_Byte(0xec));	
										printf("\r\n0x82ed=%bx",HDMI_ReadI2C_Byte(0xed));	
										printf("\r\n0x82ee=%bx",HDMI_ReadI2C_Byte(0xee));	
										printf("\r\n0x82ef=%bx",HDMI_ReadI2C_Byte(0xef));
                    #endif										
									  printf("\r\nTXPLL Lock");
										
											if(USE_DDRCLK == 1)
											{
											HDMI_WriteI2C_Byte( 0xff, 0x81 );
											HDMI_WriteI2C_Byte( 0x4d, 0x05 );//
											HDMI_WriteI2C_Byte( 0x2a, 0x10 );//
											HDMI_WriteI2C_Byte( 0x2a, 0x30 );//sync rest
											}
											else
											{
											HDMI_WriteI2C_Byte( 0xff, 0x81 );
											HDMI_WriteI2C_Byte( 0x2a, 0x00 );//
											HDMI_WriteI2C_Byte( 0x2a, 0x20 );//sync rest
											}
									return 1;
									}
									else
									{
									HDMI_WriteI2C_Byte( 0xff, 0x80 );
									HDMI_WriteI2C_Byte( 0x16, 0xf1 );
									HDMI_WriteI2C_Byte( 0x18, 0xdc );//txpll _sw_rst_n
									HDMI_WriteI2C_Byte( 0x18, 0xfc );
									HDMI_WriteI2C_Byte( 0x16, 0xf3 );	
									printf("\r\nTXPLL Reset");
									}
					  }
				printf("\r\nTXPLL Unlock");
				return 0;
}

#if 1
bool LT8618SX_PLL_Version_U2(void)
{
	  u8 read_val;
	  u8 j;
	  u8 cali_done;
	  u8 cali_val;
	  u8 lock;
	  u32 dclk;
	
	  dclk=LT8618SX_CLK_Det();
	
    if(Video_Input_Mode==Input_RGB888 || Video_Input_Mode==Input_YCbCr444||Video_Input_Mode==Input_YCbCr422_16BIT||Video_Input_Mode==Input_BT1120_16BIT)
    {
	      HDMI_WriteI2C_Byte( 0xff, 0x81 );
				HDMI_WriteI2C_Byte( 0x23, 0x40 );
				HDMI_WriteI2C_Byte( 0x24, 0x62 );               //icp set
			  HDMI_WriteI2C_Byte( 0x26, 0x55 );
	      
	     if(dclk<=25000)
			 {
				HDMI_WriteI2C_Byte( 0x25, 0x00 );
				//HDMI_WriteI2C_Byte( 0x2c, 0xA8 );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
				HDMI_WriteI2C_Byte( 0x2d, 0xaa );
			 }
	     else if((dclk>25000)&&(dclk<=50000))
			 {
				HDMI_WriteI2C_Byte( 0x25, 0x00 );
				HDMI_WriteI2C_Byte( 0x2d, 0xaa );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
			 }
	     else if((dclk>50000)&&(dclk<=100000))
			 {
				HDMI_WriteI2C_Byte( 0x25, 0x01 );
				HDMI_WriteI2C_Byte( 0x2d, 0x99 );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
				
				 printf("\r\n50~100m");
			 }
			 
			 else //if(dclk>100000)
			 {
				HDMI_WriteI2C_Byte( 0x25, 0x03 );
				HDMI_WriteI2C_Byte( 0x2d, 0x88 );
				HDMI_WriteI2C_Byte( 0x2c, 0x94 );
			 }
			 
			 if( USE_DDRCLK )
			 {
				 read_val=HDMI_ReadI2C_Byte(0x2c) &0x7f;
				 read_val=read_val*2|0x80;
				 HDMI_WriteI2C_Byte( 0x2c, read_val );
				 printf("\r\n0x812c=%x",(u16)read_val);
						HDMI_WriteI2C_Byte( 0x4d, 0x05 );
				    HDMI_WriteI2C_Byte( 0x27, 0x66 );                                               //0x60 //ddr 0x66
						HDMI_WriteI2C_Byte( 0x28, 0x88 ); 
            Debug_Printf("\r\n PLL DDR" );				 
			 }
				else
				{
						HDMI_WriteI2C_Byte( 0x4d, 0x01 );
						HDMI_WriteI2C_Byte( 0x27, 0x60 ); //0x06                                              //0x60 //ddr 0x66
						HDMI_WriteI2C_Byte( 0x28, 0x88 );                                               // 0x88
						Debug_Printf("\r\n PLL SDR" );
				}
    }
	
    else if(Video_Input_Mode==Input_BT656_8BIT||Video_Input_Mode==Input_BT656_10BIT||Video_Input_Mode==Input_BT656_12BIT)
				{
					;
				}
			  
  	    // as long as changing the resolution or changing the input clock,	You need to configure the following registers.
				HDMI_WriteI2C_Byte( 0xff, 0x81 );
				read_val=HDMI_ReadI2C_Byte(0x2b);
				HDMI_WriteI2C_Byte( 0x2b, read_val&0xfd );// sw_en_txpll_cal_en
				read_val=HDMI_ReadI2C_Byte(0x2e);
				HDMI_WriteI2C_Byte( 0x2e, read_val&0xfe );//sw_en_txpll_iband_set
				
				HDMI_WriteI2C_Byte( 0xff, 0x82 );
				HDMI_WriteI2C_Byte( 0xde, 0x00 );
				HDMI_WriteI2C_Byte( 0xde, 0xc0 );
				
        HDMI_WriteI2C_Byte( 0xff, 0x80 );
				HDMI_WriteI2C_Byte( 0x16, 0xf1 );
				HDMI_WriteI2C_Byte( 0x18, 0xdc );//txpll _sw_rst_n
				HDMI_WriteI2C_Byte( 0x18, 0xfc );
				HDMI_WriteI2C_Byte( 0x16, 0xf3 );	
					  for(j=0;j<0x05;j++)
				    {
									Delay_ms(10);
							    HDMI_WriteI2C_Byte(0xff,0x80);	
			            HDMI_WriteI2C_Byte(0x16,0xe3); /* pll lock logic reset */
			            HDMI_WriteI2C_Byte(0x16,0xf3);
							
							    HDMI_WriteI2C_Byte( 0xff, 0x82 );
									lock=0x80&HDMI_ReadI2C_Byte(0x15);	
							    cali_val=HDMI_ReadI2C_Byte(0xea);	
							    cali_done=0x80&HDMI_ReadI2C_Byte(0xeb);
									if(lock&&cali_done&&(cali_val!=0xff))
									{	
										#ifdef _DEBUG_MODE
                  HDMI_WriteI2C_Byte( 0xff, 0x82 );
									printf("\r\n0x82ea=%bx",HDMI_ReadI2C_Byte(0xea));	
									printf("\r\n0x82eb=%bx",HDMI_ReadI2C_Byte(0xeb));	
									printf("\r\n0x82ec=%bx",HDMI_ReadI2C_Byte(0xec));	
									printf("\r\n0x82ed=%bx",HDMI_ReadI2C_Byte(0xed));	
									printf("\r\n0x82ee=%bx",HDMI_ReadI2C_Byte(0xee));	
									printf("\r\n0x82ef=%bx",HDMI_ReadI2C_Byte(0xef));	
                    #endif										
									  printf("\r\nTXPLL Lock");
									return 1;
									}
									else
									{
									HDMI_WriteI2C_Byte( 0xff, 0x80 );
									HDMI_WriteI2C_Byte( 0x16, 0xf1 );
									HDMI_WriteI2C_Byte( 0x18, 0xdc );//txpll _sw_rst_n
									HDMI_WriteI2C_Byte( 0x18, 0xfc );
									HDMI_WriteI2C_Byte( 0x16, 0xf3 );	
									printf("\r\nTXPLL Reset");
									}
					  }
				printf("\r\nTXPLL Unlock");
				return 0;
}
#endif
void LT8618SX_Audio_Init(void)
{
    if(Audio_Input_Mode==I2S_2CH)
    {
       // IIS Input
				HDMI_WriteI2C_Byte( 0xff, 0x82 );   // register bank
				HDMI_WriteI2C_Byte( 0xd6, Tx_Out_Mode|0x0e );   // bit7 = 0 : DVI output; bit7 = 1: HDMI output
				HDMI_WriteI2C_Byte( 0xd7, 0x04 );   
			
				HDMI_WriteI2C_Byte( 0xff, 0x84 );   // register bank
				HDMI_WriteI2C_Byte( 0x06, 0x08 );
				HDMI_WriteI2C_Byte( 0x07, 0x10 );	
			  HDMI_WriteI2C_Byte( 0x10, 0x15 );
			  HDMI_WriteI2C_Byte( 0x12, 0x60 );

				HDMI_WriteI2C_Byte( 0x0f, 0x0b + Sample_Freq[_32KHz] );
				HDMI_WriteI2C_Byte( 0x34, 0xd4 );   //CTS_N / 2; 32bit(24bit)
			//	HDMI_WriteI2C_Byte( 0x34, 0xd5 );	//CTS_N / 4; 16bit
				HDMI_WriteI2C_Byte( 0x35, (u8)( IIS_N[_32KHz] / 0x10000 ) );
				HDMI_WriteI2C_Byte( 0x36, (u8)( ( IIS_N[_32KHz] & 0x00FFFF ) / 0x100 ) );
				HDMI_WriteI2C_Byte( 0x37, (u8)( IIS_N[_32KHz] & 0x0000FF ) );
				HDMI_WriteI2C_Byte( 0x3c, 0x21 );   // Null packet enable
    }
    else if(Audio_Input_Mode==SPDIF)///
    {
				printf("\r\nAudio inut = SPDIF");
				HDMI_WriteI2C_Byte(0xff,0x84);
				HDMI_WriteI2C_Byte(0x06,0x0c);
				HDMI_WriteI2C_Byte(0x07,0x10);	
				HDMI_WriteI2C_Byte(0x34,0xd4); //CTS_N
    } 
}

void LT8618SX_HDCP_Init( void )         //luodexing
{
	 u8 read_val=0;
	
	    HDMI_WriteI2C_Byte( 0xff, 0x80 );
	    HDMI_WriteI2C_Byte( 0x13, 0xf8 );
	    HDMI_WriteI2C_Byte( 0x13, 0xf9 );
	
	    HDMI_WriteI2C_Byte( 0xff, 0x85 );
			HDMI_WriteI2C_Byte( 0x15, 0x15 );//bit3
			HDMI_WriteI2C_Byte( 0x15, 0x05 );//0x45

	    HDMI_WriteI2C_Byte( 0xff, 0x85 );
	    HDMI_WriteI2C_Byte( 0x17, 0x0f );
	    HDMI_WriteI2C_Byte( 0x0c, 0x30 );
	   
	    HDMI_WriteI2C_Byte( 0xff, 0x82 );
	    read_val=HDMI_ReadI2C_Byte( 0xd6 );
	    printf("\r\n hdcp 0x82d6=0x%x",read_val);
	    if(read_val&0x80)//hdmi mode
			{
			HDMI_WriteI2C_Byte( 0xff, 0x85 );
			HDMI_WriteI2C_Byte( 0x13, 0x3c );//bit3
			}
			else//dvi mode
			{
			HDMI_WriteI2C_Byte( 0xff, 0x85 );
			HDMI_WriteI2C_Byte( 0x13, 0x34 );//bit3
			}
			
		HDMI_WriteI2C_Byte(0xff,0x84);
    HDMI_WriteI2C_Byte(0x10,0x2c);
	  HDMI_WriteI2C_Byte(0x12,0x64);    

}

void LT8618SX_HDCP_Enable( bool HDCP_EN )       //luodexing
{
	
	 if(HDCP_EN==Enable)
	 {
			HDMI_WriteI2C_Byte( 0xff, 0x85 );
			HDMI_WriteI2C_Byte( 0x15, 0x05 );//bit3
			HDMI_WriteI2C_Byte( 0x15, 0x65 );
			printf("\r\n hdcp en");
		 
	 }
	 else
	 {
	    HDMI_WriteI2C_Byte( 0xff, 0x85 );
			HDMI_WriteI2C_Byte( 0x15, 0x15 );//bit3
			HDMI_WriteI2C_Byte( 0x15, 0x05 );//0x45
		  HDMI_WriteI2C_Byte( 0xff, 0x80 );
			HDMI_WriteI2C_Byte( 0x13,0xf8 );
		  HDMI_WriteI2C_Byte( 0x13,0xf9 );
		  printf("\r\n hdcp dis");
	 }
}

/***********************************************************/

void LT8618SX_HDCP_Disable( void )      //luodexing
{
	HDMI_WriteI2C_Byte( 0xff, 0x85 );
	HDMI_WriteI2C_Byte( 0x15, 0x45 );   //enable HDCP
}

void LT8618SX_IRQ_Init(void)
{
		HDMI_WriteI2C_Byte(0xff,0x82);
		HDMI_WriteI2C_Byte(0x10,0x00); //Output low level active;
		HDMI_WriteI2C_Byte(0x58,0x02); //Det HPD
		
		//HDMI_WriteI2C_Byte(0x9e,0xff); //vid chk clk
		HDMI_WriteI2C_Byte(0x9e,0xf7);
		
		HDMI_WriteI2C_Byte(0x00,0xfe);   //mask0 vid_change
#ifdef _HDCP
	  HDMI_WriteI2C_Byte(0x01,0xed);   //mask1 bit1:~tx_auth_pass bit4:tx_auth_done
#endif
	  HDMI_WriteI2C_Byte(0x03,0x3f); //mask3  //tx_det
	
		HDMI_WriteI2C_Byte(0x02,0xff); //mask2
	
	
		HDMI_WriteI2C_Byte(0x04,0xff); //clear0
		HDMI_WriteI2C_Byte(0x04,0xfe); //clear0
#ifdef _HDCP
	  HDMI_WriteI2C_Byte(0x05,0xff); //clear1
		HDMI_WriteI2C_Byte(0x05,0xed); //clear1
#endif		
		HDMI_WriteI2C_Byte(0x07,0xff); //clear3
		HDMI_WriteI2C_Byte(0x07,0x3f); //clear3
}

bool LT8618SX_HPD_Status(void)
{
    HDMI_WriteI2C_Byte(0xFF,0x80);
    HDMI_WriteI2C_Byte(0xee,0x01);
    HDMI_WriteI2C_Byte(0xFF,0x81);
    HDMI_WriteI2C_Byte(0x49,HDMI_ReadI2C_Byte(0x49)&0xfb);
    HDMI_WriteI2C_Byte(0xff,0x82);
    if(HDMI_ReadI2C_Byte(0x5e)&0x04)
    {
        Tx_HPD=1;
        Debug_Printf("\r\nhpd:high");
    }
    else
    {
        Tx_HPD=0;
        Debug_Printf("\r\nhpd:low");
    }
    return Tx_HPD;
}

void LT8618SX_HDMI_Out_Enable(bool en)
{
	HDMI_WriteI2C_Byte(0xff,0x81);
	if(en)
		{
	    HDMI_WriteI2C_Byte(0x30,0xea);
	    Debug_Printf("\r\nHDMI output Enable");
		}
	else
		{
	    HDMI_WriteI2C_Byte(0x30,0x00);
	    Debug_Printf("\r\nHDMI output Disable");
		}
}

bool LT8618SX_Read_EDID(void)
{
    u8 i,j;
    //I2CADR = LT8618SX_ADDR;
	
    HDMI_WriteI2C_Byte(0xFF,0x80);
    HDMI_WriteI2C_Byte(0xee,0x01);
    HDMI_WriteI2C_Byte(0xff,0x85);
    HDMI_WriteI2C_Byte(0x02,0x0a); //I2C 100K
    HDMI_WriteI2C_Byte(0x03,0xc9);
    HDMI_WriteI2C_Byte(0x04,0xa0); //0xA0 is EDID device address
    HDMI_WriteI2C_Byte(0x05,0x00); //0x00 is EDID offset address
    HDMI_WriteI2C_Byte(0x06,0x20); //length for read
    HDMI_WriteI2C_Byte(0x14,0x7f);
    Debug_Printf("\r\nRead EDID Start:");
    for(i=0;i<8;i++)
    {
        HDMI_WriteI2C_Byte(0x05,i*32); //0x00 is EDID offset address
        HDMI_WriteI2C_Byte(0x07,0x36);
        HDMI_WriteI2C_Byte(0x07,0x31);
        HDMI_WriteI2C_Byte(0x07,0x37);
        Delay_ms(5);
        ///*
        while(HDMI_ReadI2C_Byte(0x40)&0x04) //KEY_DDC_PRGRS=1
        {
            Debug_Printf("\r\nRead EDID DDC in progressd...");
            //Delay_ms(10);
					Delay_ms(5);
        }
        if(HDMI_ReadI2C_Byte(0x40)&0x02) //KEY_DDC_ACCS_DONE=1
        {
            if(HDMI_ReadI2C_Byte(0x40)&0x50) //DDC No Ack or Abitration lost
            {
                Debug_Printf("\r\nRead EDID Fail...");
                return FALSE;
            }
            else
            {
                for(j=0;j<32;j++)
                {
                    Sink_EDID[i*32+j]=HDMI_ReadI2C_Byte(0x83);
                    Debug_DispStrNum(" ",Sink_EDID[i*32+j]);
                }
            }
        }
    }
	return TRUE;
}

void LT8618SX_Print_Video_Inf(void)
{
	Debug_Printf("\r\n##########################LT8618SX Input Infor#####################");
	Debug_Printf("\r\nhfp = ");
	Debug_DispNum(hfp); //Hex to Dec
	
	Debug_Printf("   hs_width = ");
	Debug_DispNum(hs_width); //Hex to Dec
	
	Debug_Printf("   hbp = ");
	Debug_DispNum(hbp); //Hex to Dec
	
	Debug_Printf("   h_act = ");
	Debug_DispNum(h_act); //Hex to Dec
	
	Debug_Printf("   h_tal = ");
	Debug_DispNum(h_tal); //Hex to Dec
	
	Debug_Printf("\r\nvfp = ");
	Debug_DispNum(vfp); //Hex to Dec
	
	Debug_Printf("   vs_width = ");
	Debug_DispNum(vs_width); //Hex to Dec
	
	Debug_Printf("   vbp = ");
	Debug_DispNum(vbp); //Hex to Dec
	
	Debug_Printf("   v_act = ");
	Debug_DispNum(v_act); //Hex to Dec
	
	Debug_Printf("   v_tal = ");
	Debug_DispNum(v_tal); //Hex to Dec
	Debug_Printf("\r\n---------------------------------------------------------");
}

void LT8618SX_CSC(void)
{
		if(Video_Output_Mode == Output_RGB888)
		{
			HDMI_Y=0;
			HDMI_WriteI2C_Byte(0xff,0x82);
		if(Video_Input_Mode == Input_YCbCr444)
		{
		  HDMI_WriteI2C_Byte(0xb9,0x08); //YCbCr to RGB
		}
		else if(Video_Input_Mode==Input_YCbCr422_16BIT||
						Video_Input_Mode==Input_BT1120_16BIT||
						Video_Input_Mode==Input_BT1120_20BIT||
						Video_Input_Mode==Input_BT1120_24BIT||
						Video_Input_Mode==Input_BT656_8BIT ||
						Video_Input_Mode==Input_BT656_10BIT||
						Video_Input_Mode==Input_BT656_12BIT||
		        Video_Input_Mode==Input_BT601_8BIT )
		{
		  HDMI_WriteI2C_Byte(0xb9,0x18); //YCbCr to RGB,YCbCr 422 convert to YCbCr 444
		}
		else
		{
		HDMI_WriteI2C_Byte(0xb9,0x00); //No csc
		}
	}
	else if(Video_Output_Mode == Output_YCbCr444)
	{
		 HDMI_Y = 1;
	}
	else if(Video_Output_Mode == Output_YCbCr422)
	{
		 HDMI_Y = 2;
	}
}
void LT8618SX_Output_Mode(void)
{
 HDMI_WriteI2C_Byte(0xff,0x82);
 if(Sink_EDID[126]==0x01)//set hdmi mode
 {
 HDMI_WriteI2C_Byte(0xd6,0x8e);
 printf("\r\nHDMI Mode");
 }
 else//set dvi mode
 {
 HDMI_WriteI2C_Byte(0xd6,0x0e);
 printf("\r\nDVI Mode");
 }
}
void LT8618SX_HDMI_TX_Digital( void )
{
	//AVI
	u8	AVI_PB0	   = 0x00;
	u8	AVI_PB1	   = 0x00;
	u8	AVI_PB2	   = 0x00;

	AVI_PB1 = ( HDMI_Y << 5 ) + 0x10;

	AVI_PB2 = 0x2A;  // 16:9
//	AVI_PB2 = 0x19;// 4:3

	AVI_PB0 = ( ( AVI_PB1 + AVI_PB2 + HDMI_VIC ) <= 0x6f ) ? ( 0x6f - AVI_PB1 - AVI_PB2 - HDMI_VIC ) : ( 0x16f - AVI_PB1 - AVI_PB2 - HDMI_VIC );

	HDMI_WriteI2C_Byte( 0xff, 0x84 );
	HDMI_WriteI2C_Byte( 0x43, AVI_PB0 );    //AVI_PB0
	HDMI_WriteI2C_Byte( 0x44, AVI_PB1 );    //AVI_PB1
	HDMI_WriteI2C_Byte( 0x45, AVI_PB2 );    //AVI_PB2
	HDMI_WriteI2C_Byte( 0x47, HDMI_VIC );   //AVI_PB4

	HDMI_WriteI2C_Byte( 0x10, 0x32 );       //data iland
	HDMI_WriteI2C_Byte( 0x12, 0x64 );       //act_h_blank

	//VS_IF, 4k 30hz need send VS_IF packet. Please refer to hdmi1.4 spec 8.2.3
	if( HDMI_VIC == 95 )
	{
//	   LT8618SXB_I2C_Write_Byte(0xff,0x84);
		HDMI_WriteI2C_Byte( 0x3d, 0x2a );   //UD1 infoframe enable

		HDMI_WriteI2C_Byte( 0x74, 0x81 );
		HDMI_WriteI2C_Byte( 0x75, 0x01 );
		HDMI_WriteI2C_Byte( 0x76, 0x05 );
		HDMI_WriteI2C_Byte( 0x77, 0x49 );
		HDMI_WriteI2C_Byte( 0x78, 0x03 );
		HDMI_WriteI2C_Byte( 0x79, 0x0c );
		HDMI_WriteI2C_Byte( 0x7a, 0x00 );
		HDMI_WriteI2C_Byte( 0x7b, 0x20 );
		HDMI_WriteI2C_Byte( 0x7c, 0x01 );
	}else
	{
//	   LT8618SXB_I2C_Write_Byte(0xff,0x84);
		HDMI_WriteI2C_Byte( 0x3d, 0x0a ); //UD1 infoframe disable
	}


	//AVI_audio
	HDMI_WriteI2C_Byte( 0xff, 0x84 );
	HDMI_WriteI2C_Byte( 0xb2, 0x84 );
	HDMI_WriteI2C_Byte( 0xb3, 0x01 );
	HDMI_WriteI2C_Byte( 0xb4, 0x0a );
	HDMI_WriteI2C_Byte( 0xb5, 0x60 - ( ( ( Sampling_rate + 1 ) << 2 ) + Sampling_Size ) );  //checksum
	HDMI_WriteI2C_Byte( 0xb6, 0x11 );                                                       //AVI_PB0//LPCM
	HDMI_WriteI2C_Byte( 0xb7, ( ( Sampling_rate + 1 ) << 2 ) + Sampling_Size );             //AVI_PB1//32KHz 24bit(32bit)
}
void LT8618SX_HDMI_TX_Phy(void) 
{
	HDMI_WriteI2C_Byte(0xff,0x81);
	HDMI_WriteI2C_Byte(0x30,0xea);//0xea
	HDMI_WriteI2C_Byte(0x31,0x44);//DC: 0x44, AC:0x73
	HDMI_WriteI2C_Byte(0x32,0x4a);
	HDMI_WriteI2C_Byte(0x33,0x0b);
	HDMI_WriteI2C_Byte(0x34,0x00);//d0 pre emphasis
	HDMI_WriteI2C_Byte(0x35,0x00);//d1 pre emphasis
	HDMI_WriteI2C_Byte(0x36,0x00);//d2 pre emphasis
	HDMI_WriteI2C_Byte(0x37,0x44);
	HDMI_WriteI2C_Byte(0x3f,0x0f);
	HDMI_WriteI2C_Byte(0x40,0xb0);//clk swing
	HDMI_WriteI2C_Byte(0x41,0xa0);//d0 swing
	HDMI_WriteI2C_Byte(0x42,0xa0);//d1 swing
	HDMI_WriteI2C_Byte(0x43,0xa0); //d2 swing
	HDMI_WriteI2C_Byte(0x44,0x0a);	
}

void LT8618SX_BT_Set(void)
{
	if(Video_Input_Mode == Input_BT1120_16BIT)
	{
		u8 pol;
		
		//pol=
		HDMI_WriteI2C_Byte(0xff,0x82);
		HDMI_WriteI2C_Byte(0x20,((video_bt.hact)>>8)); 
		HDMI_WriteI2C_Byte(0x21,(video_bt.hact)); 
		HDMI_WriteI2C_Byte(0x22,((video_bt.hfp)>>8)); 
		HDMI_WriteI2C_Byte(0x23,(video_bt.hfp)); 
		HDMI_WriteI2C_Byte(0x24,((video_bt.hs)>>8)); 
		HDMI_WriteI2C_Byte(0x25,(video_bt.hs)); 
		HDMI_WriteI2C_Byte(0x26,0x00);
		HDMI_WriteI2C_Byte(0x27,0x00);
		HDMI_WriteI2C_Byte(0x36,(video_bt.vact>>8));
		HDMI_WriteI2C_Byte(0x37,video_bt.vact);
		HDMI_WriteI2C_Byte(0x38,(video_bt.vfp>>8));
		HDMI_WriteI2C_Byte(0x39,video_bt.vfp);
		HDMI_WriteI2C_Byte(0x3a,(video_bt.vbp>>8));
		HDMI_WriteI2C_Byte(0x3b,video_bt.vbp);
		HDMI_WriteI2C_Byte(0x3c,(video_bt.vs>>8));
		HDMI_WriteI2C_Byte(0x3d,video_bt.vs);
	}
	else if(Video_Input_Mode == Input_BT601_8BIT)
	{
			HDMI_WriteI2C_Byte( 0xff, 0x82 );
			HDMI_WriteI2C_Byte( 0x2c,((2*video_bt.htotal)>>8));
			HDMI_WriteI2C_Byte( 0x2d,(2*video_bt.htotal));
			HDMI_WriteI2C_Byte( 0x2e,((2*video_bt.hact)>>8));
			HDMI_WriteI2C_Byte( 0x2f,(2*video_bt.hact));
			HDMI_WriteI2C_Byte( 0x30,((2*video_bt.hfp)>>8));
			HDMI_WriteI2C_Byte( 0x31,(2*video_bt.hfp));
			HDMI_WriteI2C_Byte( 0x32,((2*video_bt.hbp)>>8));
			HDMI_WriteI2C_Byte( 0x33,(2*video_bt.hbp));
			HDMI_WriteI2C_Byte( 0x34,((2*video_bt.hs)>>8));
			HDMI_WriteI2C_Byte( 0x35,(2*video_bt.hs));

			HDMI_WriteI2C_Byte( 0x36,((video_bt.vact)>>8));
			HDMI_WriteI2C_Byte( 0x37,(video_bt.vact));
			HDMI_WriteI2C_Byte( 0x38,((video_bt.vfp)>>8));
			HDMI_WriteI2C_Byte( 0x39,(video_bt.vfp));
			HDMI_WriteI2C_Byte( 0x3a,((video_bt.vbp)>>8));
			HDMI_WriteI2C_Byte( 0x3b,(video_bt.vbp));
			HDMI_WriteI2C_Byte( 0x3c,((video_bt.vs)>>8));
			HDMI_WriteI2C_Byte( 0x3d,(video_bt.vs));
	}
	else if(Video_Input_Mode == Input_BT656_8BIT)
	{
	            hs_width *= 2;
              hbp *= 2;
              hfp *= 2;
              h_tal *= 2;
              h_act *= 2;
			  
    HDMI_WriteI2C_Byte(0xff,0x82);
		HDMI_WriteI2C_Byte(0x20,((video_bt.hact)>>8)); 
		HDMI_WriteI2C_Byte(0x21,(video_bt.hact)); 
		HDMI_WriteI2C_Byte(0x22,((video_bt.hfp)>>8)); 
		HDMI_WriteI2C_Byte(0x23,(video_bt.hfp)); 
		HDMI_WriteI2C_Byte(0x24,((video_bt.hs)>>8)); 
		HDMI_WriteI2C_Byte(0x25,(video_bt.hs)); 
		HDMI_WriteI2C_Byte(0x26,0x00);
		HDMI_WriteI2C_Byte(0x27,0x00);
		HDMI_WriteI2C_Byte(0x36,(video_bt.vact>>8));
		HDMI_WriteI2C_Byte(0x37,video_bt.vact);
		HDMI_WriteI2C_Byte(0x38,(video_bt.vfp>>8));
		HDMI_WriteI2C_Byte(0x39,video_bt.vfp);
		HDMI_WriteI2C_Byte(0x3a,(video_bt.vbp>>8));
		HDMI_WriteI2C_Byte(0x3b,video_bt.vbp);
		HDMI_WriteI2C_Byte(0x3c,(video_bt.vs>>8));
		HDMI_WriteI2C_Byte(0x3d,video_bt.vs);
	/*
    HDMI_WriteI2C_Byte(0xff,0x82);//1080p
		HDMI_WriteI2C_Byte(0x20,0x0f);   //h_act
		HDMI_WriteI2C_Byte(0x21,0x00); 
		HDMI_WriteI2C_Byte(0x22,0x00); 	// hfp
		HDMI_WriteI2C_Byte(0x23,0xb0);
		HDMI_WriteI2C_Byte(0x24,0x00);     //hs_width
		HDMI_WriteI2C_Byte(0x25,0x58); 
		HDMI_WriteI2C_Byte(0x26,0x00);
		HDMI_WriteI2C_Byte(0x27,0x00);
		HDMI_WriteI2C_Byte(0x36,0x04);	//v_act
		HDMI_WriteI2C_Byte(0x37,0x38);
		HDMI_WriteI2C_Byte(0x38,0x00);    //vfp
		HDMI_WriteI2C_Byte(0x39,0x04);
		HDMI_WriteI2C_Byte(0x3a,0x00);	//vbp
		HDMI_WriteI2C_Byte(0x3b,0x24);
		HDMI_WriteI2C_Byte(0x3c,0x00);	    //vs_width
		HDMI_WriteI2C_Byte(0x3d,0x05);
   

    HDMI_WriteI2C_Byte(0xff,0x82);//480p
		HDMI_WriteI2C_Byte(0x20,0x05);   //h_act
		HDMI_WriteI2C_Byte(0x21,0xa0); 
		HDMI_WriteI2C_Byte(0x22,0x00); 	// hfp
		HDMI_WriteI2C_Byte(0x23,0x20);
		HDMI_WriteI2C_Byte(0x24,0x00);     //hs_width
		HDMI_WriteI2C_Byte(0x25,0x7c); 
		HDMI_WriteI2C_Byte(0x26,0x00);
		HDMI_WriteI2C_Byte(0x27,0x00);
		HDMI_WriteI2C_Byte(0x36,0x01);	//v_act
		HDMI_WriteI2C_Byte(0x37,0xe0);
		HDMI_WriteI2C_Byte(0x38,0x00);    //vfp
		HDMI_WriteI2C_Byte(0x39,0x09);
		HDMI_WriteI2C_Byte(0x3a,0x00);	//vbp
		HDMI_WriteI2C_Byte(0x3b,0x1f);
		HDMI_WriteI2C_Byte(0x3c,0x00);	    //vs_width
		HDMI_WriteI2C_Byte(0x3d,0x06);
	*/
	   }
	}

u8 LT8618SX_Video_Check(void)
{
   
  u8 temp;
	
	hfp = 0;
	hs_width = 0;
	hbp = 0;
	h_act = 0;
	h_tal = 0;
	vfp = 0;
	vs_width = 0;
	vbp = 0;
	v_act = 0;
	v_tal = 0;
	
				HDMI_WriteI2C_Byte( 0xff, 0x80 );
				HDMI_WriteI2C_Byte( 0x13, 0xf1 );//ttl video process reset
				HDMI_WriteI2C_Byte( 0x12, 0xfb );//video check reset
				Delay_ms( 1 ); 
				HDMI_WriteI2C_Byte( 0x12, 0xff );
				HDMI_WriteI2C_Byte( 0x13, 0xf9 );
				
				Delay_ms( 100 ); 
				
				
  if((Video_Input_Mode==Input_BT601_8BIT)||(Video_Input_Mode==Input_RGB888)||(Video_Input_Mode==Input_YCbCr422_16BIT)||(Video_Input_Mode==Input_YCbCr444))/*extern sync*/
  {
		    HDMI_WriteI2C_Byte(0xff,0x82);
		    HDMI_WriteI2C_Byte( 0x51, 0x00 );
		
       HDMI_WriteI2C_Byte(0xff,0x82); //video check
       temp=HDMI_ReadI2C_Byte(0x70);  //hs vs polarity
		
       if(temp&0x02)
			 {
			     printf("\r\n vs_pol is 1");
			 }
			 else
			 {
			     printf("\r\n vs_pol is 0");
			 }
       if( temp & 0x01 )
			 {
			     printf("\r\n hs_pol is 1");
			 }
			 else
			 {
			     printf("\r\n hs_pol is 0");
			 } 
			 
       vs_width = HDMI_ReadI2C_Byte(0x71);
       hs_width = HDMI_ReadI2C_Byte(0x72);
       hs_width = ( (hs_width & 0x0f) << 8 ) + HDMI_ReadI2C_Byte(0x73);
       vbp = HDMI_ReadI2C_Byte(0x74);
       vfp = HDMI_ReadI2C_Byte(0x75);
       hbp = HDMI_ReadI2C_Byte(0x76);
       hbp = ( (hbp & 0x0f) << 8 ) + HDMI_ReadI2C_Byte(0x77);
       hfp = HDMI_ReadI2C_Byte(0x78);
       hfp = ( (hfp & 0x0f) << 8 ) + HDMI_ReadI2C_Byte(0x79);
       v_tal = HDMI_ReadI2C_Byte(0x7a);
       v_tal = ( v_tal << 8 ) + HDMI_ReadI2C_Byte(0x7b);
       h_tal = HDMI_ReadI2C_Byte(0x7c);
       h_tal = ( h_tal << 8 ) + HDMI_ReadI2C_Byte(0x7d);
       v_act = HDMI_ReadI2C_Byte(0x7e);
       v_act = ( v_act << 8 ) + HDMI_ReadI2C_Byte(0x7f);
       h_act = HDMI_ReadI2C_Byte(0x80);
       h_act = ( h_act << 8 ) + HDMI_ReadI2C_Byte(0x81);
   }	  
  else if((Video_Input_Mode==Input_BT1120_16BIT)||(Video_Input_Mode==Input_BT656_8BIT))/*embbedded sync */
	{
				HDMI_WriteI2C_Byte(0xff,0x82);
		    HDMI_WriteI2C_Byte( 0x51, 0x42 );
				v_act = HDMI_ReadI2C_Byte(0x8b);
				v_act = ( v_act << 8 ) + HDMI_ReadI2C_Byte(0x8c);

				h_act = HDMI_ReadI2C_Byte(0x8d);
				h_act = ( h_act << 8 ) + HDMI_ReadI2C_Byte(0x8e)-0x04;/*note -0x04*/

				h_tal= HDMI_ReadI2C_Byte(0x8f);
				h_tal = ( h_tal << 8 ) + HDMI_ReadI2C_Byte(0x90);
	}
	   

    if(Video_Input_Mode==Input_BT601_8BIT||Video_Input_Mode==Input_BT656_8BIT)
    {
        hs_width/=2;
        hbp/=2;
        hfp/=2;
        h_tal/=2;
        h_act/=2;
    }
	
    LT8618SX_Print_Video_Inf();
	
		if((h_act==video_640x480_60Hz.hact)&&
	   (v_act==video_640x480_60Hz.vact)&&
	   (h_tal==video_640x480_60Hz.htotal))
	 {
		Debug_Printf("\r\nVideo_Check = video_640x480_60Hz ");
		Video_Format=video_640x480_60Hz_vic1;
		HDMI_VIC=1;
		video_bt= video_640x480_60Hz;
	 }	
	 

   else if((h_act==video_720x480_60Hz.hact)&&
		 (v_act==video_720x480_60Hz.vact)&&
		 (h_tal==video_720x480_60Hz.htotal))
		{
			Debug_Printf("\r\nVideo_Check = video_720x480_60Hz ");
			Video_Format=video_720x480_60Hz_vic3;
			HDMI_VIC=3;
			video_bt=video_720x480_60Hz;
	  }	
	else if((h_act==video_1280x720_30Hz.hact)&&
		 (v_act==video_1280x720_30Hz.vact)&&
		 (h_tal==video_1280x720_30Hz.htotal)
		 )
		{
			Debug_Printf("\r\nVideo_Check = video_1280x720_30Hz ");
			Video_Format=video_1280x720_30Hz_vic62;
			HDMI_VIC=0;
			video_bt=video_1280x720_30Hz;
	  }
	else if((h_act==video_1280x720_60Hz.hact)&&
		 (v_act==video_1280x720_60Hz.vact)&&
		 (h_tal==video_1280x720_60Hz.htotal)
		 )
		{
			Debug_Printf("\r\nVideo_Check = video_1280x720_60Hz ");
			Video_Format=video_1280x720_60Hz_vic4;
			HDMI_VIC=4;
			video_bt=video_1280x720_60Hz;
	  }
		else if((h_act==video_1920x1080_30Hz.hact)&&
		 (v_act==video_1920x1080_30Hz.vact)&&
		 (h_tal==video_1920x1080_30Hz.htotal))
		{
			Debug_Printf("\r\nVideo_Check = video_1920x1080_30Hz ");
			Video_Format=video_1920x1080_30Hz_vic34;
			HDMI_VIC=34;
			video_bt=video_1920x1080_30Hz;
	  }
	   else if((h_act==video_1920x1080_50Hz.hact)&&
		 (v_act==video_1920x1080_50Hz.vact)&&
		 (h_tal==video_1920x1080_50Hz.htotal))
		{
			Debug_Printf("\r\nVideo_Check = video_1920x1080_50Hz ");
			Video_Format=video_1920x1080_50Hz_vic31;
			HDMI_VIC=31;
			video_bt=video_1920x1080_50Hz;
	  }
    else if((h_act==video_1920x1080_60Hz.hact)&&
		 (v_act==video_1920x1080_60Hz.vact)&&
		 (h_tal==video_1920x1080_60Hz.htotal))
		{
			Debug_Printf("\r\nVideo_Check = video_1920x1080_60Hz ");
			Video_Format=video_1920x1080_60Hz_vic16;
			HDMI_VIC=16;
			video_bt=video_1920x1080_60Hz;
	  }
//		else if((h_act==video_3840x2160_24Hz.hact)&&
//		 (v_act==video_3840x2160_24Hz.vact)&&
//		 (h_tal==video_3840x2160_24Hz.htotal))
//		{
//			Debug_Printf("\r\nVideo_Check = video_3840x2160_24Hz ");
//			Video_Format=video_3840x2160_24Hz_vic207;
//			HDMI_VIC=207;
//			video_bt=video_3840x2160_24Hz;
//	  }
//		 else if((h_act==video_3840x2160_25Hz.hact)&&
//		 (v_act==video_3840x2160_25Hz.vact)&&
//		 (h_tal==video_3840x2160_25Hz.htotal))
//		{
//			Debug_Printf("\r\nVideo_Check = video_3840x2160_25Hz ");
//			Video_Format=video_3840x2160_25Hz_vic206;
//			HDMI_VIC=206;
//			video_bt=video_3840x2160_25Hz;
//	  }
    else if((h_act==video_3840x2160_30Hz.hact)&&
		 (v_act==video_3840x2160_30Hz.vact)&&
		 (h_tal==video_3840x2160_30Hz.htotal))
		{
			Debug_Printf("\r\nVideo_Check = video_3840x2160_30Hz ");
			Video_Format=video_3840x2160_30Hz_vic205;
			HDMI_VIC=16;
			video_bt=video_3840x2160_30Hz;
	  }
		else
		{
			Video_Format=video_720x480_60Hz_vic3;
			HDMI_VIC=3;
			video_bt=video_720x480_60Hz;
		 }
	return Video_Format;
}

u32 LT8618SX_CLK_Det(void)
{
  u32 dclk_;
  HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x80);
	Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad ttl dclk = ");
  Debug_DispNum(dclk_); 
	return dclk_;
}

void LT8618SX_test_clk(void)
{
  u32 dclk_;
 
	HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x8d);
	 Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad hdmitx read clk = ");
  Debug_DispNum(dclk_); 
	
	HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x8b);
	 Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad txpll pix dclk = ");
  Debug_DispNum(dclk_);
	
	HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x8c);
	 Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad hdmitx write clk = ");
  Debug_DispNum(dclk_); 
	
	HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x86);
	 Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad txpll dec ddr d clk = ");
  Debug_DispNum(dclk_);
	
	HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x87);
	 Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad txpll drpt dclk = ");
  Debug_DispNum(dclk_); 

	HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x8a);
	 Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad txpll ref dclk = ");
  Debug_DispNum(dclk_);
	
	HDMI_WriteI2C_Byte(0xff,0x82);
  HDMI_WriteI2C_Byte(0x17,0x8b);
	 Delay_ms(500);
  dclk_=(((HDMI_ReadI2C_Byte(0x1d))&0x0f)<<8)+HDMI_ReadI2C_Byte(0x1e);
  dclk_=(dclk_<<8)+HDMI_ReadI2C_Byte(0x1f);
  Debug_Printf("\r\nad txpll pix dclk = ");
  Debug_DispNum(dclk_);
}
bool LT8618SX_Jug(void)
{
  u16		V_ACT	   = 0x0000;
	u16		H_ACT	   = 0x0000;
	u16		H_TOTAL	   = 0x0000;
	u16		V_TOTAL	   = 0x0000;
	
	u8 i=4;
	
	Delay_ms(5);
	if(!LT8618SX_HPD_Status())
	{
	 return 0;
	}
	
	while(i--)
	{
		      Delay_ms(5);
					if(Video_Input_Mode == Input_RGB888)
								{
										//				  HDMI_WriteI2C_Byte( 0xff, 0x80 );
										//					HDMI_WriteI2C_Byte( 0x09, 0xf6 );
//										HDMI_WriteI2C_Byte( 0xff, 0x81 );
//										HDMI_WriteI2C_Byte( 0x51, 0x00 );
//									
										HDMI_WriteI2C_Byte( 0xff, 0x82 );
										H_TOTAL	   = HDMI_ReadI2C_Byte( 0x7c );
										H_TOTAL	   = ( H_TOTAL << 8 ) + HDMI_ReadI2C_Byte( 0x7d );
										V_TOTAL	   = HDMI_ReadI2C_Byte( 0x7a );
										V_TOTAL	   = ( V_TOTAL << 8 ) + HDMI_ReadI2C_Byte( 0x7b );
										V_ACT  = HDMI_ReadI2C_Byte( 0x7e );
										V_ACT  = ( V_ACT << 8 ) + HDMI_ReadI2C_Byte( 0x7f );
										H_ACT  = HDMI_ReadI2C_Byte( 0x80 );
										H_ACT  = ( H_ACT << 8 ) + HDMI_ReadI2C_Byte( 0x81 );


										//					 HDMI_WriteI2C_Byte( 0xff, 0x80 );
										//					 HDMI_WriteI2C_Byte( 0x09, 0xfe );
								}
								else if((Video_Input_Mode == Input_BT1120_16BIT)||(Video_Input_Mode == Input_BT656_8BIT))
								{
													//				  HDMI_WriteI2C_Byte( 0xff, 0x80 );
													//					HDMI_WriteI2C_Byte( 0x09, 0xf6 );
//													HDMI_WriteI2C_Byte( 0xff, 0x82 );
//													HDMI_WriteI2C_Byte( 0x51, 0x02 );

													HDMI_WriteI2C_Byte( 0xff, 0x82 );
													H_TOTAL	   = HDMI_ReadI2C_Byte( 0x8f );
													H_TOTAL	   = ( H_TOTAL << 8 ) + HDMI_ReadI2C_Byte( 0x90 );
													V_ACT  = HDMI_ReadI2C_Byte( 0x8b );
													V_ACT  = ( V_ACT << 8 ) + HDMI_ReadI2C_Byte( 0x8c );
													H_ACT  = HDMI_ReadI2C_Byte( 0x8d );
													H_ACT  = ( H_ACT << 8 ) + HDMI_ReadI2C_Byte( 0x8e )-0x04;//note

													//					HDMI_WriteI2C_Byte( 0xff, 0x80 );
													//					HDMI_WriteI2C_Byte( 0x09, 0xfe );
								}
//										Debug_Printf("\r\n h_total=");
//										Debug_DispNum(H_TOTAL);
//										Debug_Printf("\r\n v_total=");
//										Debug_DispNum(V_TOTAL);
//										Debug_Printf("\r\n v_act=");
//										Debug_DispNum(V_ACT);
//										Debug_Printf("\r\n h_act=");
//										Debug_DispNum(H_ACT);
								
								if( ( V_ACT ==(video_bt.vact) ) &&(H_ACT==(video_bt.hact) )&&(H_TOTAL== (video_bt.htotal) )/*&&(V_TOTAL==(_video_timing.vtotal) ) */)
								{
									return 1;
								}
								else 
								{
									;
								}
		 
		}
	return 0;
}

/**********************************************************/
//only use for embbedded_sync(bt1120,bt656...) or DDR
/************************************************************/
 bool LT8618SX_Phase_Config(void)
{
	u8		Temp       = 0x00;
	u8    Temp_f       =0x00;
	u8	    OK_CNT	   = 0x00;
	u8      OK_CNT_1   = 0x00;
	u8      OK_CNT_2   = 0x00;
	u8      OK_CNT_3   = 0x00;	
	u8		Jump_CNT   = 0x00;
	u8		Jump_Num   = 0x00;
	u8		Jump_Num_1 = 0x00;
	u8		Jump_Num_2 = 0x00;
	u8		Jump_Num_3 = 0x00;
	bool    temp0_ok   =0;
	bool    temp9_ok   =0;
	bool	b_OK	   = 0;
	u16		V_ACT	   = 0x0000;
	u16		H_ACT	   = 0x0000;
	u16		H_TOTAL	   = 0x0000;
	u16		V_TOTAL	   = 0x0000;
	
	
	  Temp_f=LT8618SX_Phase() ;//it's setted before video check
		
		if(Video_Input_Mode == Input_RGB888)
		{
		HDMI_WriteI2C_Byte( 0xff, 0x81 );
		HDMI_WriteI2C_Byte( 0x27, ( 0x60 + (Temp_f+phease_offset)%0x0a ));
		printf("cail phase is 0x%x",(u16)HDMI_ReadI2C_Byte(0x27));
		return 1;
		}
		 
		while( Temp <= 0x09 )
		{	
				HDMI_WriteI2C_Byte( 0xff, 0x81 );
				HDMI_WriteI2C_Byte( 0x27, (0x60+Temp));
			
				HDMI_WriteI2C_Byte( 0xff, 0x80 );
				HDMI_WriteI2C_Byte( 0x13, 0xf1 );//ttl video process reset
				HDMI_WriteI2C_Byte( 0x12, 0xfb );//video check reset
				Delay_ms( 1 ); 
				HDMI_WriteI2C_Byte( 0x12, 0xff );
				HDMI_WriteI2C_Byte( 0x13, 0xf9 );
				Delay_ms( 100 ); //       
				if((Video_Input_Mode == Input_BT1120_16BIT)||(Video_Input_Mode == Input_BT656_8BIT))
				{
						HDMI_WriteI2C_Byte( 0xff, 0x82 );
						HDMI_WriteI2C_Byte( 0x51, 0x42 );
						H_TOTAL	   = HDMI_ReadI2C_Byte( 0x8f );
						H_TOTAL	   = ( H_TOTAL << 8 ) + HDMI_ReadI2C_Byte( 0x90 );
						V_ACT  = HDMI_ReadI2C_Byte( 0x8b );
						V_ACT  = ( V_ACT << 8 ) + HDMI_ReadI2C_Byte( 0x8c );
						H_ACT  = HDMI_ReadI2C_Byte( 0x8d );
						H_ACT  = ( H_ACT << 8 ) + HDMI_ReadI2C_Byte( 0x8e )-0x04;//note
				}
				#ifdef _DEBUG_MODE
						Debug_Printf("\r\n h_total=");
						Debug_DispNum(H_TOTAL);
						Debug_Printf("\r\n v_act=");
						Debug_DispNum(V_ACT);
						Debug_Printf("\r\n h_act=");
						Debug_DispNum(H_ACT);
        #endif
			if( ( V_ACT ==(video_bt.vact) ) &&(H_ACT==(video_bt.hact) )&&(H_TOTAL== (video_bt.htotal) ))
				{
					OK_CNT++;
					if( b_OK == 0 )
					{
						b_OK = 1;
						Jump_CNT++;
						if( Jump_CNT == 1 )
						{
							Jump_Num_1 = Temp;
						}
						else if( Jump_CNT == 3 )
						{
							Jump_Num_2 = Temp;
						}
						else if( Jump_CNT == 5 )
						{
							Jump_Num_3 = Temp;
						}
					}

					if(Jump_CNT==1)
					{
					  OK_CNT_1++;
					}
					else if(Jump_CNT==3)
					{
					  OK_CNT_2++;
					}
					else if(Jump_CNT==5)
					{
					  OK_CNT_3++;
					}

					if(Temp==0)
					{
					  temp0_ok=1;
					}
					if(Temp==9)
					{
					  Jump_CNT++;
					  temp9_ok=1;
					}
					#ifdef _DEBUG_MODE
					Debug_Printf("\r\n this phase is ok,temp=");
			    Debug_DispNum(Temp);
					Debug_Printf("\r\n Jump_CNT=");
	        Debug_DispNum(Jump_CNT);
					#endif
				}
			else			
				{
					if( b_OK )
					{
						b_OK = 0;
						Jump_CNT++;
					}
					#ifdef _DEBUG_MODE
					Debug_Printf("\r\n this phase is fail,temp=");
			    Debug_DispNum(Temp);
					Debug_Printf("\r\n Jump_CNT=");
	        Debug_DispNum(Jump_CNT);
					#endif
				}
			Temp++;
			}
		#ifdef _DEBUG_MODE
	  Debug_Printf("\r\n OK_CNT_1=");
		Debug_DispNum(OK_CNT_1);
		Debug_Printf("\r\n OK_CNT_2=");
		Debug_DispNum(OK_CNT_2);
		Debug_Printf("\r\n OK_CNT_3=");
		Debug_DispNum(OK_CNT_3);
    #endif
		if((Jump_CNT==0)||(Jump_CNT>6))
		{
		Debug_Printf("\r\ncali phase fail");
		return 0;
		}

		if((temp9_ok==1)&&(temp0_ok==1))
		{
		  if(Jump_CNT==6)
		  {
		  OK_CNT_3=OK_CNT_3+OK_CNT_1;
		  OK_CNT_1=0;
		  }
		  else if(Jump_CNT==4)
		  {
		   OK_CNT_2=OK_CNT_2+OK_CNT_1;
		   OK_CNT_1=0;
		  }
		}
		if(Jump_CNT>=2)
		{
			if(OK_CNT_1>=OK_CNT_2)
			{
			
				if(OK_CNT_1>=OK_CNT_3)
				{
					OK_CNT=OK_CNT_1;
					Jump_Num=Jump_Num_1;
				}
				else
				{
					OK_CNT=OK_CNT_3;
					Jump_Num=Jump_Num_3;
				}
			}
		
			else
			{
				if(OK_CNT_2>=OK_CNT_3)
				{
					OK_CNT=OK_CNT_2;
					Jump_Num=Jump_Num_2;
				}
				else
				{
					OK_CNT=OK_CNT_3;
					Jump_Num=Jump_Num_3;
				}
			}
		
	    }	  
	  HDMI_WriteI2C_Byte( 0xff, 0x81 );
			
		if( ( Jump_CNT == 2 ) || ( Jump_CNT == 4 ) || ( Jump_CNT == 6 ))
		{
			HDMI_WriteI2C_Byte( 0x27, ( 0x60 + ( Jump_Num  + ( OK_CNT / 2 ) ) % 0x0a ) );
		}
		if(OK_CNT==0x0a)
		{
			 HDMI_WriteI2C_Byte( 0x27, ( 0x60 + (Temp_f+phease_offset)%0x0a ));
		
		}
		printf("cail phase is 0x%x",(u16)HDMI_ReadI2C_Byte(0x27));
		return 1;
		
	}


void LT8618SX_Init(void)
{
	RESET_Lt8618SX();
	LT8618SX_Chip_ID();	
	LT8618SX_RST_PD_Init();
	LT8618SX_TTL_Input_Analog();
	LT8618SX_TTL_Input_Digital();
	LT8618SX_PLL();
	LT8618SX_Audio_Init();
	Delay_ms(1000);
#ifdef _HDCP
	LT8618SX_HDCP_Init();
	Debug_Printf("\r\nHDCP Init");
#endif
	LT8618SX_IRQ_Init();//interrupt of hdp_change or video_input_change
	vid_chk_flag = 1;
}

void IrqInit(void)
{
    Enable_INT_Port1;
    Enable_BIT5_FallEdge_Trig;
    set_EPI;							// Enable pin interrupt
    set_EA;							// global enable bit
}


u8 LT8618SX_Phase(void)  
{
  u8 temp=0;
	u8 read_value=0;
	u8 b_ok=0;
	u8 Temp_f=0;

	for(temp=0;temp<0x0a;temp++)
	{
	  HDMI_WriteI2C_Byte(0xff,0x81); 
	  HDMI_WriteI2C_Byte(0x27,(0x60+temp));
		if(USE_DDRCLK ==0 )
		{
		HDMI_WriteI2C_Byte(0x4d,0x01); //sdr=01,ddr=05
		HDMI_WriteI2C_Byte(0x4d,0x09); //sdr=09,ddr=0d;
		}
		else
		{
		HDMI_WriteI2C_Byte(0x4d,0x05); //sdr=01,ddr=05
		HDMI_WriteI2C_Byte(0x4d,0x0d); //sdr=09,ddr=0d;
		}
		read_value=HDMI_ReadI2C_Byte(0x50);//1->0 
		#ifdef _DEBUG_MODE
		Debug_Printf("\r\ntemp=");
		Debug_DispNum(temp);
		Debug_Printf("\r\nread_value=");
		Debug_DispNum(read_value);
		#endif
		if(read_value==0x00)
		{
		   if(b_ok==0)
		   {
		    Temp_f=temp;
		   }
		   b_ok=1;
		}
		else
		{
		   b_ok=0;
		}
	}
	#ifdef _DEBUG_MODE
	Debug_Printf("\r\nTemp_f=");
	Debug_DispNum(Temp_f);
	#endif
	return Temp_f;
}

void PinInterrupt_ISR (void) interrupt 7
{
  clr_EPI;
	//intb_flag=0;
	
	irq_flag3=0;
	irq_flag1=0;
	irq_flag0=0;
	
	if(PIF != 0x20)
	{ 
	    PIF=0x00;
	    set_EPI;
	    return;
	}
	
	if(0==P15)
	{
		  HDMI_WriteI2C_Byte(0xff,0x82); 
	    irq_flag3=HDMI_ReadI2C_Byte(0x0f);//hpd
	    irq_flag0=HDMI_ReadI2C_Byte(0x0c);//vid_chang
		  irq_flag1=HDMI_ReadI2C_Byte(0x0d);//hdcp
		
		  HDMI_WriteI2C_Byte(0xff,0x82); 
			HDMI_WriteI2C_Byte(0x07,0xff); //clear3
			HDMI_WriteI2C_Byte(0x07,0x3f); //clear3 
		  HDMI_WriteI2C_Byte(0x05,0xff); //clear1
			HDMI_WriteI2C_Byte(0x05,0xed); //clear1
		  HDMI_WriteI2C_Byte(0x04,0xff); //clear0 
	    HDMI_WriteI2C_Byte(0x04,0xfe); 
		
		  Debug_Printf( "\r\nEnter IRQ" );
      printf("\r\nflag3=%bx",irq_flag3);
		  printf("\r\nflag0=%bx",irq_flag0);
		  printf("\r\nflag1=%bx",irq_flag1);
		
	PIF=0x00;	
	set_EPI;
		
	}
}

void LT8618SX_INTB(void)
{

	u8 read_val=0;
	bool stable_flag=0;
	
 if((irq_flag3==0)&&(irq_flag0==0)&&(irq_flag1==0))//no interrupt
 {
  //Debug_Printf( "\r\nint" );
 }
 else//get interrupt
 {	 
	    Debug_Printf( "\r\nEnter IRQ Task:" );	
		  #ifdef _HDCP 
			if(irq_flag3 & 0x80)//~tx hpd:1->0
			{
			 LT8618SX_HDCP_Enable(Disable);
			 LT8618SX_HDMI_Out_Enable(0);
			 //vid_chk_flag=1;
			irq_flag1=0;
			 printf("\r\n~hpd");
			}
			if(irq_flag3 & 0x40)//tx hpd:0->1
			{
				LT8618SX_HDMI_Out_Enable(1);
				LT8618SX_HDCP_Enable(Enable);
				printf("\r\nhpd");
				//vid_chk_flag=1;
				irq_flag1=0;
			}
			HDMI_WriteI2C_Byte(0xff,0x82); 
			HDMI_WriteI2C_Byte(0x07,0xff); //clear3
			HDMI_WriteI2C_Byte(0x07,0x3f); //clear3 
			
			
			if(irq_flag1 & 0x02)//~tx auth pass
			{
				LT8618SX_HDCP_Enable(Disable);
				if(LT8618SX_Jug())
				{
					LT8618SX_HDCP_Enable(Enable);
					printf("\r\n~pass");
				}
			}
			else if((irq_flag1 & 0x10))//tx_auth_done
			{
	
				HDMI_WriteI2C_Byte( 0xff, 0x85 );
				read_val=HDMI_ReadI2C_Byte( 0x44 );
				if(!(read_val&0x02))//tx_auth_pass
				{
					LT8618SX_HDCP_Enable(Disable);
					if(LT8618SX_Jug())
					{
					LT8618SX_HDCP_Enable(Enable);
					printf("\r\ndone");
					}
				}
			}
			HDMI_WriteI2C_Byte(0xff,0x82); 
			HDMI_WriteI2C_Byte(0x05,0xff); //clear1
			HDMI_WriteI2C_Byte(0x05,0xed); //clear1
			
			#else
	    if(irq_flag3 & 0xc0) //HPD_Detect
	    {
	        Debug_Printf("\r\nHPD:");
	        LT8618SX_HPD_Status();
	        if(Tx_HPD)
	        {
						Debug_Printf("High");
						#ifdef _READ_EDID
						Delay_ms(5000);
						LT8618SX_Read_EDID();
						#endif
						LT8618SX_HDMI_Out_Enable(1);
						vid_chk_flag=1;
	        }
	        else
	        {
	    	    Debug_Printf("Low");
	    	    LT8618SX_HDMI_Out_Enable(0);
	        }
			
	        HDMI_WriteI2C_Byte(0xff,0x82); 
	        HDMI_WriteI2C_Byte(0x07,0xff); //clear3
	        HDMI_WriteI2C_Byte(0x07,0x3f); //clear3
	    } 
			#endif
	    if(irq_flag0 & 0x01) //vid_chk
	    {
	           vid_chk_flag=1;
				
	        Debug_Printf("\r\nvideo change");
	        HDMI_WriteI2C_Byte(0xff,0x82); 
	        HDMI_WriteI2C_Byte(0x9e,0xff); //clear vid chk irq
	        HDMI_WriteI2C_Byte(0x9e,0xf7); 
					
	        HDMI_WriteI2C_Byte(0x04,0xff); //clear0 irq
	        HDMI_WriteI2C_Byte(0x04,0xfe); 
	    }
			printf("\r\nflag3=%bx",irq_flag3);
		  printf("\r\nflag0=%bx",irq_flag0);
		  printf("\r\nflag1=%bx",irq_flag1);
			
			irq_flag3=0;
			irq_flag1=0;
			irq_flag0=0;
 }
 
}

void LT8618SX_Task(void)
{
//static u8 timeout=0;
	
		if(vid_chk_flag)
		{     
			   // timeout++;
			    LT8618SX_PLL();
			    //LT8618SX_test_clk();//only for debug
			    LT8618SX_Video_Check();
					Debug_Printf("\r\nVideo_Format = ");
	        Debug_DispNum(Video_Format); //Hex to Dec
			    if(0)//((Video_Format == video_none))
          {
            LT8618SX_HDMI_Out_Enable(0);
            #ifdef _HDCP
					  LT8618SX_HDCP_Enable(Disable);
            #endif	 
          }					
				  else
				  {
						#ifdef _READ_EDID
						LT8618SX_Output_Mode();//set ouput as DVI/HDMI
						#endif
						LT8618SX_CSC();
						LT8618SX_HDMI_TX_Digital();
						LT8618SX_HDMI_TX_Phy();
						LT8618SX_BT_Set();	
						LT8618SX_HDMI_Out_Enable(1);
					#ifdef _HDCP
						Delay_ms(200);
						LT8618SX_HDCP_Enable(Enable);
					#endif	
						LT8618SX_Phase_Config();
						
						
						vid_chk_flag=0;
						intb_flag=0;
						//timeout=0;
				  }
//		if(timeout>10)
//		{ 
//				timeout=0;
//				vid_chk_flag=0;
//		}
	}
}

void LT8618sx_System_Init(void)  //dsren
{
		HDMI_WriteI2C_Byte(0xFF,0x82);
		HDMI_WriteI2C_Byte(0x51,0x11);
		//Timer for Frequency meter
		HDMI_WriteI2C_Byte(0xFF,0x82);
		HDMI_WriteI2C_Byte(0x1b,0x69); //Timer 2
		HDMI_WriteI2C_Byte(0x1c,0x78);
		HDMI_WriteI2C_Byte(0xcb,0x69); //Timer 1
		HDMI_WriteI2C_Byte(0xcc,0x78);
		
		/*power consumption for work*/
		HDMI_WriteI2C_Byte(0xff,0x80); 
		HDMI_WriteI2C_Byte(0x04,0xf0);
		HDMI_WriteI2C_Byte(0x06,0xf0);
		HDMI_WriteI2C_Byte(0x0a,0x80);
		HDMI_WriteI2C_Byte(0x0b,0x46); //csc clk
		HDMI_WriteI2C_Byte(0x0d,0xef);
		HDMI_WriteI2C_Byte(0x11,0xfa);
}

/*
void main( void )
{
			GPIO_Initial();
			Uart1_Init();
#ifdef _PATTERN_TEST
	    RESET_Lt8618SX();
	    LT8618SX_Chip_ID();
	    LT8618sx_System_Init();
#else
			LT8618SX_Init();
			//IrqInit();
			while(1)
			{
			LT8618SX_Task();
			LT8618SX_INTB();
			}
#endif
}
*/