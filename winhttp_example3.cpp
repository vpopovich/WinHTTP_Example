#include <windows.h>
#include <stdio.h>
#include <winhttp.h>

int main(int argc, char** argv)
{

  DWORD dwSize = 0;
  DWORD dwDownloaded = 0;
  LPSTR pszOutBuffer;
  BOOL  bResults = FALSE;
  HINTERNET  hSession = NULL, 
             hConnect = NULL,
             hRequest = NULL;
  char lURL01[802];
  unsigned int uiPort01;
			 
  printf("URL:%s\n",argv[1]);
  printf("Port:%s\n",argv[2]);
  uiPort01 = atoi(argv[2]);
  MultiByteToWideChar(
  CP_ACP                              // CodePage
  ,MB_PRECOMPOSED                     //        dwFlags
  ,argv[1]              // lpMultiByteStr
  ,-1    // cbMultiByte
  , reinterpret_cast<LPWSTR>(lURL01)                     //       lpWideCharStr
  ,  400   // cchWideChar
);


  // Use WinHttpOpen to obtain a session handle.
  printf( "Trying WinHttpOpen ...\n");
  hSession = WinHttpOpen( L"WinHTTP Example/1.0",  
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME, 
                          WINHTTP_NO_PROXY_BYPASS, 0 );

  // Specify an HTTP server.
  if( hSession )
  {
    /* hConnect = WinHttpConnect( hSession, L"www.microsoft.com",
                               INTERNET_DEFAULT_HTTPS_PORT, 0 );
	*/
	printf( "Trying WinHttpConnect ...\n");
	// hConnect = WinHttpConnect( hSession, L"www.serv01.ru", 9989, 0 );
	hConnect = WinHttpConnect( hSession, reinterpret_cast<LPWSTR>(lURL01), uiPort01, 0 );
  }

  // Create an HTTP request handle.
  if( hConnect )
  {
	printf( "Trying WinHttpOpenRequest ...\n");
    hRequest = WinHttpOpenRequest( hConnect, L"GET", NULL,
                                   NULL, WINHTTP_NO_REFERER, 
                                   WINHTTP_DEFAULT_ACCEPT_TYPES, 
                                   WINHTTP_FLAG_SECURE );
  }

  // Send a request.
  if( hRequest )
  {
	// DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
	DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
	printf( "Trying WinHttpSetOption ...\n");
	bResults = WinHttpSetOption(hRequest,WINHTTP_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags,sizeof(dwFlags)  );
		
	if(bResults)
	{
		printf( "Trying WinHttpSendRequest ...\n");
		bResults = WinHttpSendRequest( hRequest,
                                   WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                   WINHTTP_NO_REQUEST_DATA, 0, 
                                   0, 0 );
	}
  }


  // End the request.
  if( bResults )
  {
	printf( "Trying WinHttpReceiveResponse ...\n");
    bResults = WinHttpReceiveResponse( hRequest, NULL );
  }

  // Keep checking for data until there is nothing left.
  if( bResults )
  {
    do 
    {
      // Check for available data.
      dwSize = 0;
      if( !WinHttpQueryDataAvailable( hRequest, &dwSize ) )
        printf( "Error %u in WinHttpQueryDataAvailable.\n",
                GetLastError( ) );

      // Allocate space for the buffer.
      pszOutBuffer = new char[dwSize+1];
      if( !pszOutBuffer )
      {
        printf( "Out of memory\n" );
        dwSize=0;
      }
      else
      {
        // Read the data.
        ZeroMemory( pszOutBuffer, dwSize+1 );

        if( !WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, 
                              dwSize, &dwDownloaded ) )
          printf( "Error %u in WinHttpReadData.\n", GetLastError( ) );
        else
          printf( "%s", pszOutBuffer );

        // Free the memory allocated to the buffer.
        delete [] pszOutBuffer;
      }
    } while( dwSize > 0 );
  }


  // Report any errors.
  if( !bResults )
  {
    printf( "Error %d has occurred.\n", GetLastError( ) );
  }

  // Close any open handles.
  if( hRequest ) WinHttpCloseHandle( hRequest );
  if( hConnect ) WinHttpCloseHandle( hConnect );
  if( hSession ) WinHttpCloseHandle( hSession );
  
  return 0;
}
