/*
 *  Project Arduino Messenger
 *
 *  arduino_msgr.h
 *      represents a arduino serial connection
 *
 *  Copyright C Klas Henriksson 2017. All rights reserverd.
 */

#ifndef arduino_msgr_h
#define arduino_msgr_h

#define ARDUINO_SAFE 0

enum arduino_messenger_error_codes
{
    ARDUMSGR_CONNECT_FAIL,
    ARDUMSGR_ALREADY_CONNECTED,
    ARDUMSGR_SEND_FAIL,
    ARDUMSGR_NO_CONNECTION,
    ARDUMSGR_DATA_NULL,
    ARDUMSGR_BAUD_UNSUPPORTED,
    ARDUMSGR_OK,
};

#include <stdio.h>

int arduino_toggle_verbose();
int arduino_set_baud(unsigned int baud);
int arduino_connect(const char* serial);
int arduino_send(const char* data);
//char* arduino_read();
int arduino_disconnect();

#endif /* arduino_msgr_h */
