#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Global.h"

//const string currentDateTime( );
//int* getHoursAndMinutes( string str );

extern string rand_from = "";
extern string rand_to = "";
extern string echo_from = "";
extern string echo_to = "";
extern string time_from = "";
extern string time_to = "";

DWORD WINAPI ServiceWorkTime( LPVOID pPrm ) {
  cout << "ServiceWorkTime started\n" << endl;
  DWORD rc = 0;
  try {

    HANDLE mail;
    char swt_name[ 50 ];
    char readbuf[ 50 ];
    DWORD nBytesRead;
    string mailname = "\\\\.\\mailslot\\box";


    if ( ( mail = CreateMailslot( mailname.c_str( ), 300, 200000, NULL ) ) == INVALID_HANDLE_VALUE ) {
      throw SetErrorMsgText( "Create:", GetLastError( ) );
    }



    while ( *( (TalkersCmd*)pPrm ) != Exit ) {
      //cout << currentDateTime( ) << endl;
      if ( ReadFile( mail, readbuf, sizeof( readbuf ), &nBytesRead, NULL ) ) {
        //cout << readbuf << endl;
        string str = readbuf;
        //cout << str.length() << endl;
        rand_from = str.substr( 0, 5 );
        rand_to = str.substr( 6, 5 );
        echo_from = str.substr( 12, 5 );
        echo_to = str.substr( 18, 5 );
        time_from = str.substr( 24, 5 );
        time_to = str.substr( 30 , 5 );

      }
      else {
        throw "ReadFile: Ошибка ";
      }

      cout << endl;

      //cout << currentDateTime( ) << endl;
    }



    CloseHandle( mail );
  }
  catch ( string errorMsgText ) {
    std::cout << errorMsgText << endl;
  }
  catch ( ... ) {
    cout << "Error ServiceWork" << endl;
  }
  cout << "ServiceWorkTime остановлен\n" << endl;
  ExitThread( rc );
}

//int* getHoursAndMinutes( string str ) {
//  string str_1, str_2;
//  if ( str[ 2 ] == ':' ) {
//    str_1 = str[ 0 ] + str[ 1 ];
//    str_2 = str[ 3 ] + str[ 4 ];
//    int ihh = atoi( str_1.c_str( ) );
//    int imm = atoi( str_2.c_str( ) );
//    return new int[ 2 ] { ihh, imm };
//  }
//  return NULL;
//}
//
//const string currentDateTime( ) {
//  time_t     now = time( 0 );
//  struct tm  tstruct;
//  char       buf[ 80 ];
//  tstruct = *localtime( &now );
//  strftime( buf, sizeof( buf ), "%H:%M", &tstruct );
//
//  return buf;
//}