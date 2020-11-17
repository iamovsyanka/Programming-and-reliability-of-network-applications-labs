#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Windows.h"

#define NAME L"\\\\.\\pipe\\Tube"
#define STOP "STOP"

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    HANDLE cH;
    DWORD lp;
    char buf[50];

    try {
        cout << "ClientNP\n\n";

        if ((cH = CreateFile(NAME, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE) {
            throw  SetPipeError("CreateFile: ", GetLastError());
        }

        int countMessage;
        cout << "Number of messages: ";
        cin >> countMessage;

        for (int i = 1; i <= countMessage; i++) {
            string obuf = "Hello from ClientNP " + to_string(i);

            if (!WriteFile(cH, obuf.c_str(), sizeof(obuf), &lp, NULL)) {
                throw SetPipeError("WriteFile: ", GetLastError());
            }
            if (!ReadFile(cH, buf, sizeof(buf), &lp, NULL)) {
                throw SetPipeError("ReadFile: ", GetLastError());
            }

            cout << buf << endl;
        }

        if (!WriteFile(cH, STOP, sizeof(STOP), &lp, NULL)) {
            throw SetPipeError("WriteFile: ", GetLastError());
        }
        if (!CloseHandle(cH)) {
            throw SetPipeError("CloseHandle: ", GetLastError());
        }

        system("pause");
    }
    catch (string ErrorPipeText) {
        cout << endl << ErrorPipeText;
    }
}