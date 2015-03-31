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
    if (argc < 3)
    {
        wprintf(L"%s <Remote Address> <Remote Port>\n", argv[0]);
        return 0;
    }

    CHAR remoteAddr[20];
    size_t remoteAddrLengthInBytes = sizeof(remoteAddr);
    size_t numCharsConverted;
    errno_t errno = wcstombs_s(&numCharsConverted, remoteAddr, remoteAddrLengthInBytes, argv[1], _TRUNCATE);
    if (errno != 0)
    {
        wprintf(L"Failed: wcstombs_s(): %d\n", errno);
        return 0;
    }

    USHORT remotePort = StrToInt(argv[2]);

    wprintf(L"Address: %S\n", remoteAddr);
    wprintf(L"Port   : %d\n", remotePort);

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

    SOCKADDR_IN remoteAddrIn;
    remoteAddrIn.sin_family = AF_INET;
    remoteAddrIn.sin_port = htons(remotePort);    
    remoteAddrIn.sin_addr.s_addr = inet_addr(remoteAddr);

    CHAR buffer[16];
    INT bufferLengthInBytes = sizeof(buffer);
    err = sendto(sock, buffer, bufferLengthInBytes, 0, (SOCKADDR *)&remoteAddrIn, sizeof(remoteAddrIn));
    if (err == SOCKET_ERROR)
    {
        wprintf(L"Failed: sendto(): %d\n", err);
        goto Cleanup;
    }

    wprintf(L"Sent %S@%d\n", remoteAddr, remotePort);

Cleanup:

    if (sock != INVALID_SOCKET)
    {
        closesocket(sock);
    }

    WSACleanup();

    return 0;
}
