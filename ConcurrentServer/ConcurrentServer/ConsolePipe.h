#pragma once
#include "Global.h"

DWORD WINAPI ConsolePipe( LPVOID pPrm ) {
  cout << "ConsolePipe запущен\n";
  DWORD rc = 0;
  HANDLE hPipe;
  try {
    BOOL fSuccess;
    SECURITY_ATTRIBUTES SecurityAttributes;
    SECURITY_DESCRIPTOR SecurityDescriptor;

    fSuccess = InitializeSecurityDescriptor(
      &SecurityDescriptor,
      SECURITY_DESCRIPTOR_REVISION );

    if ( !fSuccess ) {
      throw new string( "InitializeSecurityDescriptor():" );
    }

    fSuccess = SetSecurityDescriptorDacl(
      &SecurityDescriptor,
      TRUE,
      NULL,
      FALSE );

    if ( !fSuccess ) {
      throw new string( "SetSecurityDescriptorDacl():" );
    }

    SecurityAttributes.nLength = sizeof( SecurityAttributes );
    SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
    SecurityAttributes.bInheritHandle = FALSE;

    char rnpname[ 50 ];
    strcpy_s( rnpname, "\\\\.\\pipe\\" );
    strcat_s( rnpname, npname );
    if ( ( hPipe = CreateNamedPipe(rnpname, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL,
                                    INFINITE, NULL ) ) == INVALID_HANDLE_VALUE )
      throw SetErrorMsgText( "Create:", GetLastError( ) );

    while ( *( (TalkersCmd*)pPrm ) != Exit ) {
      if ( !ConnectNamedPipe( hPipe, NULL ) ) 
        throw SetErrorMsgText( "Connect:", GetLastError( ) );
      char ReadBuf[ 50 ], WriteBuf[ 50 ];
      DWORD nBytesRead = 0, nBytesWrite = 0;
      TalkersCmd SetCommand;
      bool Check;
     
      while ( *( (TalkersCmd*)pPrm ) != Exit ) {
        if ( *( (TalkersCmd*)pPrm ) == Getcommand ) {

          if ( !ReadFile( hPipe, ReadBuf, sizeof( ReadBuf ), &nBytesRead, NULL ) )
            break;
          if ( nBytesRead > 0 ) {
            Check = true;
            int n = atoi( ReadBuf );
            switch ( n ) {
              case 0:
                sprintf_s( WriteBuf, "%s", "Start" );
                SetCommand = TalkersCmd::Start;
                break;
              case 1:
                sprintf_s( WriteBuf, "%s", "Stop" );
                SetCommand = TalkersCmd::Stop;
                break;
              case 2:
                sprintf_s( WriteBuf, "%s", "Exit" );
                SetCommand = TalkersCmd::Exit;
                break;
              case 3:
                sprintf_s( WriteBuf, "\nПодключено.: \t%i\nНеудачно.: \t%i\nЗавершено.: \t%i\n", Accept, Fail, Finished );
                Check = false;
                break;
              case 4:
                sprintf_s( WriteBuf, "%s", "Wait" );
                SetCommand = TalkersCmd::Wait;
                break;
              case 5:
                sprintf_s( WriteBuf, "%s", "Shutdown" );
                SetCommand = TalkersCmd::Shutdown;
                break;
              default:
                sprintf_s( WriteBuf, "%s", "NoCmd" );
                Check = false;
                break;
            }
            if ( Check == true ) {
              *( (TalkersCmd*)pPrm ) = SetCommand;
              printf_s( "ConsolePipe: команда %s\n", WriteBuf );
            }

            if ( !WriteFile( hPipe, WriteBuf, sizeof( WriteBuf ), &nBytesRead, NULL ) )
              throw new string( "CP WRITE ERROR" );
          }
        }
        else Sleep( 1000 );
      }
      if ( !DisconnectNamedPipe( hPipe ) )  
        throw SetErrorMsgText( "disconnect:", GetLastError( ) );
    }
    DisconnectNamedPipe( hPipe );
    CloseHandle( hPipe );
    cout << "ConsolePipe остановлен" << endl;
  }
  catch ( string ErrorPipeText ) {
    cout << ErrorPipeText << endl;
  }
  catch ( ... ) {
    cout << "Error ConsolePipe" << endl;
  }
  ExitThread( rc );
}