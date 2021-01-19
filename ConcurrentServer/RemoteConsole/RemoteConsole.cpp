#include <stdio.h>
#include <tchar.h>
#include "iostream"
#include "Windows.h"                
#include "ctime"
#include <string>

using std::string;

using namespace std;

string GetErrorMsgText( int code ) {
  string msgText;
  switch ( code ) {
    default: msgText = "***ERROR***";
      break;
  };
  return msgText;
}
string SetPipeError( string msgText, int code ) {
  return msgText + GetErrorMsgText( code );
};

int _tmain( int argc, _TCHAR* argv[] ) {
  setlocale( LC_ALL, "Rus" );
  SetConsoleTitle( "Remote Console" ); // заголовок консольного окна

  try {
    printf_s( "\nКоманды управления:\n 1 - Start  \t (запуск сервера)\n 2 - Stop  \t (остановить сервер)\n 3 - Exit  \t (завершить работу сервера)\n 4 - Statistics  (вывод статистики)\n 5 - Wait  \t (приостановить сервер)\n 6 - Shutdown  \t (приостанавлевает и завершает работу сервера)\n 7 - Завершить RConsole\n\n" );
    char ReadBuf[ 50 ] = "";// Буффер для примема сообшения от сервера
    char WriteBuf[ 2 ] = "";// Буффер для отправки сообщения серверу
    DWORD nBytesRead;
    DWORD nBytesWrite;

    int Code = 0;// Код команды

    char serverName[ 256 ];
    char PipeName[ 512 ];
    bool result;


    cout << "Введите имя севера: ";
    cin >> serverName;
    result = sprintf_s( PipeName, "\\\\%s\\pipe\\cpipe", serverName );


    BOOL fSuccess;

    SECURITY_ATTRIBUTES SecurityAttributes;
    SECURITY_DESCRIPTOR SecurityDescriptor;

    fSuccess = InitializeSecurityDescriptor(
      &SecurityDescriptor,
      SECURITY_DESCRIPTOR_REVISION );

    if ( !fSuccess ) {
      throw new string( "InitializeSecurityDescriptor(): Ошибка" );
    }

    fSuccess = SetSecurityDescriptorDacl(
      &SecurityDescriptor,
      TRUE,
      NULL,
      FALSE );

    if ( !fSuccess ) {
      throw new string( "SetSecurityDescriptorDacl(): Ошибка" );
    }

    SecurityAttributes.nLength = sizeof( SecurityAttributes );
    SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
    SecurityAttributes.bInheritHandle = FALSE;

    HANDLE hNamedPipe = CreateFile( PipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, &SecurityAttributes );

    do {
      printf_s( "Команда: " );
      scanf_s( "%d", &Code );
      if ( Code > 0 && Code < 7 ) {
        sprintf_s( WriteBuf, "%d", Code - 1 );
        if ( !WriteFile( hNamedPipe, WriteBuf, strlen( WriteBuf ) + 1, &nBytesWrite, NULL ) )
          throw "WriteFile: Ошибка ";
        if ( ReadFile( hNamedPipe, ReadBuf, sizeof( ReadBuf ), &nBytesRead, NULL ) )
          cout << ReadBuf << endl;
        else {
          throw "ReadFile: Ошибка ";
        }
      }
      else if ( Code > 7 ) printf( "Неверная команда.\n\n" );
    } while ( Code != 7 && Code != 3 && Code != 6 );
  }
  catch ( char* ErrorPipeText ) {
    printf_s( "%s", ErrorPipeText );
    cout << GetLastError( ) << endl;
  }
  
  cout << "Консоль управления завершает работу...";
  system("pause");
  return 0;
}

