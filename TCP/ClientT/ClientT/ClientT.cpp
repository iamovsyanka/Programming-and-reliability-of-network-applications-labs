#define _WINSOCK_DEPRECATED_NO_WARNINGS

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

    SOCKET cC;                                      //серверный сокет
    SOCKADDR_IN serv;                               //параметры сокета сервера
    serv.sin_family = AF_INET;                      //используется IP-адресация
    serv.sin_port = htons(2000);                    //TCP-порт 2000
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");  //адрес сервера

    try {
        cout << "ClientT\n\n";
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
            //версия. Младший байт основная версия, старший байт расширение версии.
            //параметры инициализации
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        if ((cC = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET) {
            //функция  позволяет создать сокет (точнее дескриптор сокета) и задать его характеристики 
            //SOCK_STREEM – сокет ориентированный на поток;    
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }
        if ((connect(cC, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) {
            //функция используется клиентом для создания канала с определенным сокетом сервера 
            throw SetErrorMsgText("connect: ", WSAGetLastError());
        }

        clock_t start, end;
        char ibuf[50] = "server: принято ";     //буфер вывода
        int  libuf = 0,                         //количество принятых байт
             lobuf = 0;                         //количество отправленных байт 

        int countMessage;
        cout << "Number of messages: ";
        cin >> countMessage;

        start = clock();
        for (int i = 1; i <= countMessage; i++) {
            string obuf = "Hello from Client " + to_string(i);

            if ((lobuf = send(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR) {
                //функция пересылает заданное  количество байт данных по каналу определенного сокета  
                throw SetErrorMsgText("send: ", WSAGetLastError());
            }
                        
            if ((libuf = recv(cC, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR) {
                //функция принимает заданное  количество байт данных по каналу определенного сокета  
                throw SetErrorMsgText("recv: ", WSAGetLastError());
            }

            cout << ibuf << endl;
        }
        end = clock();
        string obuf = "";

        if ((lobuf = send(cC, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR) {
            //функция пересылает заданное  количество байт данных по каналу определенного сокета  
            throw SetErrorMsgText("send: ", WSAGetLastError());
        }

        cout << "Time for send and recv: " << ((double)(end - start) / CLK_TCK) << " c" << endl;

        if (closesocket(cC) == SOCKET_ERROR) {
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());
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

