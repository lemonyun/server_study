#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << "Socket ErrorCode : " << errCode << endl;
}

int main()
{



    // TCP 서버

    /*
    // 윈속 초기화 (ws2_32 라이브러리 초기화)
    // 관련 정보가 wsaData에 채워짐
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    // ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
    // type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
    // protocol : 0
    // return : descriptor

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;

        return 0;
    }

    //목적지 주소 (IPv4)
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);


    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Bind ErrorCode : " << errCode << endl;
        return 0;
    }

    // 영업 시작
    if (::listen(listenSocket, 10) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Listen ErrorCode : " << errCode << endl;
        return 0;
    }

    while (true)
    {
        SOCKADDR_IN clientAddr;
        ::memset(&serverAddr, 0, sizeof(serverAddr));
        int32 addrLen = sizeof(clientAddr);
        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

        // clientaddr와 clientsocket을 보관해두면 연결이 끊어지지 않는 한 클라이언트와 계속 통신할 수 있다.

        if (clientSocket == INVALID_SOCKET)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "Accept ErrorCode : " << errCode << endl;
            return 0;
        }

        char ipAddress[16];
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
        cout << "Client Connected! IP = " << ipAddress << endl;

        while (true)
        {
            char recvBuffer[1000];
            
            this_thread::sleep_for(1s);
            int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (recvLen <= 0)
            {
                int32 errCode = WSAGetLastError();
                cout << "Recv ErrorCode : " << errCode << endl;
                return 0;
            }

            cout << "Recv Data! Data = " << recvBuffer << endl;
            cout << "Recv Data! Len = " << recvLen << endl;
            
            
            int32 resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
            if (resultCode == SOCKET_ERROR)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Send ErrorCode : " << errCode << endl;
                return 0;
            }


        }
    }


    // 윈속 종료
    ::WSACleanup();
    */

    // 윈속 초기화 (ws2_32 라이브러리 초기화)
    // 관련 정보가 wsaData에 채워짐
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        HandleError("Socket");

        return 0;
    }

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);

    if (::bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        HandleError("Bind");
        return 0;
    }

    while (true)
    {
        SOCKADDR_IN clientAddr;
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int32 addrLen = sizeof(clientAddr);
        
        this_thread::sleep_for(1s);

        char recvBuffer[1000];

        int32 recvLen = ::recvfrom(serverSocket, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&clientAddr, &addrLen);

        if (recvLen <= 0)
        {
            HandleError("RecvFrom");
            return 0;
        }

        cout << "Recv Data! Data = " << recvBuffer << endl;
        cout << "Recv Data! Len = " << recvLen << endl;

        int32 errorCode = ::sendto(serverSocket, recvBuffer, recvLen, 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));

        if (errorCode == SOCKET_ERROR)
        {
            HandleError("SendTo");
            return 0;
        }

        cout << "Send Data! Len = " << recvLen << endl;
    }

    // 윈속 종료
    ::WSACleanup();

}