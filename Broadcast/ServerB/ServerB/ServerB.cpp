#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Winsock2.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "WS2_32.lib")  

WSADATA wsaData;

SOCKET  sS;
SOCKADDR_IN serv, clnt;

char name[50] = "Hello";

bool  GetRequestFromClient(
    char* name, //[in] позывной сервера  
    short            port, //[in] номер просушиваемого порта 
    struct sockaddr* from, //[out] указатель на SOCKADDR_IN
    int* flen  //[out] указатель на размер from 
);

bool  PutAnswerToClient(
    char* name, //[in] позывной сервера  
    struct sockaddr* to,   //[in] указатель на SOCKADDR_IN
    int* lto   //[in] указатель на размер from 
);

void GetServer(
    const char* call, //[in] позывной сервера  
    short            port, //[in] номер порта сервера    
    struct sockaddr* from, //[out] указатель на SOCKADDR_IN
    int* flen  //[out] указатель на размер from 
);

int main()
{
    setlocale(LC_ALL, "rus");

    serv.sin_family = AF_INET;
    serv.sin_port = htons(2000);
    serv.sin_addr.s_addr = INADDR_ANY;

    memset(&clnt, 0, sizeof(clnt));
    int lc = sizeof(clnt);

    try {

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }

        GetServer(name, 2000, (sockaddr*)&clnt, &lc);

        if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }
        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
            throw SetErrorMsgText("bind: ", WSAGetLastError());
        }

        cout << "ServerB\n\n";

        while (true)
        {
            if (GetRequestFromClient(name, htons(2000), (sockaddr*)&clnt, &lc)) {
                PutAnswerToClient(name, (sockaddr*)&clnt, &lc);
            }           
        }

        if (closesocket(sS) == SOCKET_ERROR) {
            throw  SetErrorMsgText("closesocket: ", WSAGetLastError());
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

bool GetRequestFromClient(char* name, short port, struct sockaddr* from, int* flen) {
    char ibuf[50];
    int  lb = 0;

    try {
        while (true) {
            if ((lb = recvfrom(sS, ibuf, sizeof(ibuf), NULL, from, flen)) == SOCKET_ERROR) {
                throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
            }
            if (strcmp(name, ibuf) == 0) {
                struct sockaddr_in* client = (struct sockaddr_in*) from;
                cout << "Client IP: " << inet_ntoa(client->sin_addr) << endl;
                return true;
            } 
        }
    }
    catch (int errCode) {
        return errCode == WSAETIMEDOUT ? false : throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
    }
}

bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto) {
    if (sendto(sS, name, sizeof(name) + 1, NULL, to, *lto) == SOCKET_ERROR) {
        throw SetErrorMsgText("sendto: ", WSAGetLastError());
    }
}

void GetServer(const char* call, short port, struct sockaddr* from, int* flen) {
    try {
        SOCKET cC;
        int optval = 1, lb = 0, lobuf = 0, count = 0;
        char buf[50];

        SOCKADDR_IN all;                        // параметры  сокета sS
        all.sin_family = AF_INET;               // используется IP-адресация  
        all.sin_port = htons(port);             // порт 2000
        all.sin_addr.s_addr = INADDR_BROADCAST; // всем 

        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket:", WSAGetLastError());
        }

        if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
            throw  SetErrorMsgText("setsocketopt:", WSAGetLastError());
        }

        if ((lb = sendto(cC, call, sizeof(call) + 2, NULL, (sockaddr*)&all, sizeof(all))) == SOCKET_ERROR) {
            throw SetErrorMsgText("sendto:", WSAGetLastError());
        }

        timeval timeout;
        timeout.tv_sec = 2000;
        timeout.tv_usec = 0;

        setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

        while (lb = recvfrom(cC, buf, sizeof(buf), NULL, from, flen) != SOCKET_ERROR)
        {
            if (strcmp(buf, name) == 0) {
                cout << "Server IP: " << inet_ntoa(((SOCKADDR_IN*)from)->sin_addr) << endl;
                count++;
            }
        }
        cout << "Find servers " << count << endl;
        if (closesocket(cC) == SOCKET_ERROR) {
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
    }
    catch (string errorMsgText) {
        cout << endl << errorMsgText;
    }
}