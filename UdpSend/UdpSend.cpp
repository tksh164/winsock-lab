#define _UNICODE
#define UNICODE

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
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

    PCWSTR remoteAddr = argv[1];
    const USHORT remotePort = StrToInt(argv[2]);

    wprintf(L"Remote address: %s\n", remoteAddr);
    wprintf(L"Remote port   : %d\n", remotePort);

    //

    WORD versionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    INT err = WSAStartup(versionRequested, &wsaData);
    if (err != 0)
    {
        wprintf(L"Failed: WSAStartup(): %d\n", err);
        return 0;
    }

    //

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        wprintf(L"Failed: socket(): %d\n", err);
        goto Cleanup;
    }

    //

    SOCKADDR_IN remoteAddrIn;
    SecureZeroMemory(&remoteAddrIn, sizeof(remoteAddrIn));
    remoteAddrIn.sin_family = AF_INET;
    err = InetPton(remoteAddrIn.sin_family, remoteAddr, &remoteAddrIn.sin_addr);
    if (err != 1)
    {
        if (err == 0)
        {
            wprintf(L"Failed: InetPton(): Not a valid IPv4 dotted-decimal string or a valid IPv6 address string\n");
        }
        else
        {
            wprintf(L"Failed: InetPton(): 0x%08x\n", WSAGetLastError());
        }
        goto Cleanup;
    }
    remoteAddrIn.sin_port = htons(remotePort);

    const int BUFFER_SIZE = 64;
    CHAR buffer[BUFFER_SIZE];
    INT bufferLengthInBytes = sizeof(buffer);
    if (sendto(sock, buffer, bufferLengthInBytes, 0, (SOCKADDR *)&remoteAddrIn, sizeof(remoteAddrIn)) == SOCKET_ERROR)
    {
        wprintf(L"Failed: sendto(): 0x%08x\n", WSAGetLastError());
        goto Cleanup;
    }

    wprintf(L"Sent to %s:%d\n", remoteAddr, remotePort);

Cleanup:

    if (sock != INVALID_SOCKET)
    {
        closesocket(sock);
    }

    WSACleanup();

    return 0;
}
