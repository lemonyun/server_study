#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


int main()
{
    // 윈속 초기화 (ws2_32 라이브러리 초기화)
    // 관련 정보가 wsaData에 채워짐
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    // ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
    // type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
    // protocol : 0
    // return : descriptor
    
    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;

        return 0;
    }

    //목적지 주소 (IPv4)
    SOCKADDR_IN serverAddr; 
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    // serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); << deprecated
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);

    // host to network short
    // big-endian이 네트워크 공식
    // Little-Endian vs Big-Endian

    if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Connect ErrorCode : " << errCode << endl;
        return 0;
    }

    cout << "Connected To Server!" << endl;

    while (true)
    {
        char sendBuffer[100] = "Hello World!";
        
        for (int32 i = 0; i < 10; i++)
        {
            int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
            if (resultCode == SOCKET_ERROR)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Send ErrorCode : " << errCode << endl;
                return 0;
            }
        }

        
        cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
        /*
        char recvBuffer[1000];

        int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (recvLen <= 0)
        {
            int32 errCode = WSAGetLastError();
            cout << "Recv ErrorCode : " << errCode << endl;
            return 0;
        }

        cout << "Recv Data! Data = " << recvBuffer << endl;
        cout << "Recv Data! Len = " << recvLen << endl;*/

        this_thread::sleep_for(1s);
    }


    // 소켓 리소스 반환
    ::closesocket(clientSocket);

    // 윈속 종료
    ::WSACleanup();
}