#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    WSADATA wsaData;
    SOCKET cC;

    try {
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }

        SOCKADDR_IN serv; 
        int ls = sizeof(serv);
        serv.sin_family = AF_INET;    
        serv.sin_port = htons(2000);   
        serv.sin_addr.s_addr = inet_addr("192.168.100.8");                

        clock_t start, end;
        char ibuf[50] = "server: принято ";
        int  libuf = 0, lobuf  = 0;

        int countMessage;
        cout << "Number of messages: ";
        cin >> countMessage;

        start = clock();
        for (int i = 1; i <= countMessage; i++) {
            string obuf = "Hello from Client " + to_string(i);

            if ((libuf = sendto(cC, obuf.c_str(), obuf.length() + 1, NULL,
                (SOCKADDR*)&serv, sizeof(serv))) == SOCKET_ERROR) {
                throw  SetErrorMsgText("sendto: ", WSAGetLastError());
            }

            //if ((lobuf = recvfrom(cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)&serv, &ls)) == SOCKET_ERROR) {
            //    throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
            //}

            cout << obuf << endl;
        }
        end = clock();
        string obuf = "";

        if ((libuf = sendto(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL,
            (SOCKADDR*)&serv, sizeof(serv))) == SOCKET_ERROR) {
            throw  SetErrorMsgText("sendto: ", WSAGetLastError());
        }

        cout << "Time for sendto and recvfrom: " << ((double)(end - start) / CLK_TCK) << " c" << endl;

        if (closesocket(cC) == SOCKET_ERROR) {
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
        if (WSACleanup() == SOCKET_ERROR) {
            throw  SetErrorMsgText("Cleanup: ", WSAGetLastError());
        }
    }
    catch (string errorMsgText) {
        cout << endl << errorMsgText;
    }

    system("pause");
    return 0;
}

