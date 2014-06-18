#include "string_converter.h"

const size_t MY_BUFSIZE = 1024;

void multi2uni(const std::string & src, std::wstring * dest)
{
	wchar_t buf[MY_BUFSIZE];
	const char * srcptr = src.c_str();
	dest->clear();
	size_t converted;
	errno_t err = mbstowcs_s(&converted, buf, srcptr, MY_BUFSIZE - 1);
	dest->assign(buf);
}

void uni2multi(const std::wstring & src, std::string * dest)
{
	char buf[MY_BUFSIZE];
	const wchar_t * srcptr = src.c_str();
	dest->clear();
	size_t converted;
	errno_t err = wcstombs_s(&converted, buf, srcptr, MY_BUFSIZE - 1);
	dest->assign(buf);
}
