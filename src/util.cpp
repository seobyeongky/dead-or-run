#include "util.h"
#include "res.h"
#include "thread.h"

#include <io.h>
#include <cstdio>

#include <fstream>
#include <algorithm>
#include <locale>
#include <chrono>

/***********************
�����ڵ� ����
************************/
LPWSTR MyStrcat(LPWSTR dest, LPWSTR destBufEnd, LPCWSTR src)
{
	if(dest == destBufEnd)
	{
		assert(!L"MyStrcat: ���� �ȵǰ� �Լ��� ����߽��ϴ�.");
		return nullptr;
	}

	//���� dest������ �� ��ġ�� ã���ϴ�.
	while(*dest)
	{
		++dest; 
		if(dest == destBufEnd)
		{
			//dest���۰� ���������� ���� ����Դϴ�.
			//���۰� �������� ���ɼ��� �ſ� Ů�ϴ�.
			assert(!L"MyStrcat: dest ���۰� �����ƽ��ϴ�.");
			LPWSTR behind = destBufEnd - 1;
			*behind = L'\0';
			return behind;
		}
	}

	//dest�� �� ��ġ���� ���ʴ�� src�� ���ڸ� �ֽ��ϴ�.
	while(*src)
	{
		*dest = *src;
		++dest;
		++src;
		if(dest == destBufEnd)
		{
			//�� á��.
			LPWSTR behind = destBufEnd - 1;
			*behind = L'\0';
			return behind;
		}
	}

	*dest = L'\0';
	
	return dest;
}

LPWSTR MyStrcpy(LPWSTR dest, LPCWSTR src)
{
	while(*src)
	{
		*(dest++) = *(src++);
		*dest = 0;
	}
	return dest;
}

bool IsDigits(const string & str)
{
	return !str.empty() && find_if(str.begin(), 
        str.end(), [](char c) { return !isdigit(c); }) == str.end();
}

bool IsDigits(const wstring & str)
{
	return !str.empty() && find_if(str.begin(), 
        str.end(), [](wchar_t c) { return !isdigit(c); }) == str.end();
}

void Msgbox(LPCWSTR format_string, va_list arg_list, LPCWSTR title)
{
	int length = _vscwprintf(format_string, arg_list) + 1;  
	WCHAR *buf = static_cast<WCHAR *>(
		_malloca(length * sizeof(WCHAR)));
	vswprintf_s(buf, length, format_string, arg_list);
	
	LeaveLog(buf);
	MessageBox(NULL, buf, title, 0U);

	_freea(buf);
}

void ErrorMsg(LPCWSTR format_string, ...)
{
	va_list args;
	va_start(args, format_string);
	Msgbox(format_string, args, L"����");
	va_end(args);
}

DirChanger::DirChanger()
	: _pre_dir()
{
	Init();
}

DirChanger::DirChanger(const wchar_t * new_dir)
	: _pre_dir()
{
	Init();
	Change(new_dir);
}

DirChanger::~DirChanger()
{
	Change(_pre_dir.c_str());
}

void DirChanger::Change(const wchar_t * new_dir)
{
	_wchdir(new_dir);
}

void DirChanger::Init()
{
	size_t			buf_size = 128;
	vector<wchar_t>	buf(buf_size);
	
	for(;;)
	{
		_wgetcwd(&buf[0], buf_size);
		if(buf[0] != L'\0')
			break;
		buf_size *= 2;
		if(buf_size > 1024)
			ErrorMsg(L"�۾� ���丮 ���̰� �ʹ� ����. %d �Ѿ��.", buf_size / 2);
		buf.resize(buf_size);
	}
	_pre_dir.assign(&buf[0], buf.size());
}

class LogMgr
{
private:
	bool		_first;
	wofstream	_file;
	opzMutex	_mutex;
			
public:
	LogMgr() : _first(true), _file(), _mutex() { }
	~LogMgr() { _file.close(); }

	void SaveLog(const wchar_t * msg)
	{
		LOCK(_mutex);

		if(_first)
		{
			_file.imbue(locale( "korean" ) ); 
			_file.open(L"Ƣ��_log.txt", fstream::out);

			typedef chrono::system_clock clock_t;
			auto		now = clock_t::now();
			time_t		now_c = clock_t::to_time_t(now);
			struct tm	parts;
			localtime_s(&parts, &now_c);

			_file	<< L"=======================================================" << endl
					<< L"\t" << (1900 + parts.tm_year) << L"��"
					<< (1 + parts.tm_mon) << L"�� "
					<< parts.tm_mday << L"��\t"
					<< parts.tm_hour << L"�� "
					<< parts.tm_min << L"�� "
					<< parts.tm_sec << L"���� ���" << endl
					<< L"=======================================================" << endl;
			_first = false;
		}

		_file << msg << endl;
	}
};

void LeaveLog(const wchar_t * format_string, ...)
{
	va_list args;
	va_start(args, format_string);

	int length = _vscwprintf(format_string, args) + 1;  
	WCHAR *buf = static_cast<WCHAR *>(
		_malloca(length * sizeof(WCHAR)));
	vswprintf_s(buf, length, format_string, args);
	
	static LogMgr mgr;
	mgr.SaveLog(buf);

	_freea(buf);
	va_end(args);
}


Color color_table[8] = {
	Color(168,8,8), Color(8, 52, 152), Color(32, 144, 112),
	Color(250, 250, 124), Color(0, 127, 127), Color(200, 162, 144),
	Color(64, 64, 64), Color(213, 197, 6)};

int color_stack_ptr = 0;

const Color & PopColor()
{
	++color_stack_ptr;
	return color_table[(color_stack_ptr - 1) % 8];
}

void ReturnColor()
{
	if(--color_stack_ptr < 0)
	{
		LeaveLog(L"ReturnColor : ������ �ʹ� ���� �ݳ��߽��ϴ�.");
		color_stack_ptr = 0;
	}
}

bool IsValidAddr(const wstring & str)
{
	int state = 0;
	int dot_cnt = 0;
	int num = 0;

	for(size_t i = 0; i < str.length(); ++i)
	{
		wchar_t ch = str[i];

		switch(state)
		{
		case 0:
			if(L'0' <= ch && ch <= L'9')
			{
				num *= 10; num += ch - L'0';
				if(num > 255) return false;
				state = 1;
			}
			else return false;
			break;

		case 1:
			if(L'0' <= ch && ch <= L'9')
			{
				num *= 10; num += ch - L'0';
				if(num > 255) return false;
				state = 1;
			}
			else if(ch == L'.')
			{
				if(++dot_cnt > 4) return false;
				state = 0;
				num = 0;
			}
		}
	}

	return true;
}