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
};

enum IO_TYPE
{
    READ,
    WRITE,
    ACCEPT,
    CONNECT,
};

struct OverlappedEx
{
    WSAOVERLAPPED overlapped = {};
    int32 type = 0; // read, write, accept, connect
};


void WorkerThreadMain(HANDLE iocpHandle)
{
    while (true)
    {
        DWORD bytesTransferred = 0; // 송수신된 바이트 길이를 받아줌
        Session* session = nullptr; 
        OverlappedEx* overlappedEx = nullptr;
        bool ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);
        // 일감이 생길 때 까지 대기중

        if (ret == FALSE || bytesTransferred == 0)
        {
            continue;
        }

        ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);

        cout << "Recv Data IOCP = " << bytesTransferred << endl;

        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        DWORD recvLen = 0;
        DWORD flags = 0;
        ::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
    }
}

int main()
{

    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
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
    
    // IOCP (Completion Port) 모델
    // - APC 큐 대신에 Completion Port가 처리함 (쓰레드마다 있는건 아니고 유일하게 1개 존재한다. 중앙에서 관리하는 APC 큐?)
    // - Alertable Wait 대신 CP 결과 처리를 GetQueueCompletionStatus
    // 쓰레드랑 궁합이 좋다.

    // CreateComletionPort
    // GetQueueCompletionStatus

    vector<Session*> sessionManager;

    // CP 생성 용도, 소켓을 CP에 등록하는 용도
    HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // CP 생성 용도

    // Worker Threads
    // CP를 관찰하면서 완료된 입출력 함수가 있는지 확인하여 데이터 처리
    
    for (int32 i = 0; i < 5; i++)
    {
        GThreadManager->Launch([=]() {WorkerThreadMain(iocpHandle); });
    }

    // Main Thread = Accept 담당
    while (true)
    {
        SOCKADDR_IN clientAddr;
        int32 addrLen = sizeof(clientAddr);

        SOCKET clientSocket = clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
            return 0;

        Session* session = new Session();
        session->socket = clientSocket; 
        sessionManager.push_back(session);

        cout << "Client Connected !" << endl;

        // 소켓을 CP에 등록
        ::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*KEY*/(ULONG_PTR)session, 0); // 소켓을 CP에 등록하는 용도
        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        OverlappedEx* overlappedEx = new OverlappedEx();
        overlappedEx->type = IO_TYPE::READ;

        DWORD recvLen = 0;
        DWORD flags = 0;
        ::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
        // cp를 관찰하면 우리가 등록한 소켓에 해당하는 recv나 send가 완료되었다는 통지가 CP를 통해 온다.
        // 실제 recv 처리는 별도의 스레드에서 처리

        // 유저가 게임 접속 종료 // 문제 발생
        /*Session* s = sessionManager.back();
        sessionManager.pop_back();
        xdelete(s); */
        // 소켓을 등록을 해준 다음에 입출력 함수를 하나라도 걸어주는 순간분터 세션을 날리지 못하도록 막아줘야 한다.
        // 데이터를 처리하는 쪽(worker thread)에서 날리던가 해야 한다.

        //::closesocket(session.socket);
        //::WSACloseEvent(wsaEvent);
    }

    GThreadManager->Join();

    // 윈속 종료
    ::WSACleanup();

}