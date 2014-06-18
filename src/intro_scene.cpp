#include "intro_scene.h"
#include "res.h"
#include "util.h"

IntroScene::IntroScene(scene_id_t id)
	: Scene(id),
	_over(false),
	_ooparts_logo(*sprite_map[L"ooparts"]),
	_title_logo(*sprite_map[L"title"]),
	_cur_logo(nullptr),
	_sfml_logo(*sprite_map[L"sfml-logo"]),
	_sfml_prefix_text(L"powered by", normal_font, 30U),
	_bg(*sprite_map[L"pyramid"]),
	_state(OOPARTS_LOGO_STATE),
	_anim_count(0)
{
	SetMiddleOfLocal(&_ooparts_logo);
	SetMiddleOfScreen(&_ooparts_logo);
	SetMiddleOfLocal(&_title_logo);
	SetMiddleOfScreen(&_title_logo);
	_cur_logo = &_ooparts_logo;
	_sfml_logo.setScale(0.5f, 0.5f);
	_sfml_logo.setPosition(470.f, 490.f);
	_sfml_prefix_text.setPosition(300.f, 500.f);
	FitScaleToScreen(&_bg);
}

IntroScene::~IntroScene()
{
}

void IntroScene::Init()
{	
	_state = OOPARTS_LOGO_STATE;
	_anim_count = 0;
}

bool IntroScene::HandleKeyPressed(Event::KeyEvent key)
{
	if(_state == OOPARTS_LOGO_STATE) { _state = TITLE_LOGO_STATE; _cur_logo = &_title_logo; }
	else if(_state == TITLE_LOGO_STATE) SetNextScene(STARTING_SCENE);
	else ErrorMsg(L"IntroScene : 알 수 없는 상태");

	return true;
}

void IntroScene::Go()
{
	if(_anim_count > 100) _anim_count = 0;

	if(_anim_count < 50)
		_cur_logo->setScale(1.f + 0.01f * _anim_count, 1.f + 0.01f * _anim_count);
	else
		_cur_logo->setScale(2.f - 0.01f * _anim_count, 2.f - 0.01f * _anim_count);

	++_anim_count;

	window.draw(_bg);
	window.draw(_sfml_logo);
	window.draw(_sfml_prefix_text);
	window.draw(*_cur_logo);
}