/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	util.h

	이것저것 다양한 함수들을 선언한 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "res.h"

// strcat함수처럼 동작합니다.
// 그리고 널문자가 있는 원소의 주소값을 반환합니다.
// destBufEnd에 버퍼의 마지막 주소값을 넣어주세요.
LPWSTR MyStrcat(LPWSTR dest, LPWSTR destBufEnd, LPCWSTR src);

// strcpy처럼 동작합니다.
LPWSTR MyStrcpy(LPWSTR dest, LPCWSTR src);

// 메시지 박스로 에러 메시지를 띄웁니다.
void ErrorMsg(LPCWSTR format_string, ...);

// 로그를 남깁니다.
void LeaveLog(const wchar_t * format_string, ...);

// 이 문자열이 숫자로 이루어져있는지 확인합니다.
bool IsDigits(const string & str);
bool IsDigits(const wstring & str);

// 작업 디렉터리를 변경시켜줍니다.
class DirChanger
{
public:
	 DirChanger();
	 DirChanger(const wchar_t * new_dir);
	~DirChanger();

	void Change(const wchar_t * new_dir);

private:
	wstring		_pre_dir;

	void	Init();

			DirChanger(const DirChanger &rhs);	// 복사 방지
			DirChanger & operator = (const DirChanger &rhs); //복사 방지
};

template <typename T>
inline void SetMiddleOfScreen(T * object)
{
	object->setPosition(
		static_cast<float>(window.getSize().x / 2),
		static_cast<float>(window.getSize().y / 2)
		);
}

// sprite의 중심을 중앙으로 위치시킵니다.
template <typename T>
inline void SetMiddleOfLocal(T * object)
{
	auto &bounds = object->getLocalBounds();
	object->setOrigin(
		bounds.width / 2,
		bounds.height / 2
		);
}

template <typename T>
inline void SetScaleToSize(T * object, float width, float height)
{
	auto &bounds = object->getLocalBounds();
	object->setScale(width / bounds.width,
				height / bounds.height);
}

template<typename T>
inline void FitSizeToScreen(T * object)
{
	auto &size = window.getSize();
	object->setSize(Vector2f(	static_cast<float>(size.x),
								static_cast<float>(size.y)));
}

template<typename T>
inline void FitScaleToScreen(T * object)
{
	auto &size = window.getSize();
	SetScaleToSize(	object,
					static_cast<float>(size.x),
					static_cast<float>(size.y));
}

const Color & PopColor();
void ReturnColor();

bool IsValidAddr(const wstring & str);

template<typename T>
inline T ToPositive(T x)
{
	if(x < 0) return -x;
	else return x;
}

template<typename T>
inline T ToNegative(T x)
{
	return -ToPositive(x);
}

