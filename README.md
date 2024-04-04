# IOCP 채팅 프로그램
- 2024.02~ / 개인프로젝트 <br/>
- IOCP 서버를 이용한 채팅 프로그램

## 주요 기능
- 닉네임 입력하여 접속
- 방 입장/퇴장
- 채팅

## 구조
### Server
- ServerNetLib: 소켓 통신과 관련된 일을 하는 정적 라이브러리
- ChatServerLib: 클라이언트로부터 받은 채팅 관련 패킷을 처리하여 응답을 보내는 일을 하는 정적 라이브러리
- iocp_server: 라이브러리를 사용하는 콘솔 프로그램

### Client
- ClientNetLib: 소켓 통신과 관련된 일을 하는 정적 라이브러리
- ChatClientLib: 채팅과 관련된 패킷을 서버에 요청하고 응답을 받아 처리하는 일을 하는 정적 라이브러리
- iocp_client: 라이브러리를 사용하는 콘솔 프로그램
- DummyClient: 500ms마다 서버에 패킷을 보내는 N개의 스레드를 실행하는 콘솔 프로그램

### Common
- CommonLib: Server, Client가 공유하는 ErrorCode, Packet 관련 정의

### thirdparty
- spdlog
