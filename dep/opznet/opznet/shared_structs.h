#pragma once

#include <string>
#include <vector>

#if defined(OPZNET_COMMON_DLL)
#define	OPZNET_COMMON_IMPL	__declspec( dllexport )
#else
#define	OPZNET_COMMON_IMPL	__declspec( dllimport )
#endif

namespace opznet
{	
	typedef	unsigned short	ID;

	typedef unsigned char	byte_t;

	// 8 bits integer types
	typedef signed   char Int8;
	typedef unsigned char Uint8;

	// 16 bits integer types
	typedef signed   short Int16;
	typedef unsigned short Uint16;

	// 32 bits integer types
	typedef signed   int Int32;
	typedef unsigned int Uint32;

	// 64 bits integer types
	#if defined(_MSC_VER)
		typedef signed   __int64 Int64;
		typedef unsigned __int64 Uint64;
	#else
		typedef signed   long long Int64;
		typedef unsigned long long Uint64;
	#endif
		
	#define GAME_NAME_BUF_LENGTH	32
	#define	SERVER_NAME_BUF_SIZE	32
	#define	CLIENT_NAME_BUF_SIZE	32

	struct client_t
	{
		ID				id;
		bool			is_member;
		std::wstring	name;
	};

	#pragma warning(disable : 4251)	// STL 벡터 DLL IMPORT하는 코드에서 사용못한다는 경고...

	class OPZNET_COMMON_IMPL Packet
	{
		typedef bool (Packet::*BoolType)(std::size_t);
	public:
		Packet();
		virtual ~Packet();
		
		void			Append(const void * data, size_t byte_size);
		void			Clear();
		const void *	GetData() const;
		size_t			GetByteSize() const;
		void			Extract(void * dest, size_t byte_size);
		bool			EndOfPacket() const;

		operator BoolType() const;

		Packet& operator >>(bool&         data);
		Packet& operator >>(Int8&         data);
		Packet& operator >>(Uint8&        data);
		Packet& operator >>(Int16&        data);
		Packet& operator >>(Uint16&       data);
		Packet& operator >>(Int32&        data);
		Packet& operator >>(Uint32&       data);
		Packet& operator >>(char&         data);
		Packet& operator >>(wchar_t&      data);
		Packet& operator >>(float&        data);
		Packet& operator >>(double&       data);
		Packet& operator >>(char*         data);
		Packet& operator >>(std::string&  data);
		Packet& operator >>(wchar_t*      data);
		Packet& operator >>(std::wstring& data);

		Packet& operator <<(bool                data);
		Packet& operator <<(Int8                data);
		Packet& operator <<(Uint8               data);
		Packet& operator <<(Int16               data);
		Packet& operator <<(Uint16              data);
		Packet& operator <<(Int32               data);
		Packet& operator <<(Uint32              data);
		Packet& operator <<(char                data);
		Packet& operator <<(wchar_t             data);
		Packet& operator <<(float               data);
		Packet& operator <<(double              data);
		Packet& operator <<(const char*         data);
		Packet& operator <<(const std::string&  data);
		Packet& operator <<(const wchar_t*      data);
		Packet& operator <<(const std::wstring& data);

	private:
		bool operator ==(const Packet& right) const;	// 비교 금지
		bool operator !=(const Packet& right) const;	// 비교 금지

		bool checkSize(size_t size);

		std::vector<char>	m_data;    ///< Data stored in the packet. never use outside of this class
		size_t				m_readPos; ///< Current reading position in the packet
		bool				m_isValid; ///< Reading state of the packet
	};

	#pragma warning(default : 4251)
}