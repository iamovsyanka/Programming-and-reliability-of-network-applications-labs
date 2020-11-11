#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Winsock2.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "WS2_32.lib")

#define CALL  "Hello"
#define PORT 2000

WSADATA wsaData;

SOCKET  sS;
SOCKADDR_IN serv, clnt;


char buf[50];

bool  GetRequestFromClient(
    const char* name, //[in] позывной сервера  
    short            port, //[in] номер просушиваемого порта 
    struct sockaddr* from, //[out] указатель на SOCKADDR_IN
    int* flen  //[out] указатель на размер from 
);

bool  PutAnswerToClient(
    const char* name, //[in] позывной сервера  
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
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = INADDR_ANY;

    memset(&clnt, 0, sizeof(clnt));
    int lc = sizeof(clnt);

    try {
        cout << "ServerB\n\n";

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }

        GetServer(CALL, PORT, (sockaddr*)&clnt, &lc);

        if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }
        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
            throw SetErrorMsgText("bind: ", WSAGetLastError());
        }

        while (true)
        {
            if (GetRequestFromClient(CALL, PORT, (sockaddr*)&clnt, &lc)) {
                hostent* hostnameClient = gethostbyaddr((char*)&clnt.sin_addr, sizeof(clnt.sin_addr), AF_INET);
                cout << "Client hostname: " << hostnameClient->h_name << endl;
                cout << "~~~~~~~~~~~~\n\n";

                PutAnswerToClient(CALL, (sockaddr*)&clnt, &lc);
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

bool GetRequestFromClient(const char* name, short port, struct sockaddr* from, int* flen) {
    char ibuf[6], hostname[50];
    int  lb = 0;

    try {
        while (true) {
            if ((lb = recvfrom(sS, ibuf, sizeof(ibuf), NULL, from, flen)) == SOCKET_ERROR) {
                throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
            }
            if (strcmp(name, ibuf) == 0) {
                struct sockaddr_in* client = (struct sockaddr_in*) from;
                gethostname(hostname, sizeof(hostname));

                cout << "Client IP: " << inet_ntoa(client->sin_addr) << endl;
                cout << "Server hostname: " << hostname << endl;

                return true;
            }
        }
    }
    catch (int errCode) {
        return errCode == WSAETIMEDOUT ? false : throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
    }
}

bool PutAnswerToClient(const char* name, struct sockaddr* to, int* lto) {
    try {
        if ((sendto(sS, name, strlen(name) + 1, NULL, to, *lto)) == SOCKET_ERROR) {
            throw  SetErrorMsgText("send:", WSAGetLastError());
        }
    }
    catch (string errorMsgText) {
        cout << endl << "WSAGetLastError: " << errorMsgText;
        return false;
    }

    return true;
}

void GetServer(const char* call, short port, struct sockaddr* from, int* flen) {
    SOCKET cC;
    int countServers = 0;

    try {
        int optval = 1;

        SOCKADDR_IN all;                        // параметры  сокета sS
        all.sin_family = AF_INET;               // используется IP-адресация  
        all.sin_port = htons(port);             // порт 2000
        all.sin_addr.s_addr = inet_addr("192.168.0.255"); // всем 

        timeval timeout;
        timeout.tv_sec = 5000;
        timeout.tv_usec = 0;

        if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
            throw  SetErrorMsgText("socket:", WSAGetLastError());
        }
        if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
            throw  SetErrorMsgText("setsocketopt:", WSAGetLastError());
        }
        if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))) {
            throw  SetErrorMsgText("setsocketopt:", WSAGetLastError());
        }
        if ((sendto(cC, call, sizeof(call) + 2, NULL, (sockaddr*)&all, sizeof(all))) == SOCKET_ERROR) {
            throw SetErrorMsgText("sendto:", WSAGetLastError());
        }

        while (true)
        {
            if (recvfrom(cC, buf, sizeof(buf), NULL, from, flen) == SOCKET_ERROR) {
                throw  SetErrorMsgText("recvfrom:", WSAGetLastError());
            }
            if (strcmp(call, buf) == 0) {
                cout << "Server IP: " << inet_ntoa(((SOCKADDR_IN*)from)->sin_addr) << endl;
                countServers++;
            }
        }

    }
    catch (string errorMsgText)
    {
        if (WSAGetLastError() == WSAETIMEDOUT) {
            cout << "Find servers: " << countServers << endl << endl;
            if (closesocket(cC) == SOCKET_ERROR) {
                throw SetErrorMsgText("closesocket: ", WSAGetLastError());
            }
        }
        else {
            throw errorMsgText;
        }
    }
}