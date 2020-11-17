#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Windows.h"

#define NAME L"\\\\.\\mailslot\\Box"

using namespace std;

int main()
{
    HANDLE sH;
    DWORD rb, time = 1800000, nMaxMessageSize = 1000;
    clock_t start, end;
    char rbuf[300];
    bool flag = true;

    try {
        cout << "ServerMS\n\n";

        if ((sH = CreateMailslot(NAME, nMaxMessageSize, time, NULL)) == INVALID_HANDLE_VALUE) {
            throw SetPipeError("CreateMailslot: ", GetLastError());
        }

        while (strcmp(rbuf, "STOP") != 0) {
            if (flag) {
                start = clock();
                flag = false;
            }

            if (!ReadFile(sH, rbuf, sizeof(rbuf), &rb, NULL)) {
                throw SetPipeError("ReadFile: ", GetLastError());
            }

            cout << rbuf << endl;
        }
        end = clock();
        cout << "Time for send and recv: " << ((double)(end - start) / CLK_TCK) << " c" << endl;

        if (!CloseHandle(sH)) {
            throw SetPipeError("CloseHandle: ", GetLastError());
        }

        system("pause");
    }
    catch (string ErrorPipeText) {
        cout << endl << GetLastError();
    }
}

