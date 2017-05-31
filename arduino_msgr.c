/*
 *  Project Arduino Messenger
 *
 *  arduino_msgr.c
 *	values are currently for the Atmega 8/168, which covers the Arduino Uno
 *
 *  Created by Klas Henriksson on 27/05/2017.
 *  Copyright C Klas Henriksson 2017. All rights reserverd.
 */


#include <string.h>
#include <fcntl.h>

#if defined(__APPLE__) || defined(__LINUX__)
#include <unistd.h>
#include <termios.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include "arduino_msgr.h"

#if defined(__APPLE__) || defined(__LINUX__)
#define ARDUINO_BASE_BAUD B9600
#else
#define ARDUINO_BASE_BAUD CBR_9600
#endif


static unsigned int mBaudrate = ARDUINO_BASE_BAUD;

#if defined(__APPLE__) || defined(__LINUX__)
static int mArduinoConnection = 0;
#elif defined(_WIN32)
static HANDLE mArduinoConnection = NULL;
#endif

static int mVerbose = 0;

int arduino_toggle_verbose()
{
    mVerbose = ~mVerbose & 0x1;
    return ARDUMSGR_OK;
}

int arduino_set_baud(unsigned int baud)
{
    
    if(mArduinoConnection != 0)
    {
        if(mVerbose)
            printf("Connection already established, please disconnect to change baud rate\n");
        return ARDUMSGR_ALREADY_CONNECTED;
    }
    
#if ARDUINO_SAFE == 1
    unsigned int supported_bauds[12] = { 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200 };
    
    char found = 0;
    for(int i = 0; i < 12; i++)
    {
        if(baud == supported_bauds[i])
        {
            found = 1;
            break;
        }
    }
    
    if(!found)
    {
        if(mVerbose)
            printf("Baud rate is not suppoerted! (see arduino serial page for list)\n");
        return ARDUMSGR_BAUD_UNSUPPORTED;
    }
#endif
    
    mBaudrate = baud;
    
    if(mVerbose)
        printf("Baud rate set to %u bps\n", baud);
    
    return ARDUMSGR_OK;
}

int arduino_connect(const char* serial)
{
    if(mArduinoConnection != 0)
        return ARDUMSGR_ALREADY_CONNECTED;
    
#if defined(__APPLE__) || defined(__LINUX__)
    
    if(access(serial, F_OK) == -1)
    {
        if(mVerbose)
            printf("Could not find serial connection..\n");
        return ARDUMSGR_CONNECT_FAIL;
    }
    
    mArduinoConnection = open(serial, O_RDWR | O_NDELAY | O_APPEND);
    
    struct termios options;
    tcgetattr(mArduinoConnection, &options);
    
    cfsetispeed(&options, mBaudrate);
    cfsetospeed(&options, mBaudrate);
    
    options.c_cflag |= CS8; //8 data bits
    
    tcsetattr(mArduinoConnection, TCSANOW, &options);
    
    if(mArduinoConnection < 1)
    {
        if(mVerbose)
            printf("Could not open serial connection (running root?)\n");
        
        mArduinoConnection = 0;
        return ARDUMSGR_CONNECT_FAIL;
    }
    
#elif defined(_WIN32)
	mArduinoConnection = CreateFileA(serial, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (mArduinoConnection == INVALID_HANDLE_VALUE)
	{
		if (mVerbose)
			printf("Failed to connect, windows system error code: %d\n", GetLastError()); //maybe add the most common error codes here

		mArduinoConnection = NULL;
		return ARDUMSGR_CONNECT_FAIL;
	}

	DCB dcb;
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	BOOL success = GetCommState(mArduinoConnection, &dcb);
	if (!success)
	{
		if (mVerbose)
			printf("Could not get communication state, windows system error code: %d\n", GetLastError());
		return ARDUMSGR_CONNECT_FAIL;
	}

	dcb.BaudRate = mBaudrate;
	dcb.ByteSize = 8; //as defined at http://www.arduino.cc/playground/Learning/ArduinoSpecs
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	success = SetCommState(mArduinoConnection, &dcb);
	if (!success)
	{
		if(mVerbose)
			printf("Could not apply communication settings, windows system error code: %d\n", GetLastError());
		return ARDUMSGR_CONNECT_FAIL;
	}

#endif
	if (mVerbose)
		printf("Established a connection to unit (baud rate %u, data bits: 8)\n", mBaudrate);

    return ARDUMSGR_OK;
}

int arduino_send(const char* data)
{
    if(!data)
        return ARDUMSGR_DATA_NULL;
    
    if(mArduinoConnection == 0)
    {
        if(mVerbose)
            printf("Send failed, no connection established (see connect)\n");
        return ARDUMSGR_NO_CONNECTION;
    }
    
    unsigned long len = strlen(data);

#if defined (__APPLE__) || defined(__LINUX__)
    long n = write(mArduinoConnection, data, len);
    
    if(n < 0)
    {
        if(mVerbose)
            printf("Could not write to arduino..");
        return ARDUMSGR_SEND_FAIL;
    }
    
    if(mVerbose)
        printf("Successfully sent %d bytes of data\n", (int)len);
    
    return ARDUMSGR_OK;
#elif defined(_WIN32)

	DWORD bytesWritten = 0;
	BOOL success = WriteFile(mArduinoConnection, data, len, &bytesWritten, NULL);

	if (!success)
	{
		if (mVerbose)
			printf("Could not write to arduino, windows system error code: %d\n", GetLastError());
		return ARDUMSGR_SEND_FAIL;
	}

	return ARDUMSGR_OK;
#endif

}

int arduino_disconnect()
{
    if(mArduinoConnection == 0)
        return ARDUMSGR_NO_CONNECTION;
    
#if defined(__APPLE__) || defined(__LINUX__)
    close(mArduinoConnection);
#elif defined(_WIN32)
	CloseHandle(mArduinoConnection);
#endif
    mArduinoConnection = 0;
    
    if(mVerbose)
        printf("Disconnected from unit\n");
    
    return ARDUMSGR_OK;
}
