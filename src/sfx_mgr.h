/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sfx_msg.h

	효과음을 재생시켜줍니다.
=======================================================*/

#pragma once

#include "audiere.h"
#include "internal.h"

#define FULL_NAME_BUFFER_LENGTH	512
#define NAME_BUFFER_LENGTH		64

enum sfx_t
{
	SFX_NULL,
	SFX_BAYABA,
	SFX_EAT,
	SFX_MALE_HIT,
	SFX_MALE_YELL,
	SFX_MALE_SCREAM,
	SFX_FEMALE_HIT,
	SFX_FEMALE_YELL,
	SFX_GET,
	SFX_GRUNT,
	SFX_HAIR_SHOT,
	SFX_LEAVE_CHAT,
	SFX_READY,
	SFX_LOSE,
	SFX_WIN,
	SFX_WATER,
	SFX_STONE_THROW,
	SFX_STIMPACK,
	SFX_BACK_ATTACK,
	SFX_SMOKE,
	SFX_CL_SET,
	SFX_ENTER,
	SFX_HEADSHOT,
	SFX_LEAP,
	SFX_INFO,
	SFX_FLASH,
	SFX_VOICE_RAMPAGE,
	SFX_VOICE_DOMINATING,
	SFX_VOICE_UNSTOPPABLE,
	SFX_VOICE_GODLIKE,
	SFX_BELL,
	SFX_GOD_COME,
	SFX_GHOST,
	SFX_PORTAL_OPEN,
	SFX_PORTAL_PASSING,
	SFX_GROUND_ATTACK,
	SFX_GROUND_ATTACK_VOICE,
	SFX_BOOGER_THROW,
	SFX_BOOGER_HIT,
	SFX_PROVOKATION,
	SFX_NUMBER
};

class opzSFXMgr
{
public:
			 opzSFXMgr(void);
			~opzSFXMgr(void);

	void	 Play(sfx_t sfx);

	void	 Update();		//재생을 다 한 outputstream은 삭제해야 한다.

private:
	LPCSTR	_fileNames[SFX_NUMBER];
	LPCSTR	_extNames[SFX_NUMBER];
	int		_fileNumbers[SFX_NUMBER];

	char	_fileFullName[512];
	LPSTR	_fileNamePtr;

	audiere::AudioDevicePtr _device;
	std::list<audiere::OutputStreamPtr> _streams;
};

