/*
 *  Project Arduino Messenger
 *
 *  main.cpp
 *
 *  Copyright C Klas Henriksson 2017. All rights reserverd.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "arduino_msgr.h"

static int quit = 0;

int print_usage();
int parse_command(const char* arg);

int command_verbose();
int command_baudrate();
int command_connect();
int command_send();
int command_disconnect();
int command_quit();

int main(int argc, const char * argv[]) {
    
    if(argc == 1)
        print_usage();
    
    char cmdBuff[124];
    
    while(quit == 0)
    {
        printf("enter command: ");
        scanf("%s", cmdBuff);
        
        int error = parse_command(cmdBuff);
        if(error != ARDUMSGR_OK)
            printf("[!] failed to execute command, error code: %d\n", error);
        
        memset(cmdBuff, 0, 124);
    }
    
}

int print_usage()
{
    printf("arduino-messenger by bajsko\n"
           "available commands:\n"
           "verbose - toggle verbosity (default to 0)\n"
           "baudrate - set baud rate (defaults to 9600)\n"
           "connect {PATH} - establish a connection to arduino serial\n"
           "send {DATA} - send data to arduino (connection must be established)\n"
           "read - prints data receieved (connection must be established)\n"
           "disconnect - disconnect from arduino\n"
           "help - display this\n"
           "quit - exit application\n\n");
    
    return ARDUMSGR_OK;
}

int command_verbose()
{
    return arduino_toggle_verbose();
}

int parse_command(const char* arg)
{
    const char* cmd = arg;
    
    if(strcmp(cmd, "verbose") == 0)
        return command_verbose();
    if(strcmp(cmd, "baudrate") == 0)
        return command_baudrate();
    else if(strcmp(cmd, "connect") == 0)
        return command_connect();
    else if(strcmp(cmd, "send") == 0)
        return command_send();
    else if(strcmp(cmd, "disconnect") == 0)
        return command_disconnect();
    else if(strcmp(cmd, "help") == 0)
        return print_usage();
    else if(strcmp(cmd, "quit") == 0)
        return command_quit();
    
    return 0;
}

int command_baudrate()
{
    char buff[124];
    scanf("%s", buff);
    
    unsigned int baud = 0;
    baud = (unsigned int)strtoul(buff, NULL, 10);
    
    return arduino_set_baud(baud);
}

int command_connect()
{
    char buff[1024];
    //printf("connect to?: ");
    scanf("%s", buff);
    
    int error = arduino_connect(buff);
    
    return error;
}

int command_send()
{
    char buff[124];
    //printf("send what?: ");
    scanf("%s", buff);
    
    return arduino_send(buff);
}

int command_disconnect()
{
    return arduino_disconnect();
}

int command_quit()
{
	arduino_toggle_verbose();
	arduino_disconnect();

    quit = 1;
    printf("=================BYE==================\n");
    return ARDUMSGR_OK;
}
