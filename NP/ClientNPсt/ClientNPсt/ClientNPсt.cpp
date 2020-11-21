#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Windows.h"

#define STOP "STOP"
#define NAME L"\\\\.\\pipe\\Tube"

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    HANDLE cH;
    DWORD lp;
    char ibuf[50], obuf[50];

    try {
        cout << "ClientNPсt\n\n";

        int countMessage;
        cout << "Number of messages: ";
        cin >> countMessage;

        for (int i = 1; i <= countMessage; i++) {
            string obufstr = "Hello from ClientNPct " + to_string(i);
            strcpy_s(obuf, obufstr.c_str());

            if (!CallNamedPipe(NAME, obuf, sizeof(obuf), ibuf, sizeof(ibuf), &lp, NMPWAIT_WAIT_FOREVER)) {
                throw SetPipeError("CallNamedPipe: ", GetLastError());
            }

            cout << ibuf << endl;
        }

        system("pause");
    }
    catch (string ErrorPipeText) {
        cout << endl << ErrorPipeText;
    }
}