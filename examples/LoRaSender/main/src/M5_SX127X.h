// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.
// Copyright (c) 2024 M5Stack Technology CO LTD
#ifndef _M5_SX127X_H_
#define _M5_SX127X_H_

// #include <Arduino.h>
// #include <SPI.h>
#include "c_periphery/spi.h"
#include "c_periphery/gpio.h"
#include <iostream>
#include <unistd.h>
#define LORA_DEFAULT_SPI           SPI
#define LORA_DEFAULT_SPI_FREQUENCY 8E6
#define LORA_DEFAULT_SS_PIN        10
#define LORA_DEFAULT_RESET_PIN     9
#define LORA_DEFAULT_DIO0_PIN      2

#define PA_OUTPUT_RFO_PIN      0
#define PA_OUTPUT_PA_BOOST_PIN 1

class M5_SX127X {
   public:
    M5_SX127X();

    int begin(long frequency);
    void end();

    void setSPI(void* spi);

    int beginPacket(int implicitHeader = false);
    int endPacket(bool async = false);

    int parsePacket(int size = 0);
    int packetRssi();
    float packetSnr();
    long packetFrequencyError();

    int rssi();

    // from Print
    // size_t write(uint8_t byte);
    size_t hw_write(const uint8_t* buffer, size_t size);

    // from Stream
    int available();
    int read();
    int peek();
    void flush();

#ifndef ARDUINO_SAMD_MKRWAN1300
    void onReceive(void (*callback)(int));
    void onCadDone(void (*callback)(bool));
    void onTxDone(void (*callback)());

    void receive(int size = 0);
    void channelActivityDetection(void);
#endif
    void hw_idle();
    void hw_sleep();

    void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
    void setFrequency(long frequency);
    void setSpreadingFactor(int sf);
    void setSignalBandwidth(long sbw);
    void setCodingRate4(int denominator);
    void setPreambleLength(long length);
    void setSyncWord(int sw);
    void enableCrc();
    void disableCrc();
    void enableInvertIQ();
    void disableInvertIQ();
    void enableLowDataRateOptimize();
    void disableLowDataRateOptimize();

    void setOCP(uint8_t mA);  // Over Current Protection control

    void setGain(uint8_t gain);  // Set LNA gain

    // deprecated
    void crc() {
        enableCrc();
    }
    void noCrc() {
        disableCrc();
    }

    uint8_t random();

    void setPins(int ss    = LORA_DEFAULT_SS_PIN,
                 int reset = LORA_DEFAULT_RESET_PIN,
                 int dio0  = LORA_DEFAULT_DIO0_PIN);
    // void setSPI(SPIClass& spi);
    // void setSPIFrequency(uint32_t frequency);

    void dumpRegisters();

   private:
    void explicitHeaderMode();
    void implicitHeaderMode();

    void handleDio0Rise();
    bool isTransmitting();

    int getSpreadingFactor();
    long getSignalBandwidth();

    void setLdoFlag();
    void setLdoFlagForced(const bool);

    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);
    uint8_t singleTransfer(uint8_t address, uint8_t value);

    static void onDio0Rise();

   private:
    // SPISettings _spiSettings;
    void* _spi;
    void* irq;
    void* rst;
    int _ss;
    int _reset;
    int _dio0;
    long _frequency;
    int _packetIndex;
    int _implicitHeaderMode;
    void (*_onReceive)(int);
    void (*_onCadDone)(bool);
    void (*_onTxDone)();
};

extern M5_SX127X LoRa;

#endif
