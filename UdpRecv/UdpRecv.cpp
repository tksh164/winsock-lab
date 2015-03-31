#define _UNICODE
#define UNICODE

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <windows.h>
#include <wchar.h>
#include <shlwapi.h>

INT
wmain(INT argc, WCHAR *argv[])
{
    if (argc < 2)
    {
        wprintf(L"%s <Bind Port>\n", argv[0]);
        return 0;
    }

    USHORT localPort = StrToInt(argv[1]);

    //

    INT err;
    WORD versionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    err = WSAStartup(versionRequested, &wsaData);
    if (err != 0)
    {
        wprintf(L"Failed: WSAStartup(): %d\n", err);
        return 0;
    }

    //

    SOCKET sock;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        wprintf(L"Failed: socket(): %d\n", err);
        goto Cleanup;
    }

    //

    SOCKADDR_IN localAddrIn;
    localAddrIn.sin_family = AF_INET;
    localAddrIn.sin_port = htons(localPort);    
    localAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (SOCKADDR *)&localAddrIn, sizeof(localAddrIn));
    if (err != 0)
    {
        wprintf(L"Failed: bind(): %d\n", err);
        goto Cleanup;
    }

    //

    CHAR buffer[32];
    INT bufferLength = sizeof(buffer);
    SOCKADDR_IN remoteAddrIn;
    INT remoteAddrInLength = sizeof(remoteAddrIn);
    err = recvfrom(sock, buffer, bufferLength, 0, (SOCKADDR *)&remoteAddrIn, &remoteAddrInLength);
    if (err == SOCKET_ERROR)
    {
        wprintf(L"Failed: recvfrom(): %d\n", err);
        goto Cleanup;
    }

    wprintf(L"Received from %S@%d\n", inet_ntoa(remoteAddrIn.sin_addr), remoteAddrIn.sin_port);

Cleanup:

    if (sock != INVALID_SOCKET)
    {
        closesocket(sock);
    }

    WSACleanup();

    return 0;
}
