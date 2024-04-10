#pragma once

extern char uart_dev[20];
extern int debugging;
extern int Debug_s;
#define HAVE_MAIN 1
#define LOG_PRINT(fmt, ...) do{\
	if(Debug_s)\
	{\
		printf(fmt"  [info:%s:%d] [%s]\n", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__);\
	}\
}while(0);

void server_main();
void main_clint();