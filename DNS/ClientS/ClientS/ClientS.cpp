#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")

#define CALL "Hello"
#define NAME "WIN-C5B264JT20J"
#define PORT 2000

using namespace std;

bool  GetServerByName(
    const char* name, //[in] имя компьютера в сети  
    const char* call, //[in] позывной   
    struct sockaddr* from, //[in,out] указатель на SOCKADDR_IN
    int* flen  //[in,out] указатель на размер from 
);

int main()
{
    setlocale(LC_ALL, "rus");

    WSADATA wsaData;
    SOCKADDR_IN clnt;
    int lc = sizeof(clnt);

    try {
        cout << "ClientS\n\n";

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }

        GetServerByName(NAME, CALL, (sockaddr*)&clnt, &lc);

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

bool GetServerByName(const char* name, const char* call, struct sockaddr* from, int* flen) {
    hostent* host = gethostbyname(name);

    try {
        SOCKET cC;
        int optval = 1, lb = 0, lobuf = 0;
        char buf[50];
        in_addr* addr_list = (in_addr*)*(host->h_addr_list);

        SOCKADDR_IN all;                        // параметры  сокета sS
        all.sin_family = AF_INET;               // используется IP-адресация  
        all.sin_port = htons(PORT);             // порт 2000
        all.sin_addr = *addr_list;

        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket:", WSAGetLastError());
        }
        if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
            throw  SetErrorMsgText("setsocketopt:", WSAGetLastError());
        }
        if ((lb = sendto(cC, call, sizeof(call) + 2, NULL, (sockaddr*)&all, sizeof(all))) == SOCKET_ERROR) {
            throw SetErrorMsgText("sendto:", WSAGetLastError());
        }
        if (lobuf = recvfrom(cC, buf, sizeof(buf), NULL, (sockaddr*)from, flen) == SOCKET_ERROR) {
            throw SetErrorMsgText("recvfrom:", WSAGetLastError());
        }

        struct sockaddr_in* server = (struct sockaddr_in*) from;
        cout << "Server port: " << ntohs(server->sin_port) << endl;
        cout << "Server IP: " << inet_ntoa(*addr_list) << endl;

        if (closesocket(cC) == SOCKET_ERROR) {
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());
        }

        return strcmp(call, buf) == 0 ? true : false;
    }
    catch (int errCode) {
        return errCode == WSAETIMEDOUT ? false : throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
    }
}