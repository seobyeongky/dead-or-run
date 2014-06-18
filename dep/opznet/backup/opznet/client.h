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
		// 서버에 연결 실패했을 때 입니다.
		// error_msg에 실패 메시지 정보가 들어있습니다.
		CLMSG_CONNECT_FAILED,

		// 서버에 연결 성공했을 때 입니다.
		// server_info에 연결된 서버의 정보가 들어있습니다.
		// my_id에 서버로부터 발급받은 고유 클라이언트 id가 들어있습니다.
		CLMSG_CONNECTED,

		// 서버와 연결 끊겼을 때 입니다.
		// error_msg에 오류 메시지 정보가 들어있을 겁니다.
		CLMSG_DISCONNECTED,

		// 처음 발견한 다른 클라이언트를 소개할 때 입니다.
		// client_info에 다른 클라이언트 정보가 들어있습니다.
		CLMSG_CLIENT_INTRODUCTION,

		// 클라이언트가 서버를 떠났을 때 입니다.
		// client_info에 떠난 클라이언트 정보가 들어있습니다.
		CLMSG_CLIENT_GONE,

		// 서버로부터 패킷을 받았을 때 입니다.
		// server_info 서버 정보가
		// packet에 패킷 정보가 들어있습니다.
		CLMSG_PACKET_RECV,

		// 예상치 못한 네트웍 오류가 발생했을 때 입니다.
		// error_msg에 오류 내용이 들어있습니다.
		CLMSG_UNEXPECTED_ERROR,

		// 로컬 네트웍 서버 찾기 명령을 수행한 후에,
		// 로컬 네트웍 서버를 발견했을 때 입니다.
		// server_info에 찾은 서버 정보가 들어있습니다.
		CLMSG_LOCAL_SERVER_FOUND,

		// 로컬 네트웍 서버 찾기 명령을 수행한 후에,
		// 작업이 완료됐음을 알리는 신호입니다.
		CLMSG_FINDING_LOCAL_SERVER_OVER
	};

	// 서버의 정보를 담은 구조체입니다.
	struct server_info_t
	{
		std::wstring		name;
		std::wstring		address;
		size_t				num_of_clients;
		size_t				max_num_of_clients;
	};

	// 클라이언트 메시지 구조체입니다. type에 타입 정보가 들어있습니다.
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