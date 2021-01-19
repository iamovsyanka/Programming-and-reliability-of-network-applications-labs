#define _CRT_SECURE_NO_WARNINGS

#include "Global.h"
#include "AcceptServer.h"
#include "DispathServer.h"
#include "GarbageCleaner.h"
#include "ConsolePipe.h"
#include "ResponseServer.h"
#include "tchar.h"
#include "ServiceWorkTime.h"


int _tmain( int argc, _TCHAR* argv[] ) {
  setlocale( LC_ALL, "Russian" )
  SetConsoleTitle( "Concurrent Server" );

  try {
    cout << "Параллельный сервер запущен" << endl;

    if ( argc > 1 ) {
      int tmp = atoi( argv[ 1 ] );
      if ( tmp >= 0 && tmp <= 65535 ) {
        port = atoi( argv[ 1 ] );
        cout << "Используемый TCP-порт: " << port << endl;
      }
      else {
        cout << "Выбран не верный TCP-порт" << endl;
      }
    }
    else {
      cout << "TCP-порт по умолчанию: " << port << endl;
    }

    if ( argc > 2 ) {
      int tmp = atoi( argv[ 2 ] );
      if ( tmp >= 0 && tmp <= 65535 ) {
        uport = atoi( argv[ 2 ] );
        cout << "Используемый UDP-порт: " << uport << endl;
      }
      else {
        cout << "Выбран не верный UDP-порт" << endl;
      }
    }
    else {
      cout << "UDP-порт по умолчанию: " << uport << endl;
    }

    if ( argc > 3 ) {
      dllname = argv[ 3 ];
    }

    if ( argc > 4 ) {
      npname = argv[ 4 ];
      cout << "Имя именованного канала: " << npname << endl;
    }
    else {
      cout << "Имя именованного канала по умолчанию: " << npname << endl;
    }

    if ( argc > 5 ) {
      ucall = argv[ 5 ];
      cout << "Позывной:   " << ucall << endl;
    }
    else {
      cout << "Позывной по умолчанию: " << ucall << endl;
    }
    srand( (unsigned)time( NULL ) );
    
    volatile TalkersCmd  cmd = Start;  

    InitializeCriticalSection( &scListContact ); 
    st1 = LoadLibrary( dllname ); //загружаем dll
    ts1 = ( HANDLE( * )( char*, LPVOID ) )GetProcAddress( st1, "SSS" ); 

    if ( st1 == NULL ) cout << "Ошибка загрузки DLL" << endl;
    else cout << "Загруженная DLL: " << dllname << endl << endl;

    
    hAcceptServer = CreateThread( NULL, NULL, AcceptServer, (LPVOID)& cmd, NULL, NULL );
    HANDLE hDispathServer = CreateThread( NULL, NULL, DispathServer, (LPVOID)& cmd, NULL, NULL );
    HANDLE hGarbageCleaner = CreateThread( NULL, NULL, GarbageCleaner, (LPVOID)& cmd, NULL, NULL );
    HANDLE hConsolePipe = CreateThread( NULL, NULL, ConsolePipe, (LPVOID)& cmd, NULL, NULL );
    HANDLE hResponseServer = CreateThread( NULL, NULL, ResponseServer, (LPVOID)& cmd, NULL, NULL );

    SetThreadPriority( hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL );
    SetThreadPriority( hDispathServer, THREAD_PRIORITY_NORMAL );
    SetThreadPriority( hConsolePipe, THREAD_PRIORITY_NORMAL );
    SetThreadPriority( hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL );
    SetThreadPriority( hAcceptServer, THREAD_PRIORITY_HIGHEST );

    WaitForSingleObject( hAcceptServer, INFINITE );
    WaitForSingleObject( hDispathServer, INFINITE );
    WaitForSingleObject( hGarbageCleaner, INFINITE );
    WaitForSingleObject( hConsolePipe, INFINITE );
    WaitForSingleObject( hResponseServer, INFINITE );

    CloseHandle( hAcceptServer );
    CloseHandle( hDispathServer );
    CloseHandle( hGarbageCleaner );
    CloseHandle( hConsolePipe );
    CloseHandle( hResponseServer );

    DeleteCriticalSection( &scListContact ); 

    FreeLibrary( st1 ); 
  }
  catch ( ... ) {
    cout << "error" << endl;
  }

  system( "pause" );
  return 0;
}