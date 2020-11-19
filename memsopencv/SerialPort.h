// https://github.com/ZainUlMustafa/Connect-And-Use-Arduino-via-Cpp-Software-Made-In-Any-IDE/tree/master/Arduino2PC_SC
#pragma once
#ifndef SERIALPORT_H
#define SERIALPORT_H

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class SerialPort
{
private:
    HANDLE handler;
    bool connected;
    COMSTAT status;
    DWORD errors;
public:
    SerialPort(char* portName);
    ~SerialPort();

    int readSerialPort(char* buffer, unsigned int buf_size);
    bool writeSerialPort(char* buffer, unsigned int buf_size);
    bool isConnected();
};

void serialcomm(int&, SerialPort&);

#endif // SERIALPORT_H