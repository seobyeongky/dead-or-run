/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	util.h

	�̰����� �پ��� �Լ����� ������ header file�Դϴ�.
=======================================================*/

#pragma once

#include "internal.h"
#include "res.h"

// strcat�Լ�ó�� �����մϴ�.
// �׸��� �ι��ڰ� �ִ� ������ �ּҰ��� ��ȯ�մϴ�.
// destBufEnd�� ������ ������ �ּҰ��� �־��ּ���.
LPWSTR MyStrcat(LPWSTR dest, LPWSTR destBufEnd, LPCWSTR src);

// strcpyó�� �����մϴ�.
LPWSTR MyStrcpy(LPWSTR dest, LPCWSTR src);

// �޽��� �ڽ��� ���� �޽����� ���ϴ�.
void ErrorMsg(LPCWSTR format_string, ...);

// �α׸� ����ϴ�.
void LeaveLog(const wchar_t * format_string, ...);

// �� ���ڿ��� ���ڷ� �̷�����ִ��� Ȯ���մϴ�.
bool IsDigits(const string & str);
bool IsDigits(const wstring & str);

// �۾� ���͸��� ��������ݴϴ�.
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

			DirChanger(const DirChanger &rhs);	// ���� ����
			DirChanger & operator = (const DirChanger &rhs); //���� ����
};

template <typename T>
inline void SetMiddleOfScreen(T * object)
{
	object->setPosition(
		static_cast<float>(window.getSize().x / 2),
		static_cast<float>(window.getSize().y / 2)
		);
}

// sprite�� �߽��� �߾����� ��ġ��ŵ�ϴ�.
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

