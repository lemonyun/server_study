#include "pch.h"
#include <iostream>

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
    // 윈속 초기화 (ws2_32 라이브러리 초기화)
    // 관련 정보가 wsaData에 채워짐
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    // ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
    // type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
    // protocol : 0
    // return : descriptor
    
    SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        HandleError("Socket");
        return 0;
    }

    //목적지 주소 (IPv4)
    SOCKADDR_IN serverAddr; 
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    // serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); << deprecated
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);

    // Connected UDP 설정
    ::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

    while (true)
    {
        
        
        // 나의 IP 주소 + 포트 번호 설정 (bind됨, client port는 명시적으로 지정하지 않아도 알아서 결정됨)
        // TCP는 connect 할때 결정됨
        
		char sendBuffer[100] = "Hello World!";

        // Unconnected UDP
		/*int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, 
            (SOCKADDR*)&serverAddr, sizeof(serverAddr));*/

        // Connected UDP
        int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
		
        
        if (resultCode == SOCKET_ERROR)
		{
			HandleError("SendTo");
			return 0;
		}


		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
        
        SOCKADDR_IN recvAddr;
        ::memset(&recvAddr, 0, sizeof(recvAddr));
        int32 addrLen = sizeof(recvAddr);

        char recvBuffer[1000];

        //Unconnected UDP
        //int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0, 
        //    (SOCKADDR*)&recvAddr, &addrLen);

        //Connected UDP
        int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

        if (recvLen <= 0)
        {
            HandleError("RecvFrom");
            return 0;
        }

        cout << "Recv Data! Data = " << recvBuffer << endl;
        cout << "Recv Data! Len = " << recvLen << endl;

        this_thread::sleep_for(1s);
    }


    // 소켓 리소스 반환
    ::closesocket(clientSocket);

    // 윈속 종료
    ::WSACleanup();
}