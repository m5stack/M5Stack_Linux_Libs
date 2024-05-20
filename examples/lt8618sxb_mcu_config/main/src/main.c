#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include "lt8618sx.h"
#include <stdlib.h>


hdmi_lt8618sx_t lt8618;
int main(int argc, char *argv[])
{
	HDMI_I2C_INIT();
	LT8618SX_Init(&lt8618);
	HDMI_I2C_DEINIT();
	return 0;
}