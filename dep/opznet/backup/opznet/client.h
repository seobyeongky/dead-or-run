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
		// ������ ���� �������� �� �Դϴ�.
		// error_msg�� ���� �޽��� ������ ����ֽ��ϴ�.
		CLMSG_CONNECT_FAILED,

		// ������ ���� �������� �� �Դϴ�.
		// server_info�� ����� ������ ������ ����ֽ��ϴ�.
		// my_id�� �����κ��� �߱޹��� ���� Ŭ���̾�Ʈ id�� ����ֽ��ϴ�.
		CLMSG_CONNECTED,

		// ������ ���� ������ �� �Դϴ�.
		// error_msg�� ���� �޽��� ������ ������� �̴ϴ�.
		CLMSG_DISCONNECTED,

		// ó�� �߰��� �ٸ� Ŭ���̾�Ʈ�� �Ұ��� �� �Դϴ�.
		// client_info�� �ٸ� Ŭ���̾�Ʈ ������ ����ֽ��ϴ�.
		CLMSG_CLIENT_INTRODUCTION,

		// Ŭ���̾�Ʈ�� ������ ������ �� �Դϴ�.
		// client_info�� ���� Ŭ���̾�Ʈ ������ ����ֽ��ϴ�.
		CLMSG_CLIENT_GONE,

		// �����κ��� ��Ŷ�� �޾��� �� �Դϴ�.
		// server_info ���� ������
		// packet�� ��Ŷ ������ ����ֽ��ϴ�.
		CLMSG_PACKET_RECV,

		// ����ġ ���� ��Ʈ�� ������ �߻����� �� �Դϴ�.
		// error_msg�� ���� ������ ����ֽ��ϴ�.
		CLMSG_UNEXPECTED_ERROR,

		// ���� ��Ʈ�� ���� ã�� ����� ������ �Ŀ�,
		// ���� ��Ʈ�� ������ �߰����� �� �Դϴ�.
		// server_info�� ã�� ���� ������ ����ֽ��ϴ�.
		CLMSG_LOCAL_SERVER_FOUND,

		// ���� ��Ʈ�� ���� ã�� ����� ������ �Ŀ�,
		// �۾��� �Ϸ������ �˸��� ��ȣ�Դϴ�.
		CLMSG_FINDING_LOCAL_SERVER_OVER
	};

	// ������ ������ ���� ����ü�Դϴ�.
	struct server_info_t
	{
		std::wstring		name;
		std::wstring		address;
		size_t				num_of_clients;
		size_t				max_num_of_clients;
	};

	// Ŭ���̾�Ʈ �޽��� ����ü�Դϴ�. type�� Ÿ�� ������ ����ֽ��ϴ�.
	struct client_msg_t
	{
		client_msg_type_t	type;
		client_t			client_info;
		Packet				packet;
		ID					my_id;
		server_info_t		server_info;
		const wchar_t *		error_msg;
	};

	//�������� Ŭ���̾�Ʈ ���񽺸� �ʱ�ȭ��Ű�� ���۽�Ű�� �Լ��Դϴ�.
	//�̹� Ŭ���̾�Ʈ ���񽺰� �����ִٸ� false�� ��ȯ�մϴ�.
	//�����ϴµ� �����ϸ� true�� ��ȯ�մϴ�.
	OPZNET_CLIENT_IMPL	bool	BeginClient();

	//�������� Ŭ���̾�Ʈ�� �����Ű�� �Լ��Դϴ�.
	//���� ������ �ణ�� �ð�(�ִ� 3������?)�� �ɸ��ϴ�.
	OPZNET_CLIENT_IMPL	void	EndClient();

	//Ŭ���̾�Ʈ �޽����� �޽��ϴ�. �ƹ��� �޽����� ������ false�� ��ȯ�մϴ�.
	OPZNET_CLIENT_IMPL	bool	GetClientMsg(client_msg_t *msg);
	
	//������ ������ ��û�ϴ� �Լ��Դϴ�.
	//�������� Ŭ���̾�Ʈ ���񽺰� �����¿� ���� �ʾƼ� �߸��� ��û�̶�� false�� ��ȯ�մϴ�...
	//��û�ϴµ� ���������� true�� ��ȯ�մϴ�.
	//�뷫 5������ ��ٸ��ϴ�.
	OPZNET_CLIENT_IMPL	bool	RequestConnectToServer(const wchar_t * server_address, const wchar_t * user_name);

	// TCP ���������� ����ؼ� �������� ��Ŷ�� �����մϴ�.
	OPZNET_CLIENT_IMPL	void	SafeSend(const Packet &packet);

	//�������� ������ �����ϴ�. ������ ���µ� ���������� true�� ��ȯ�մϴ�.
	//���� ������ ���� ���°� �ƴϸ� false�� ��ȯ�մϴ�.
	OPZNET_CLIENT_IMPL	bool	DisconnectToServer();

	//���� ��Ʈ��ũ�� �����ϴ� ������ �˻��մϴ�.
	//�̹� ã�� �ִ� ���¶�� false�� ��ȯ�մϴ�...
	//��û�ϴµ� ���������� true�� ��ȯ�մϴ�.
	OPZNET_CLIENT_IMPL	bool	RequestSearchLocalNetServers();
}