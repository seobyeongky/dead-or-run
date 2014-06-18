#pragma once

#ifdef OPZNET_SERVER_DLL
#define	OPZNET_SERVER_IMPL	__declspec( dllexport )
#else
#define	OPZNET_SERVER_IMPL	__declspec( dllimport )
#endif

#include <opznet/shared_structs.h>

#include <vector>

namespace opznet
{
	enum server_msg_type_t
	{
		// 새로운 클라이언트 접속 타입입니다.
		// client_info에 새로 접속한 클라이언트 정보가 들어있습니다.
		SVMSG_NEW_CLIENT,

		// 클라이언트가 서버를 떠났습니다.
		// client_info에 떠난 클라이언트 정보가 들어있습니다.
		SVMSG_CLIENT_GONE,

		// 클라이언트로부터 패킷을 받았습니다.
		// client_info에 보낸 클라이언트 정보가 들어있습니다.
		// packet에 보낸 패킷 정보가 들어있습니다.
		SVMSG_PACKET_RECV,

		// 클라이언트의 지연 시간 계산이 갱신됐습니다.
		// client_info에 계산 완료된 클라이언트의 정보가 들어있습니다.
		// delay_time에 밀리세컨드 단위의 지연 시간 정보가 들어있습니다.
		SVMSG_CLIENT_DELAY_TIME_UPDATED,

		// 맨 처음에 초기화 작업 때 포트 바인딩이 실패한 경우입니다.
		// 현재 컴퓨터에서 다른 프로세스가 이미 그 포트를 잡고 있기 때문에 발생합니다.
		SVMSG_BIND_FAILED
	};

	// 서버 메시지 구조체입니다. type에 따라서 메시지 타입이 나뉩니다.
	struct server_msg_t
	{
		server_msg_type_t			type;
		client_t					client_info;
		Packet						packet;
		int							delay_time;
	};

	// 오파츠넷 서버 서비스를 초기화시키고 시작시키는 함수입니다.
	// 이미 서버 서비스가 켜져있다면 false를 반환합니다.
	// 서버를 시작하는데 성공하면 true를 반환합니다.
	OPZNET_SERVER_IMPL	bool	BeginServer(const std::wstring &server_name);

	// 오파츠넷 서버 서비스를 종료시키는 함수입니다.
	// 꺼질 때까지 약간의 시간(약 3초정도?)이 걸립니다.
	OPZNET_SERVER_IMPL	void	EndServer();

	// 서버 서비스로부터 메시지를 받습니다.
	// 아무런 메시지도 없으면 false를 반환합니다.
	// 메시지가 있으면 true를 반환합니다.
	OPZNET_SERVER_IMPL	bool	GetServerMsg(server_msg_t *msg);
	
	// TCP 프로토콜을 사용해서 패킷을 전송합니다.
	OPZNET_SERVER_IMPL	void	SafeSend(ID client_id, const Packet &packet);

	// UDP 프로토콜을 사용해서 패킷을 전송합니다.
	// 특정 클라이언트에게 보낼 수 있습니다. (유니케스트)
	OPZNET_SERVER_IMPL	void	UnsafeSend(ID client_id, const Packet &packet);

	// 클라이언트와 연결을 끊습니다.
	OPZNET_SERVER_IMPL	void	DisconnectClient(ID client_id);

	// UDP 프로토콜을 사용해서 패킷을 전송합니다.
	// 여러 클라이언트에게 한꺼번에 보낼 수 있습니다. (멀티케스트)
	// 아직 지원 안함
	// OPZNET_SERVER_IMPL	void	UnsafeSendMultiTarget(std::vector<ID> &client_id_vector, const char * data, size_t byte_size);
}