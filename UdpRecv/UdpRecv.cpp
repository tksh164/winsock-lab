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
        wprintf(L"%s <Local Address> <Local Port>\n", argv[0]);
        return 0;
    }

    PCWSTR localAddr = argv[1];
    const USHORT localPort = StrToInt(argv[2]);

    wprintf(L"Local address: %s\n", localAddr);
    wprintf(L"Local port   : %d\n", localPort);

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

    SOCKADDR_IN localAddrIn;
    SecureZeroMemory(&localAddrIn, sizeof(localAddrIn));
    localAddrIn.sin_family = AF_INET;
    err = InetPton(localAddrIn.sin_family, localAddr, &localAddrIn.sin_addr);
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
    localAddrIn.sin_port = htons(localPort);    

    if (bind(sock, (SOCKADDR *)&localAddrIn, sizeof(localAddrIn)) == SOCKET_ERROR)
    {
        wprintf(L"Failed: bind(): 0x%08x\n", WSAGetLastError());
        goto Cleanup;
    }

    //

    const int BUFFER_SIZE = 64;
    CHAR receiveBuffer[BUFFER_SIZE];
    SecureZeroMemory(receiveBuffer, sizeof(receiveBuffer));
    INT receiveBufferLength = sizeof(receiveBuffer);
    SOCKADDR_IN remoteAddrIn;
    SecureZeroMemory(&remoteAddrIn, sizeof(remoteAddrIn));
    remoteAddrIn.sin_family = AF_INET;
    INT remoteAddrInLength = sizeof(remoteAddrIn);
    if (recvfrom(sock, receiveBuffer, receiveBufferLength, 0, (SOCKADDR *)&remoteAddrIn, &remoteAddrInLength) == SOCKET_ERROR)
    {
        wprintf(L"Failed: recvfrom(): 0x%08x\n", WSAGetLastError());
        goto Cleanup;
    }

    const int IPV4_IP_ADDRESS_LENGTH = 16;
    WCHAR remoteAddr[IPV4_IP_ADDRESS_LENGTH];
    PCWSTR pRemoteAddr = InetNtop(remoteAddrIn.sin_family, &remoteAddrIn.sin_addr, (PWSTR)remoteAddr, sizeof(remoteAddr) / sizeof(WCHAR));
    if (pRemoteAddr == NULL)
    {
        wprintf(L"Failed: InetNtop(): 0x%08x\n", WSAGetLastError());
        goto Cleanup;
    }
    wprintf(L"Received from %s:%d\n", remoteAddr, remoteAddrIn.sin_port);

Cleanup:

    if (sock != INVALID_SOCKET)
    {
        closesocket(sock);
    }

    WSACleanup();

    return 0;
}
