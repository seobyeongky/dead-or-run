/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	intro_scene.h

	IntroScene 클래스의 헤더 파일입니다.
=======================================================*/

#pragma once

#include "scene.h"

#include "object.h"

class IntroScene : public Scene {
//	맨 처음 로고를 보여주는 scene입니다.

public:
	IntroScene(scene_id_t id);
	~IntroScene();

	void		Init();

	bool		HandleKeyPressed(Event::KeyEvent key);

	void		Go();

private:
	bool		_over;
	Sprite		_ooparts_logo;
	Sprite		_title_logo;
	Sprite *	_cur_logo;
	Sprite		_sfml_logo;
	Text		_sfml_prefix_text;
	Sprite		_bg;

	enum state_t
	{
		OOPARTS_LOGO_STATE,
		TITLE_LOGO_STATE
	}			_state;

	int			_anim_count;
};