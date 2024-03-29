## 개요
- 기간: 2024.02~
- 한줄소개: IOCP 서버를 활용한 채팅 프로그램
- 목적: 소켓 프로그래밍과 멀티스레딩에 대한 이해
- 종류: 콘솔 프로그램
- 인원: 1명
- 언어: C++
- IDE: Visual Studio 2022


## 구조
### 1. Server
- ServerNetLib: 소켓 통신과 관련된 일을 하는 정적 라이브러리
- ChatServerLib: 클라이언트로부터 받은 채팅 관련 패킷을 처리하여 응답을 보내는 일을 하는 정적 라이브러리
- iocp_server: 라이브러리를 사용하는 콘솔 프로그램

### 2. Client
- ClientNetLib: 소켓 통신과 관련된 일을 하는 정적 라이브러리
- ChatClientLib: 채팅과 관련된 패킷을 서버에 요청하고 응답을 받아 처리하는 일을 하는 정적 라이브러리
- iocp_client: 라이브러리를 사용하는 콘솔 프로그램

### 3. Common
- ErrorCode, Packet 관련 정의

### 4. thirdparty
- 외부 라이브러리


## 주요 특징
- IOCP, 비동기 함수 사용: AcceptEx, WSARecv, WSASend)
- Worker Thread 6개: Recv(4), Accept(1), Packet Process(1)


## 서비스 흐름


## 성능
- DummyClient: 주기적으로 서버에 패킷을 보내는 1000개 스레드를 실행
- 콘솔 출력에 따른 지연 시간 줄이기 위해 spdlog 라이브러리를 활용하여 파일에 로깅
