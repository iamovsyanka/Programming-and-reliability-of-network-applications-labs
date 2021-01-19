#pragma once
#include "Global.h"
#include "ServiceWorkTime.h"

const string currentDateTime( );
int* getHoursAndMinutes( string str );
bool isBetween( string now, string from, string to );

DWORD WINAPI DispathServer( LPVOID pPrm ) {
  cout << "DispathServer запущен\n";
  DWORD rc = 0;
  try {
    while ( *( (TalkersCmd*)pPrm ) != Exit ) {
      try {
        if ( WaitForSingleObject( Event, 300 ) == WAIT_OBJECT_0 ) {

          if ( &Work > 0 ) {
            Contact* client = NULL;
            int libuf = 1;
            char CallBuf[ 50 ] = "", SendError[ 50 ] = "ErrorInquiry";
            EnterCriticalSection( &scListContact );
            for ( ListContact::iterator p = Contacts.begin( ); p != Contacts.end( ); p++ ) {
              if ( p->type == Contact::ACCEPT ) 
              {
                client = &( *p );

                bool Check = false;
                while ( Check == false ) {
                  if ( ( libuf = recv( client->s, CallBuf, sizeof( CallBuf ), NULL ) ) == SOCKET_ERROR ) {
                    switch ( WSAGetLastError( ) ) {
                      case WSAEWOULDBLOCK: Sleep( 100 ); break;
                      default: throw  SetErrorMsgText( "Recv:", WSAGetLastError( ) );
                    }
                  }
                  else Check = true;
                }
                if ( strcmp( CallBuf, "Rand" ) == 0 || strcmp( CallBuf, "Echo" ) == 0 || strcmp( CallBuf, "Time" ) == 0 )
                  Check == true;
                else Check == false;

                if ( Check == true ) {
                  string time_error;

                  if ( rand_from.empty( ) && rand_to.empty( ) &&
                       echo_from.empty( ) && echo_to.empty( ) &&
                       time_from.empty( ) && time_to.empty( ) ) {
                    client->type = Contact::CONTACT; 
                    strcpy_s( client->srvname, CallBuf );
                    client->htimer = CreateWaitableTimer( NULL, false, NULL ); 
                    _int64 time = -600000000;
                    SetWaitableTimer( client->htimer, (LARGE_INTEGER*)& time, 0, ASWTimer, client, false );

                    cout << CallBuf << endl;

                    if ( ( libuf = send( client->s, CallBuf, sizeof( CallBuf ), NULL ) ) == SOCKET_ERROR )
                      throw SetErrorMsgText( "Send:", WSAGetLastError( ) );
                    client->hthread = ts1( CallBuf, client );
                  }
                  else {
                    cout << rand_from << endl;
                    cout << rand_to << endl;
                    cout << echo_from << endl;
                    cout << echo_to << endl;
                    cout << time_from << endl;
                    cout << time_to << endl;
                  
                    if ( strcmp( CallBuf, "Rand" ) == 0 ) {
                      string time_now = currentDateTime( );

                      cout << time_now << endl;

                      if ( isBetween( time_now, rand_from, rand_to ) ) {
                        client->type = Contact::CONTACT; 
                        strcpy_s( client->srvname, CallBuf );
                        client->htimer = CreateWaitableTimer( NULL, false, NULL ); //создание ожидающего таймера
                        _int64 time = -600000000; //установка времени (15 секунд)
                        SetWaitableTimer( client->htimer, (LARGE_INTEGER*)& time, 0, ASWTimer, client, false );

                        cout << CallBuf << endl;

                        if ( ( libuf = send( client->s, CallBuf, sizeof( CallBuf ), NULL ) ) == SOCKET_ERROR )
                          throw SetErrorMsgText( "Send:", WSAGetLastError( ) );
                        client->hthread = ts1( CallBuf, client );
                      }
                      else {

                        time_error = "RandService unavailable";

                        EnterCriticalSection( &scListContact );
                        client->CloseConn = true;
                        client->sthread = Contact::FINISH;
                        LeaveCriticalSection( &scListContact );

                        if ( ( libuf = send( client->s, time_error.c_str( ), time_error.length( ), NULL ) ) == SOCKET_ERROR )
                          throw SetErrorMsgText( "Send:", WSAGetLastError( ) );

                        SYSTEMTIME stt;
                        GetLocalTime( &stt );
                        printf( "%d.%d.%d %d:%02d RandService unavailable ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
                        cout << ";" << endl;

                      }
                    }
                    else if ( strcmp( CallBuf, "Echo" ) == 0 ) {
                      string time_now = currentDateTime( );
                      cout << time_now << endl;

                      if ( isBetween( time_now, echo_from, echo_to ) ) {
                        client->type = Contact::CONTACT;
                        strcpy_s( client->srvname, CallBuf );
                        client->htimer = CreateWaitableTimer( NULL, false, NULL );
                        _int64 time = -600000000; 
                        SetWaitableTimer( client->htimer, (LARGE_INTEGER*)& time, 0, ASWTimer, client, false );

                        cout << CallBuf << endl;

                        if ( ( libuf = send( client->s, CallBuf, sizeof( CallBuf ), NULL ) ) == SOCKET_ERROR )
                          throw SetErrorMsgText( "Send:", WSAGetLastError( ) );
                        client->hthread = ts1( CallBuf, client );
                      }
                      else {
                        time_error = "EchoService unavailable";

                        EnterCriticalSection( &scListContact );
                        client->CloseConn = true;
                        client->sthread = Contact::FINISH;
                        LeaveCriticalSection( &scListContact ); 	

                        if ( ( libuf = send( client->s, time_error.c_str( ), time_error.length( ), NULL ) ) == SOCKET_ERROR )
                          throw SetErrorMsgText( "Send:", WSAGetLastError( ) );

                        SYSTEMTIME stt;
                        GetLocalTime( &stt );
                        printf( "%d.%d.%d %d:%02d EchoService unavailable ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
                        cout << ";" << endl;

                      }
                    }
                    else if ( strcmp( CallBuf, "Time" ) == 0 ) {
                      string time_now = currentDateTime( );
                      cout << time_now << endl;

                      if ( isBetween( time_now, time_from, time_to ) ) {
                        client->type = Contact::CONTACT;
                        strcpy_s( client->srvname, CallBuf );
                        client->htimer = CreateWaitableTimer( NULL, false, NULL );
                        _int64 time = -600000000;
                        SetWaitableTimer( client->htimer, (LARGE_INTEGER*)& time, 0, ASWTimer, client, false );

                        cout << CallBuf << endl;

                        if ( ( libuf = send( client->s, CallBuf, sizeof( CallBuf ), NULL ) ) == SOCKET_ERROR )
                          throw SetErrorMsgText( "Send:", WSAGetLastError( ) );
                        client->hthread = ts1( CallBuf, client );
                      }
                      else {
                        time_error = "TimeService unavailable";

                        EnterCriticalSection( &scListContact );
                        client->CloseConn = true;
                        client->sthread = Contact::FINISH;
                        LeaveCriticalSection( &scListContact ); 	

                        if ( ( libuf = send( client->s, time_error.c_str( ), time_error.length( ), NULL ) ) == SOCKET_ERROR )
                          throw SetErrorMsgText( "Send:", WSAGetLastError( ) );

                        SYSTEMTIME stt;
                        GetLocalTime( &stt );
                        printf( "%d.%d.%d %d:%02d TimeService unavailable ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute );
                        cout << ";" << endl;

                      }
                    }
                  }
                }
                else {
                  if ( ( libuf = send( client->s, SendError, sizeof( SendError ) + 1, NULL ) ) == SOCKET_ERROR )
                    throw SetErrorMsgText( "Send:", WSAGetLastError( ) );
                  closesocket( client->s );
                  client->sthread = Contact::ABORT;
                  CancelWaitableTimer( client->htimer );
                  InterlockedIncrement( &Fail );
                }
              }
            }
            LeaveCriticalSection( &scListContact );
          }
          SleepEx( 0, true );
        }
        SleepEx( 0, true );
      }
      catch ( string errorMsgText ) {
        std::cout << errorMsgText << endl;
      }
    }
  }
  catch ( string errorMsgText ) {
    std::cout << errorMsgText << endl;
  }
  catch ( ... ) {
    std::cout << "Error DispathServer" << std::endl;
  }
  cout << "DispathServer остановлен\n" << endl;
  ExitThread( rc );
}

int* getHoursAndMinutes( string str ) {
  string str_1, str_2;
  if ( str[ 2 ] == ':' ) {
    str_1 += str.substr( 0, 2 );
    str_2 += str.substr( 3, 2 );
    int ihh = atoi( str_1.c_str( ) );
    int imm = atoi( str_2.c_str( ) );
    return new int[ 2 ] { ihh, imm };
  }
  return NULL;
}

const string currentDateTime( ) {
  time_t     now = time( 0 );
  struct tm  tstruct;
  char       buf[ 80 ];
  tstruct = *localtime( &now );
  strftime( buf, sizeof( buf ), "%H:%M", &tstruct );

  return buf;
}

bool isBetween( string now, string from, string to ) {
  bool rs = false;
  int* arr_now = getHoursAndMinutes( now );
  int* arr_from = getHoursAndMinutes( from );
  int* arr_to = getHoursAndMinutes( to );

  if ( arr_now[ 0 ] == arr_from[ 0 ] ) {
    if ( arr_now[ 1 ] > arr_from[ 1 ] ) {
      if ( arr_now[ 1 ] < arr_to[ 1 ] ) {
        rs = true;
      }
    }
  }
  else if ( arr_now[ 0 ] == arr_to[ 0 ] ) {
    if ( arr_now[ 1 ] < arr_to[ 1 ] ) {
      rs = true;
    }
  }
  else if ( arr_now[ 0 ] > arr_from[ 0 ] && arr_now[ 0 ] < arr_to[ 0 ] ) {
    rs = true;
  }

  return rs;
}