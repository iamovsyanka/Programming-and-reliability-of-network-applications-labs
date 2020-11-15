#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Windows.h"

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    HANDLE sH; 
    DWORD lp;
    char buf[50];

    try  {
        cout << "ServerNP" << endl << endl;

        if ((sH = CreateNamedPipe(L"\\\\.\\pipe\\Tube",
            PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT,
            1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE) {
            throw SetPipeError("CreateNamedPipe: ", GetLastError());
        }

        cout << "Waiting for connect...\n";
        if (!ConnectNamedPipe(sH, NULL)) {
            throw SetPipeError("ConnectNamedPipe: ", GetLastError());
        }


        while (true) {
            if (ReadFile(sH, buf, sizeof(buf), &lp, NULL)) {
                cout << buf << endl;

                if (strcmp(buf, "STOP") == 0) {
                    break;
                }
                if (!WriteFile(sH, buf, sizeof(buf), &lp, NULL)) {
                    throw SetPipeError("WriteFile: ", GetLastError());
                }
            }
            else {
                throw SetPipeError("ReadFile: ", GetLastError());
            }
        }

        if (!DisconnectNamedPipe(sH)) {
            throw SetPipeError("DisconnectNamedPipe: ", GetLastError());
        }

        if (!CloseHandle(sH)) {
            throw SetPipeError("CloseHandle: ", GetLastError());
        }
    }
    catch (string ErrorPipeText) {
        cout << endl << ErrorPipeText;
    }
}