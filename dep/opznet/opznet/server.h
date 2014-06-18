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
		SVMSG_NEW_CLIENT,
		SVMSG_CLIENT_GONE,
		SVMSG_PACKET_RECV,
		SVMSG_CLIENT_DELAY_TIME_UPDATED,
		SVMSG_BIND_FAILED
	};

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