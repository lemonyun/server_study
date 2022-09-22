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
    WSAOVERLAPPED overlapped = {};
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int32 recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
    cout << "Data Recv Len Callback = " << recvLen << endl;
    // TODO : 에코 서버를 만든다면 여기서 WSASend를 호출

    Session* session = (Session*)overlapped;

}

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

   
    
    // Overlapped 모델 (Completion Routine 콜백 기반)
    // - 비동기 입출력 지원하는 소켓 생성
    // - 비동기 입출력 함수 호출 (완료 루틴의 시작 주소 (함수 포인터)를 넘겨준다.)
    // - 비동기 작업이 바로 완료되지 않으면, pending 오류 코드가 반환됨
    // - 비동기 입출력 함수 호출한 쓰레드를 -> Alertable Wait 상태로 만든다.
    //  ex) WaitForSingleObjectEx, WaitForMultipleObjectsEx, SleepEx, WSAWaitForMultipleEvents
    // - 비동기 I/O 완료되면 운영체제는 완료 루틴 호출
    // - 완료 루틴 호출이 모두 끝나면 쓰레드는 Alertable Wait 상태에서 빠져 나온다.
    // - WSAWaitForMultipleEvents 함수 호출해서 이벤트 객체의 signal 판별
    // - WSAGetOverlappedResult 호출해서 비동기 입출력 결과 확인 및 데이터 처리


    // 1) 비동기 소켓
    // 2) 넘겨준 overlapped 구조체
    // 3) 전송된 바이트 수
    // 4) 비동기 입출력 작업이 끝날때 까지 대기할지?
    // 5) 비동기 입출력 작업 관련 부가 정보 (거의 사용 안함)
    // WSAGetOverlappedResult

    // 1) 오류 발생시 0 아닌값
    // 2) 전송 바이트 수
    // 3) 비동기 입출력 함수 호출 시 넘겨준 WSAOVERLAPPED 구조체의 주소값
    // 4) 0
    // void CompletionRoutine()


    // Select 모델
    // ㄴ 장점) 윈도우 / 리눅스 공통 (안드로이드 가능)
    // ㄴ 단점) 성능 최하 (매번 소켓을 소켓 셋에 등록하는 비용), 소켓 셋 크기 64개 제한
    // WSAAsyncSelect 모델 = 소켓 이벤트를 윈도우 메세지 형태로 처리. (일반 윈도우 메세지랑 같이 처리하니 성능이 ...)
    
    // WSAEventSelect 모델
    // ㄴ 장점) 비교적 성능이 뛰어남
    // ㄴ 단점) WSAWaitForMultipleEvents가 64개 제한
    // Overlapped (이벤트 기반)
    // ㄴ 장점) 성능
    // ㄴ 단점) 64개 제한
    // Overlapped (콜백 기반)
    // ㄴ 장점) 성능
    // ㄴ 단점) 모든 비동기 소켓 함수에서 사용 가능하진 않음 (accept) 빈번한 Alertable Wait으로 인한 성능 저하
    // ㄴ APC 큐에 있는 일감을 다른 스레드가 처리 할 수 없다.
    
    // IOCP

    // Reactor Pattern (~뒤늦게. 논블로킹 소켓. 소켓 상태 확인 후 -> 뒤늦게 recv send 호출)
    // Proactor Pattern (~미리. 예약. Overlapped WSA~)


    // 클라에서는 어떤 모델을 써도 상관이 없음

    while (true)
    {
        SOCKADDR_IN clientAddr;
        int32 addrLen = sizeof(clientAddr);

        SOCKET clientSocket;

        while (true)
        {
            clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
                break;

            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            return 0;
        }

        Session session = Session{ clientSocket };
        WSAEVENT wsaEvent = ::WSACreateEvent();
        session.overlapped.hEvent = wsaEvent;

        cout << "Client Connected !" << endl;

        while (true)
        {
            WSABUF wsaBuf;
            wsaBuf.buf = session.recvBuffer;
            wsaBuf.len = BUFSIZE;

            DWORD recvLen = 0;
            DWORD flags = 0;
            // wsaBuf 구조체 메모리는 recv 호출하고 날려도 되지만 recvBuffer는 건드리면 안됨
            // 비동기 방식의 recv 호출
            if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
            {
                if (::WSAGetLastError() == WSA_IO_PENDING)
                {
                    // Pending
                    // Alertable Wait
                    // Alertable Wait 상태에서 비동기 입출력이 완료되면 스레드가 가진 APC 큐에 있는 콜백 함수들이 호출된다.
                    // 한 번 Alertable Wait 상태에 진입하면 APC 큐에 여러 개의 콜백 함수가 있다고 해도 모두 실행한다.
                    // 완료 루틴이 모두 끝나면 Alertable Wait 상태에서 원래대로 돌아온다.
                    // 후보 1 ::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);
                    ::SleepEx(INFINITE, TRUE);
                    //  콜백 함수가 끝나면 여기로 되돌아 온다.
                }
                else
                {
                    // TODO: 문제있는 상황
                    break;
                }
            }
            else
            {
                cout << "Data Recv Len = " << recvLen << endl;  
            }


        }

        ::closesocket(session.socket);
        //::WSACloseEvent(wsaEvent);
    }



    // 윈속 종료
    ::WSACleanup();

}