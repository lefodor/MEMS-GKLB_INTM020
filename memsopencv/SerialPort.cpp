
// https://github.com/ZainUlMustafa/Connect-And-Use-Arduino-via-Cpp-Software-Made-In-Any-IDE/tree/master/Arduino2PC_SC
#include "SerialPort.h"
#include "init.h"
#include <iostream>

SerialPort::SerialPort(char* portName)
{
    this->connected = false;

    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (this->handler == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        }
        else
        {
            printf("ERROR!!!");
        }
    }
    else {
        DCB dcbSerialParameters = { 0 };

        if (!GetCommState(this->handler, &dcbSerialParameters)) {
            printf("failed to get current serial parameters");
        }
        else {
            dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters))
            {
                printf("ALERT: could not set Serial port parameters\n");
            }
            else {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
}

SerialPort::~SerialPort()
{
    if (this->connected) {
        this->connected = false;
        CloseHandle(this->handler);
    }
}

int SerialPort::readSerialPort(char* buffer, unsigned int buf_size)
{
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0) {
        if (this->status.cbInQue > buf_size) {
            toRead = buf_size;
        }
        else toRead = this->status.cbInQue;
    }

    if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

    return 0;
}

bool SerialPort::writeSerialPort(char* buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) {
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    }
    else return true;
}

bool SerialPort::isConnected()
{
    return this->connected;
}

void serialcomm(int& coord, SerialPort& arduino)
{
    //char output[MAX_DATA_LENGTH];
    //char incomingData[MAX_DATA_LENGTH];

    // change the name of the port with the port name of your computer
    // must remember that the backslashes are essential so do not remove them
    // char port[] = "\\\\.\\COM3";

    /*
    SerialPort arduino(port);
    if (arduino.isConnected()) {
        std::cout << "Connection made" << std::endl;
    }
    else {
        std::cout << "Error in port name" << std::endl;
    }
    */
    if (arduino.isConnected()) {
        //std::cout << "Enter your command: " << std::endl;
        //std::string data = getLine();
        std::string data = std::to_string(coord);

        char* charArray = new char[data.size() + 1];
        //char* charArray = new char[data.size()];
        copy(data.begin(), data.end(), charArray);
        charArray[data.size()] = '\n';
        //charArray[data.size()+1] = '\n';

        arduino.writeSerialPort(charArray, MAX_DATA_LENGTH);

        delete[] charArray;
    }
}