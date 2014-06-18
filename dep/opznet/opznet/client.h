#pragma once

#ifdef OPZNET_CLIENT_DLL
#define	OPZNET_CLIENT_IMPL	__declspec( dllexport )
#else
#define	OPZNET_CLIENT_IMPL	__declspec( dllimport )
#endif

#include <string>
#include <opznet/shared_structs.h>

namespace opznet
{
	enum client_msg_type_t
	{
		CLMSG_CONNECT_FAILED,
		CLMSG_CONNECTED,
		CLMSG_DISCONNECTED,
		CLMSG_CLIENT_INTRODUCTION,
		CLMSG_CLIENT_GONE,
		CLMSG_PACKET_RECV,
		CLMSG_UNEXPECTED_ERROR,
		CLMSG_LOCAL_SERVER_FOUND,
		CLMSG_FINDING_LOCAL_SERVER_OVER
	};

	struct server_info_t
	{
		std::wstring		name;
		std::wstring		address;
		size_t				num_of_clients;
		size_t				max_num_of_clients;
	};

	struct client_msg_t
	{
		client_msg_type_t	type;
		client_t			client_info;
		Packet				packet;
		ID					my_id;
		server_info_t		server_info;
		const wchar_t *		error_msg;
	};

	//오파츠넷 클라이언트 서비스를 초기화시키고 시작시키는 함수입니다.
	//이미 클라이언트 서비스가 켜져있다면 false를 반환합니다.
	//시작하는데 성공하면 true를 반환합니다.
	OPZNET_CLIENT_IMPL	bool	BeginClient();

	//오파츠넷 클라이언트를 종료시키는 함수입니다.
	//꺼질 때까지 약간의 시간(최대 3초정도?)이 걸립니다.
	OPZNET_CLIENT_IMPL	void	EndClient();

	//클라이언트 메시지를 받습니다. 아무런 메시지도 없으면 false를 반환합니다.
	OPZNET_CLIENT_IMPL	bool	GetClientMsg(client_msg_t *msg);
	
	//서버에 접속을 요청하는 함수입니다.
	//오파츠넷 클라이언트 서비스가 대기상태에 있지 않아서 잘못된 요청이라면 false를 반환합니다...
	//요청하는데 성공했으면 true를 반환합니다.
	//대략 5초정도 기다립니다.
	OPZNET_CLIENT_IMPL	bool	RequestConnectToServer(const wchar_t * server_address, const wchar_t * user_name);

	// TCP 프로토콜을 사용해서 서버에게 패킷을 전송합니다.
	OPZNET_CLIENT_IMPL	void	SafeSend(const Packet &packet);

	//서버와의 접속을 끊습니다. 접속을 끊는데 성공했으면 true를 반환합니다.
	//현재 서버와 접속 상태가 아니면 false를 반환합니다.
	OPZNET_CLIENT_IMPL	bool	DisconnectToServer();

	//지역 네트워크에 존재하는 서버를 검색합니다.
	//이미 찾고 있는 상태라면 false를 반환합니다...
	//요청하는데 성공했으면 true를 반환합니다.
	OPZNET_CLIENT_IMPL	bool	RequestSearchLocalNetServers();
}