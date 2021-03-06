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
 */

#include "MyHwESP32.h"
#include "EEPROM.h"

bool hwInit(void)
{
#if !defined(MY_DISABLED_SERIAL)
	MY_SERIALDEVICE.begin(MY_BAUD_RATE, SERIAL_8N1);
#endif
	return EEPROM.begin(MY_EEPROM_SIZE);
}

void hwReadConfigBlock(void* buf, void* adr, size_t length)
{
	uint8_t* dst = static_cast<uint8_t*>(buf);
	int offs = reinterpret_cast<int>(adr);
	while (length-- > 0) {
		*dst++ = EEPROM.read(offs++);
	}
}

void hwWriteConfigBlock(void* buf, void* adr, size_t length)
{
	uint8_t* src = static_cast<uint8_t*>(buf);
	int offs = reinterpret_cast<int>(adr);
	while (length-- > 0) {
		EEPROM.write(offs++, *src++);
	}
	EEPROM.commit();
}

uint8_t hwReadConfig(int adr)
{
	uint8_t value;
	hwReadConfigBlock(&value, reinterpret_cast<void*>(adr), 1);
	return value;
}

void hwWriteConfig(int adr, uint8_t value)
{
	uint8_t curr = hwReadConfig(adr);
	if (curr != value) {
		hwWriteConfigBlock(&value, reinterpret_cast<void*>(adr), 1);
	}
}

bool hwUniqueID(unique_id_t *uniqueID)
{
	uint64_t val = ESP.getEfuseMac();
	(void)memcpy((uint8_t*)uniqueID, &val, 8);
	(void)memset((uint8_t*)uniqueID + 8, 0x0A, 8); // padding
	return true;
}

ssize_t hwGetentropy(void *__buffer, size_t __length)
{
	// cut length if > 256
	if (__length > 256) {
		__length = 256;
	}
	uint8_t *dst = (uint8_t *)__buffer;
	// get random numbers
	for (size_t i = 0; i < __length; i++) {
		dst[i] = (uint8_t)esp_random();
	}
	return __length;
}

int8_t hwSleep(uint32_t ms)
{
	// TODO: Not supported!
	(void)ms;
	return MY_SLEEP_NOT_POSSIBLE;
}

int8_t hwSleep(uint8_t interrupt, uint8_t mode, uint32_t ms)
{
	// TODO: Not supported!
	(void)interrupt;
	(void)mode;
	(void)ms;
	return MY_SLEEP_NOT_POSSIBLE;
}

int8_t hwSleep(uint8_t interrupt1, uint8_t mode1, uint8_t interrupt2, uint8_t mode2,
               uint32_t ms)
{
	// TODO: Not supported!
	(void)interrupt1;
	(void)mode1;
	(void)interrupt2;
	(void)mode2;
	(void)ms;
	return MY_SLEEP_NOT_POSSIBLE;
}

uint16_t hwCPUVoltage(void)
{
	// in mV
	return FUNCTION_NOT_SUPPORTED;
}

uint16_t hwCPUFrequency(void)
{
	// in 1/10Mhz
	return ESP.getCpuFreqMHz() * 10;
}

uint8_t hwCPUTemperature(void)
{
	// CPU temperature in °C
	return (uint8_t)temperatureRead();
}

uint16_t hwFreeMem(void)
{
	return (uint16_t)ESP.getFreeHeap();
}

void hwDebugPrint(const char *fmt, ...)
{
	char fmtBuffer[MY_SERIAL_OUTPUT_SIZE];
#ifdef MY_GATEWAY_SERIAL
	// prepend debug message to be handled correctly by controller (C_INTERNAL, I_LOG_MESSAGE)
	snprintf_P(fmtBuffer, sizeof(fmtBuffer), PSTR("0;255;%d;0;%d;%lu "), C_INTERNAL, I_LOG_MESSAGE,
	           hwMillis());
	MY_SERIALDEVICE.print(fmtBuffer);
#else
	// prepend timestamp
	MY_SERIALDEVICE.print(hwMillis());
	MY_SERIALDEVICE.print(" ");
#endif
	va_list args;
	va_start(args, fmt);
#ifdef MY_GATEWAY_SERIAL
	// Truncate message if this is gateway node
	vsnprintf_P(fmtBuffer, sizeof(fmtBuffer), fmt, args);
	fmtBuffer[sizeof(fmtBuffer) - 2] = '\n';
	fmtBuffer[sizeof(fmtBuffer) - 1] = '\0';
#else
	vsnprintf_P(fmtBuffer, sizeof(fmtBuffer), fmt, args);
#endif
	va_end(args);
	MY_SERIALDEVICE.print(fmtBuffer);
	MY_SERIALDEVICE.flush();
}
