/*
 * Simple Linux wrapper for access to /dev/spidev
 * File: "spi_test.c" - test module
 */

//-----------------------------------------------------------------------------
#include "linux_spi/linux_spi.h"
#include <stdio.h>
//-----------------------------------------------------------------------------
#define SPI_DEVICE "/dev/spidev0.0"
//#define SPI_DEVICE "/dev/spidev1.0"
//-----------------------------------------------------------------------------
int main()
{
  spi_t spi;
  char buf[1024];
  int i;

  int retv = spi_init(&spi,
                      SPI_DEVICE, // filename like "/dev/spidev0.0"
                      0,          // SPI_* (look "linux/spi/spidev.h")
                      0,          // bits per word (usually 8)
                      2500000);   // max speed [Hz]

  printf(">>> spi_init() return %d\n", retv);

  retv = spi_read(&spi, buf, 1024);
  printf(">>> spi_read(1024) return %d\n", retv);

  for (i = 0; i < 1024; i++)
    buf[i] = 0x55;

  retv = spi_write(&spi, buf, 1024);
  printf(">>> spi_write(1024) return %d\n", retv);

  spi_free(&spi);

  return 0;
}

/*** end of "spi_test.c" file ***/