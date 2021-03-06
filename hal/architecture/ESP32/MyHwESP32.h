/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2017 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * Arduino core for ESP32: https://github.com/espressif/arduino-esp32
 *
 * MySensors ESP32 implementation by tekka
 *
 * Radio wiring ESP32(Node32s): RF24, RFM69, RFM95:
 * MOSI	23
 * MISO	19
 * SCK	18
 * CSN	5
 * CE	17 (RF24) [default]
 * IRQ	16 (opt. RF24, RFM69, RFM95) [default]
 *
 */

#ifndef MyHwESP32_h
#define MyHwESP32_h

#include <WiFi.h>

#ifdef __cplusplus
#include <Arduino.h>
#endif

#define MY_SERIALDEVICE Serial

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define MIN min
#define MAX max

#define MY_EEPROM_SIZE 1024

// Define these as macros to save valuable space
#define hwDigitalWrite(__pin, __value) digitalWrite(__pin, __value)
#define hwDigitalRead(__pin) digitalRead(__pin)
#define hwPinMode(__pin, __value) pinMode(__pin, __value)

#define hwWatchdogReset()
#define hwReboot() ESP.restart()
#define hwMillis() millis()
#define hwRandomNumberInit() randomSeed(esp_random())

bool hwInit(void);
void hwReadConfigBlock(void* buf, void* adr, size_t length);
void hwWriteConfigBlock(void* buf, void* adr, size_t length);
void hwWriteConfig(int adr, uint8_t value);
uint8_t hwReadConfig(int adr);
ssize_t hwGetentropy(void *__buffer, size_t __length);
#define MY_HW_HAS_GETENTROPY

/**
* Restore interrupt state.
* Helper function for MY_CRITICAL_SECTION.
*/
static __inline__ void __psRestore(const uint32_t *__s)
{
	XTOS_RESTORE_INTLEVEL(*__s);
}

#ifndef DOXYGEN
#define MY_CRITICAL_SECTION    for ( uint32_t __psSaved __attribute__((__cleanup__(__psRestore))) = XTOS_DISABLE_ALL_INTERRUPTS, __ToDo = 1; __ToDo ; __ToDo = 0 )
#endif  /* DOXYGEN */


#endif
