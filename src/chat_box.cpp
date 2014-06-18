#include "chat_box.h"
#include "res.h"
#include "play_scene_layout.h"

ChatBox::ChatBox()
	:  _chatting_text(normal_font, CHATTING_TEXT_SIZE),
	_my_color(),
	_my_name(),
	_msg_list(),
	_enter_done(true)
{
	_chatting_text.drawCaret(true);
	_chatting_text.setPosition(
		static_cast<float>(JIYBA),
		static_cast<float>(ZIN) - static_cast<float>(CHATTING_TEXT_SIZE));

	_chatting_text.setTextColor(Color(255, 255, 255));
	_chatting_text.setCaretColor(Color(128, 128, 128));
}

ChatBox::~ChatBox()
{
	for(auto it = _msg_list.begin(); it != _msg_list.end(); ++it)
	{
		if(it->text_pair.first) delete it->text_pair.first;
		if(it->text_pair.second) delete it->text_pair.second;
	}
}

void ChatBox::Reset(const player_t & me)
{
	_my_color = me.color;
	_my_name = me.name;
	_enter_done = true;
}

void ChatBox::Update()
{
	// 오래된 메시지는 지웁니다.
	for(auto it = _msg_list.begin(); it != _msg_list.end();)
	{
		if(it->time + 8000 < present_time)
		{
			if(it->text_pair.first) delete it->text_pair.first;
			if(it->text_pair.second) delete it->text_pair.second;
			it = _msg_list.erase(it);
		}
		else break;
	}
}

void ChatBox::AddChatMsg(const Color & color, const wstring & str, const Color & color2, const wstring & str2)
{
	Text * new_text = new Text(str + " : ", normal_font, CHATTING_TEXT_SIZE);
	new_text->setColor(color);

	Text * new_text2 = new Text(str2, normal_font, CHATTING_TEXT_SIZE);
	new_text2->setColor(color2);
	
	msg_t msg;
	msg.time = present_time;
	msg.text_pair.first = new_text;
	msg.text_pair.second = new_text2;

	_msg_list.push_back(msg);
}

void ChatBox::AddAlarmMsg(const wstring & str)
{
	AddChatMsg(	Color(255, 255, 255), L"알림",
				Color(200, 200, 200), str);
}

void ChatBox::AddInfoMsg(const wstring & info_str)
{
	Text * new_text = new Text(info_str, normal_font, CHATTING_TEXT_SIZE);
	new_text->setColor(Color(190, 120, 100));
	
	msg_t msg;
	msg.time = present_time;
	msg.text_pair.first = nullptr;
	msg.text_pair.second = new_text;

	_msg_list.push_back(msg);
}

void ChatBox::AlertEnterDone()
{
	_chatting_text.clear();
	_enter_done = true;
}

void ChatBox::draw(RenderTarget& target, RenderStates states) const
{
	float x = static_cast<float>(JIYBA);
	float y = static_cast<float>(ZIN) - 2 * CHATTING_TEXT_SIZE;

	// 메시지를 화면에 그립니다.
	for(auto it = _msg_list.rbegin(); it != _msg_list.rend(); ++it)
	{
		Text * first = it->text_pair.first;
		Text * second = it->text_pair.second;

		if(y < 0.f) break;

		float height_max = CHATTING_TEXT_SIZE;
		if(first != nullptr && height_max < first->getLocalBounds().height)
			height_max = first->getLocalBounds().height;
		if(height_max < second->getLocalBounds().height)
			height_max = second->getLocalBounds().height;

		y -= height_max;

		if(first)
		{
			first->setPosition(x, y);
			target.draw(*first);

			second->setPosition(x + first->getLocalBounds().width + 5.f , y);
		}
		else second->setPosition(x, y);
		
		target.draw(*second);
	}

	if(!_enter_done) target.draw(_chatting_text);
}

bool ChatBox::HandleKeyPressed(Event::KeyEvent key)
{
	if(key.code == Keyboard::Return)
	{
		if(!_chatting_text.getString().isEmpty())
		{
			wstring & wstr = _chatting_text.getString().toWideString();
			AddChatMsg(	_my_color, _my_name,
						Color(220, 245, 245), wstr);
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(CL_TO_SV_CHAT);
			send_packet << wstr;
			SafeSend(send_packet);
			_chatting_text.clear();
		}
		_enter_done = true;
		return true;
	}
	else return _chatting_text.handleKeyPress(key);
}

bool ChatBox::HandleIMECompMsg(wchar_t key, bool done)
{
	return _chatting_text.handleIMEUpdated(key, done);
}