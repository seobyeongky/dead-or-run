/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	res.h

	�ڿ��� ����� ��� �����Դϴ�.
=======================================================*/

#pragma once

#include "internal.h"
#include "s_map.h"
#include "sfx_mgr.h"

//	file system�� �ִ� �ڿ����� load�մϴ�. (font, image, sound)
bool LoadResources();
void ReleaseResource();

extern RenderWindow						window;
extern Font								normal_font;
extern int								present_time;
extern int								delta_time;
extern SMap<wstring, Texture *>			texture_map;
extern map<wstring, Sprite *>			sprite_map;
extern audiere::AudioDevicePtr			audio_device;
extern audiere::OutputStreamPtr			bg_music;
extern opzSFXMgr *						sfx_mgr;