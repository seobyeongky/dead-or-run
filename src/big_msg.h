/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	big_msg.h

	custom SFML BigMsg class의 header file 입니다.
=======================================================*/

#pragma once

#include "internal.h"

#include <queue>

class BigMsg : public Drawable
{
public:
	BigMsg();
	~BigMsg();

	void Update();

	void Show(const wchar_t * format_msg_str, ...);

private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

private:
	// time segment, msg
	typedef pair<int, wstring> big_msg_t;

private:
	Text						_big_msg;
	big_msg_t	*				_cur_big_msg;
	queue<big_msg_t>			_big_msg_queue;
};