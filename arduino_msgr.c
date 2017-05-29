/*
 *  Project Arduino Messenger
 *
 *  arduino_msgr.c
 *
 *  Created by Klas Henriksson on 27/05/2017.
 *  Copyright C Klas Henriksson 2017. All rights reserverd.
 */


#include <string.h>
#include "arduino_msgr.h"


static FILE* mArduinoConnection = NULL;
static int mVerbose = 0;

int arduino_toggle_verbose()
{
    mVerbose = ~mVerbose & 0x1;
    return ARDUMSGR_OK;
}

int arduino_connect(const char* serial)
{
    if(mArduinoConnection != NULL)
        return ARDUMSGR_ALREADY_CONNECTED;
    
    mArduinoConnection = fopen(serial, "w+");
    if(!mArduinoConnection)
    {
        return ARDUMSGR_CONNECT_FAIL;
    }
    
    if(mVerbose)
        printf("Established a connection to unit\n");
    
    return ARDUMSGR_OK;
}

int arduino_send(const char* data)
{
    if(!data)
        return ARDUMSGR_DATA_NULL;
    
    if(mArduinoConnection == NULL)
        return ARDUMSGR_NO_CONNECTION;
    
    unsigned long len = strlen(data);
    fwrite(data, sizeof(char), len, mArduinoConnection);
    
    if(mVerbose)
        printf("Successfully sent %d bytes of data\n", (int)len);
    
    return ARDUMSGR_OK;
}

int arduino_disconnect()
{
    if(mArduinoConnection == NULL)
        return ARDUMSGR_NO_CONNECTION;
    
    fclose(mArduinoConnection);
    mArduinoConnection = NULL;
    
    if(mVerbose)
        printf("Disconnected from unit\n");
    
    return ARDUMSGR_OK;
}
