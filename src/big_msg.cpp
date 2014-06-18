#include "big_msg.h"
#include "res.h"

BigMsg::BigMsg()
	: _big_msg(L"", normal_font, 30U),
	_cur_big_msg(nullptr),
	_big_msg_queue()
{
	_big_msg.setColor(Color(255, 255, 255));
	_big_msg.setPosition(static_cast<float>(window.getSize().x / 2), 160.f);
	_big_msg.setStyle(Text::Bold);
}

BigMsg::~BigMsg()
{
	SafeDelete(_cur_big_msg);
}

void BigMsg::Update()
{
	if(_cur_big_msg == nullptr)
	{
		if(!_big_msg_queue.empty())
		{
			_cur_big_msg = new big_msg_t(_big_msg_queue.front());
			_big_msg.setString(_cur_big_msg->second);
			_big_msg.setOrigin(_big_msg.getLocalBounds().width / 2.f, 0.f);
			_big_msg_queue.pop();
		}
	}
	else
	{
		if(_cur_big_msg->first + 3000 < present_time)
		{
			// times UP!
			if(!_big_msg_queue.empty())
			{
				_cur_big_msg->first = present_time;
				_cur_big_msg->second = _big_msg_queue.front().second;
				_big_msg.setString(_cur_big_msg->second);
				_big_msg.setOrigin(_big_msg.getLocalBounds().width / 2.f, 0.f);
				_big_msg_queue.pop();
			}
			else SafeDelete(_cur_big_msg);
		}
		else
		{
			// 효과 적용

		}
	}
}

void BigMsg::Show(const wchar_t * format_msg_str, ...)
{
	va_list args;
	va_start(args, format_msg_str);
	int length = _vscwprintf(format_msg_str, args) + 1;  
	WCHAR *buf = static_cast<WCHAR *>(
		_malloca(length * sizeof(WCHAR)));
	vswprintf_s(buf, length, format_msg_str, args);
	_big_msg_queue.push(big_msg_t(present_time, buf));
	_freea(buf);
	va_end(args);
}

void BigMsg::draw(RenderTarget& target, RenderStates states) const
{
	if(_cur_big_msg != nullptr)
		target.draw(_big_msg);
}