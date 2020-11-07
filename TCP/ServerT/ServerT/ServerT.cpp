#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include "Winsock2.h"                // заголовок  WS2_32.dll
#pragma comment(lib, "WS2_32.lib")   // экспорт  WS2_32.dll

int main()
{
    setlocale(LC_ALL, "rus");

    WSADATA wsaData;      //запись служебной информации

    SOCKET  sS;           // дескриптор сокета 
    SOCKADDR_IN serv;                    // параметры  сокета sS
    serv.sin_family = AF_INET;           // используется IP-адресация  
    serv.sin_port = htons(2000);         // порт 2000
    serv.sin_addr.s_addr = INADDR_ANY;   // любой собственный IP-адрес 

    try {
        cout << "ServerT\n\n";
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
            //версия. Младший байт основная версия, старший байт расширение версии.
            //параметры инициализации
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET) {
            //функция  позволяет создать сокет (точнее дескриптор сокета) и задать его характеристики 
            //SOCK_STREEM – сокет ориентированный на поток;    
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }
        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
            // связать сокет с параметрами
            throw  SetErrorMsgText("bind: ", WSAGetLastError());
        }
        if (listen(sS, SOMAXCONN) == SOCKET_ERROR) {
            // переключить сокет в режим прослушивания
            throw SetErrorMsgText("listen: ", WSAGetLastError());
        }

        SOCKET cS;	                            // сокет для обмена данными с клиентом 
        SOCKADDR_IN clnt;                       // параметры  сокета клиента
        memset(&clnt, 0, sizeof(clnt));         // обнулить память
        int lclnt = sizeof(clnt);               // размер SOCKADDR_IN

        clock_t start, end;
        char ibuf[50],                          //буфер ввода 
             obuf[50] = "server: принято ";     //буфер вывода
        int  libuf = 0,                         //количество принятых байт
             lobuf = 0;                         //количество отправленных байт 

        if ((cS = accept(sS, (sockaddr*)&clnt, &lclnt)) == INVALID_SOCKET) {
            // разрешить подключение к сокету
            throw SetErrorMsgText("accept: ", WSAGetLastError());
        }

        start = clock();
        while (true) {
            if ((libuf = recv(cS, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR) {
                //функция принимает заданное  количество байт данных по каналу определенного сокета  
                throw SetErrorMsgText("recv: ", WSAGetLastError());
            }

            string obuf = ibuf;
            if ((lobuf = send(cS, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR) {
                //функция пересылает заданное  количество байт данных по каналу определенного сокета  
                throw SetErrorMsgText("send: ", WSAGetLastError());
            }

            if (strcmp(ibuf, "") == 0) break;
            cout << ibuf << endl;
        }
        end = clock();
        cout << "Time for send and recv: " << ((double)(end - start) / CLK_TCK) << " c" << endl;

        if (closesocket(cS) == SOCKET_ERROR) {
            //переводит сокет в неработоспособное состояние и освобождает все ресурсы связанные с ним  
            throw  SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
        if (closesocket(sS) == SOCKET_ERROR) {
            //переводит сокет в неработоспособное состояние и освобождает все ресурсы связанные с ним  
            throw  SetErrorMsgText("closesocket: ", WSAGetLastError());
        }
        if (WSACleanup() == SOCKET_ERROR) {
            // завершить  работу с библиотекой  WS2_32.DLL
            throw  SetErrorMsgText("Cleanup: ", WSAGetLastError());
        }
    }
    catch (string errorMsgText) {
        cout << endl << errorMsgText;
    }

    return 0;
}