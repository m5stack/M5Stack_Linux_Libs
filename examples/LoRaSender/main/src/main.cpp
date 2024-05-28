/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "M5_SX127X.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CS_PIN 5
#define RST_PIN 13
#define IRQ_PIN 34

#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SCLK 18

// LoRa Parameters Config
#define LORA_FREQ 868E6
#define LORA_SF 12
#define LORA_BW 125E3
#define LORA_TX_POWER 17

int counter = 0;

void setup()
{
  // Serial.begin(115200);

  printf("LoRa Sender\n");
  // SPI.begin(LORA_SCLK, LORA_MISO, LORA_MOSI, -1);  // SCK, MISO, MOSI, SS
  // LoRa.setSPI(&SPI);
  // LoRa.setPins(CS_PIN, RST_PIN, IRQ_PIN);  // set CS, reset, IRQ pin
  //   while (!LoRa.begin(LORA_FREQ)) {
  //     printf("LoRa init fail.\n");
  //     delay(1000);
  //   }
  LoRa.begin(LORA_FREQ);

  LoRa.setTxPower(LORA_TX_POWER);
  LoRa.setSignalBandwidth(LORA_BW);
  LoRa.setSpreadingFactor(LORA_SF);

  //   LoRa.dumpRegisters();
}

void loop()
{
  char msg_buffer[64];
  printf("Sending packet: ");
  printf("%d\n", counter);

  // send packet
  LoRa.beginPacket();
  //   LoRa.print("hello ");
  //   LoRa.print(counter);

  size_t len = sprintf(msg_buffer, "hello %d", counter);
  LoRa.hw_write((const uint8_t *)msg_buffer, len);
  LoRa.endPacket();

  counter++;

  //   delay(5000);
  sleep(5);
}

int main(int argc, char *argv[])
{
  setup();
  for (;;)
    loop();

  return 0;
}
