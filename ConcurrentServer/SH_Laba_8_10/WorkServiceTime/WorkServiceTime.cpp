#include <stdio.h>
#include <tchar.h>
#include "iostream"
#include "Windows.h"                
#include "ctime"
#include <string>

using namespace std;

bool validTime( string str );
bool correctTimeFromTo( string from, string to );
int* getHoursAndMinutes( string str );


int main( ) {
  setlocale( LC_ALL, "Rus" );
  SetConsoleTitle( "WorkServiceTime" ); // заголовок консольного окна

  try {
    HANDLE mail;
    char bufwrite[ 50 ] = "", bufread[ 50 ] = {};
    DWORD dw;
    int count = 0, i = 0;
    char serverName[ 256 ];
    char MailName[ 512 ];
    char WriteBuf[ 50 ] = "test";// Буффер для отправки сообщения серверу
    bool result;
    DWORD nBytesWrite;
    string servname;
    string str;
    string rand_time_from = "10:15";
    string rand_time_to = "15:30";
    string echo_time_from = "13:00";
    string echo_time_to = "13:05";
    string time_time_from = "15:00";
    string time_time_to = "17:30";


    cout << "Enter servaname: ";
    cin >> str;

    servname.append( "\\\\" );
    servname.append( str );
    servname.append( "\\mailslot\\box" );

    mail = CreateFile( servname.c_str( ), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL );

   /* cout << "Enter RandSerice time (hh:mm)\nfrom ";
    cin >> rand_time_from;
    cout << "to ";
    cin >> rand_time_to;

    cout << "Enter EchService time (hh:mm)\nfrom ";
    cin >> echo_time_from;
    cout << "to ";
    cin >> echo_time_to;


    cout << "Enter TimeSerice time (hh:mm)\nfrom ";
    cin >> time_time_from;
    cout << "to ";
    cin >> time_time_to;*/

    if ( correctTimeFromTo( rand_time_from, rand_time_to ) &&
         correctTimeFromTo( echo_time_from, echo_time_to ) &&
         correctTimeFromTo( time_time_from, time_time_to ) ) {
      string msg;
      msg += rand_time_from + '\n' + rand_time_to + '\n';
      msg += echo_time_from + '\n' + echo_time_to + '\n';
      msg += time_time_from + '\n' + time_time_to + '\n';

      if ( !WriteFile( mail, msg.c_str( ), strlen( msg.c_str( ) ) + 1, &nBytesWrite, NULL ) )
        throw "WriteFile: Ошибка ";

    }
    else {
      cout << "invalid time input" << endl;
    }

    CloseHandle( mail );
  }
  catch ( ... ) {
    cout << "dkjfdk" << endl;
  }

  system( "pause" );
  return 0;
}

bool validTime( string str ) {
  bool res = false;

  if ( str.length( ) == 5 ) {
  int* arr = getHoursAndMinutes( str );
    if ( arr != NULL ) {
      if ( arr[0] >= 0 && arr[ 0 ] <= 23 && arr[ 1 ] >= 0 && arr[ 1 ] <= 59 ) {
        return true;
      }
    }
  }
  return res;
}

bool correctTimeFromTo( string from, string to ) {
  if ( validTime( from ) && validTime( to ) ) {
    int* time_from, * time_to;
    if ( ( time_from = getHoursAndMinutes( from ) ) != NULL &&
      ( time_to = getHoursAndMinutes( to ) ) != NULL ) {
      if ( time_from[ 0 ] == time_to[ 0 ] ) {
        if ( time_from[ 1 ] < time_to[ 1 ] ) {
          return true;
        }
      }
      else if ( time_from[ 0 ] < time_to[ 0 ] ) {
        return true;
      }
    }
  }
  return false;
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

