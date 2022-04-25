// NMEASerial.c : This file contains the 'main' function. Program execution begins and ends there.
//

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "serial_nmea.h"

#include <stdio.h>

const char* const test_nmea_nofix =
"$GPRMC,200000.00,V,,,,,,,240422,,,N\r\n"
"$GPVTG,,,,,,,,,N*30\r\n"
"$GPGGA,200000.00,,,,,0,00,99.99,,,,,,*64\r\n"
"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99\r\n"
"$GPGSV,4,1,16,02,49,262,19,03,05,036,,05,01,199,11,06,68,000,28*7E\r\n"
"$GPGSV,4,2,16,09,00,101,,11,52,251,,12,28,319,,14,25,144,25*76\r\n"
"$GPGSV,4,3,16,17,40,065,,19,51,041,30,20,28,190,23,24,20,269,*7A\r\n"
"$GPGSV,4,4,16,28,53,143,,46,34,241,,48,37,237,,51,50,216,*7C\r\n"
"$GPGLL,,,,,200000.00,V,N*48\r\n";

int main()
{
	printf("starting...\r\n");

	// Open serial port
	HANDLE serialHandle;

	serialHandle = CreateFile(L"\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	// Do some basic settings
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serialHandle, &serialParams);
	serialParams.BaudRate = 9600;
	serialParams.ByteSize = 8;
	serialParams.StopBits = ONESTOPBIT;
	serialParams.Parity = NOPARITY;
	SetCommState(serialHandle, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;

	SetCommTimeouts(serialHandle, &timeout);

	char nmea_message_buf[NMEA_MESSAGE_MAX];
	nmea.state = ST_INIT,
		nmea.message = nmea_message_buf;
	nmea.message_len = 0;
	nmea.message_max = NMEA_MESSAGE_MAX;

	wchar_t inBuf = 0;
	DWORD nBytesToRead = 0;
	DWORD nBytesRead = 0;

	for (size_t i = 0; test_nmea_nofix[i]; ++i) {
		process_char((char)test_nmea_nofix[i]);
	}

	while (1) {
		if (ReadFile(serialHandle, &inBuf, 1, &nBytesRead, NULL)) {
			if (nBytesRead)
				process_char((char)inBuf);
		}
		else {
			break;
		}
	}

	CloseHandle(serialHandle);
	return 0;
}
