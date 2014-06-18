/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	chat_box.h

	custom SFML ChatBox class의 header file 입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "player.h"

class ChatBox : public Drawable
{
public:
			 ChatBox();
			~ChatBox();

	void	 Reset(const player_t & me);

	void	 Update();

	void	 AddChatMsg(const Color & color, const wstring & str, const Color & color2, const wstring & str2);
	void	 AddAlarmMsg(const wstring & str);
	void	 AddInfoMsg(const wstring & info_str);

	bool	 HandleTextEntered(wchar_t key) { return _chatting_text.handleEnteredKey(key); }
	bool	 HandleKeyPressed(Event::KeyEvent key);
	bool	 HandleIMECompMsg(wchar_t key, bool done);

	void	 ClearEditText() { _chatting_text.clear(); }

	// 입력 mode를 킵니다.
	void	 BeginToEnter() { _enter_done = false; }
	// 강제로 입력을 끝마칩니다.
	void	 AlertEnterDone();

	// 입력했는지 확인합니다.
	bool	 CheckEnterDone() { return _enter_done; }

private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

private:
	struct msg_t
	{
		int						time;
		pair<Text *, Text *>	text_pair;
	};

private:
	EditText					_chatting_text;
	Color						_my_color;
	wstring						_my_name;
	list<msg_t>					_msg_list;
	bool						_enter_done;
};