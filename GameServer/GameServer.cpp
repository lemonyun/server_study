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

const int32 BUFSIZE = 1000;

struct Session
{
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int32 recvBytes = 0;
    int32 sendBytes = 0;
};


int main()
{

    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    // 블로킹 소켓
    // accept -> 접속한 클라가 있을 때
    // connect -> 서버 접속 성공했을 때
    // send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
    // recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 때

    // 논블로킹

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        return 0;
    
    u_long on = 1;
    if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);

    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        return 0;

    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Accept" << endl;

    // Select 모델 = (select 함수가 핵심이 되는 모델)
    // 보통 클라에서 많이 사용하게 될 방식
    // 간단하게 구현할 수 있다는 장점이 있지만
    // 단점 FD_SETSIZE가 64이기 때문에 단일 fd_set에 64개의 소켓만 등록할 수 있다. 그 이상의
    // 소켓을 사용하려면 여러 개의 fd_set을 사용해야 한다.

    // 소켓 함수 호출이 성공할 시점을 미리 알 수 있다.
    // 수신 버퍼에 데이터가 없는데, read 한다거나 송신 버퍼가 꽉 찼는데 write 한다거나
    // - 블로킹 소켓 : 조건이 만족되지 않아서 블로킹되는 상황 예방
    // - 논 블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방

    // socket set
    // 1) 읽기[ ] 쓰기[ ] 예외(OOB)[ ] 관찰 대상 등록
    // OutofBand는 send 마지막 인자로 MSG_OOB로 보내는 특별한 데이터
    // 받는 쪽에서도 OOB 세팅을 해야 읽을 수 있음

    // 2) select(readSet, writeSet, exceptionSet); -> 관찰 시작
    // 3) 적어도 하나의 소켓이 준비되면 리턴 -> 낙오자는 알아서 제거됨
    // 4) 남은 소켓 체크해서 진행 // would block이 뜨는 상황을 예방할 수 있음

    // fd_set read;
    // FD_ZERO() : 비운다.
    // FD_SET(s, &set); 소켓 s를 넣는다.
    // FD_CLR(s, &set); 소켓 s를 제거
    // FD_ISSET 소켓 s가 set에 들어있으면 0이 아닌 값을 리턴한다.


    vector<Session> sessions;
    sessions.reserve(100);

    fd_set reads;
    fd_set writes;

    while (true)
    {
        // 소켓 셋 초기화
        FD_ZERO(&reads);
        FD_ZERO(&writes);

        // ListenSocket 등록
        FD_SET(listenSocket, &reads);

        // 소켓 등록
        for (Session& s : sessions)
        {
            if (s.recvBytes <= s.sendBytes)
                FD_SET(s.socket, &reads);
            else
                FD_SET(s.socket, &writes);
        }
        timeval timeout;
        int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
        if (retVal == SOCKET_ERROR)
            break;


        // Listen 소켓이 reads에 있느냐
        if (FD_ISSET(listenSocket, &reads))
        {
            SOCKADDR_IN clientAddr;
            int32 addrLen = sizeof(clientAddr);
            SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Client Connected" << endl;
                sessions.push_back(Session{ clientSocket });
            }
        }

        // 나머지 소켓 체크

        for (Session& s : sessions)
        {
            // Read
            if (FD_ISSET(s.socket, &reads))
            {
                int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
                if (recvLen <= 0)
                {
                    // TODO : sessions 제거
                    continue;
                }

                s.recvBytes = recvLen;
            }

            //Write
            if (FD_ISSET(s.socket, &writes))
            {
                // 블로킹 모드 -> 모든 데이터 다 보냄
                // 논블로킹 모드 -> 일부만 보낼 수가 있음 (상대방 수신 버퍼 상황에 따라)

                int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == SOCKET_ERROR)
                {
                    // TODO : sessions 제거
                    continue;
                }
                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes)
                {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }

            }


        }

        

    }

    
    // 윈속 종료
    ::WSACleanup();

}