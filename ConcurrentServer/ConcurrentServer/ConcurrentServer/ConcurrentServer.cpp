#include <iostream>
#include <clocale>
#include <ctime>

#include "ErrorMsgText.h"
#include <msclr\marshal_cppstd.h>
//#include "processthreadsapi.h"
#include "Winsock2.h"                
#pragma comment(lib, "WS2_32.lib")   

DWORD WINAPI AcceptServer(LPVOID  pPrm) 
{
    WSADATA wsaData;
    SOCKET  sS;
    SOCKADDR_IN serv;                    
    serv.sin_family = AF_INET;             
    serv.sin_port = htons(2000);         
    serv.sin_addr.s_addr = INADDR_ANY;

    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
            throw  SetErrorMsgText("Startup: ", WSAGetLastError());
        }
        if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET) {    
            throw  SetErrorMsgText("socket: ", WSAGetLastError());
        }
        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
            throw  SetErrorMsgText("bind: ", WSAGetLastError());
        }
        if (listen(sS, SOMAXCONN) == SOCKET_ERROR) {
            throw SetErrorMsgText("listen: ", WSAGetLastError());
        }

        u_long nonblk;
        if (ioctlsocket(sS, FIONBIO, &(nonblk = 1)) == SOCKET_ERROR)
            throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());
        CommandsCycle(*((TalkersCommand*)pPrm));

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

    ExitThread(*(DWORD*)pPrm);               
}

void CommandsCycle(TalkersCommand& cmd)      
{
    int  squirt = 0;
    while (cmd != EXIT)    
    {
        switch (cmd)
        {
        case START: cmd = GETCOMMAND; // возобновить подключение клиентов
            squirt = AS_SQUIRT; //#define AS_SQUIRT 10
            break;
        case STOP:  cmd = GETCOMMAND; // остановить подключение клиентов   
            squirt = 0;
            break;
            //........................................................
        };
        if (AcceptCycle(squirt))   //цикл  запрос/подключение (accept) 
        {
            cmd = GETCOMMAND;
            //.... запуск потока EchoServer.......................
            //.... установка ожидающего таймера для процесса EchoServer ...      
        }
        else SleepEx(0, TRUE);    // выполнить асинхронные процедуры 
    };
}

int main()
{

}
