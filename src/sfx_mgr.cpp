#include "sfx_mgr.h"

using namespace audiere;

#define REGISTER(x, y, z, q)		{_fileNames[x] = y; \
								_extNames[x] = z; \
								_fileNumbers[x] = q;}

opzSFXMgr::opzSFXMgr(void)
{
	REGISTER(SFX_BAYABA, "bayaba", "wav", 1);
	REGISTER(SFX_EAT, "eat", "wav", 1);
	REGISTER(SFX_MALE_HIT, "male_hit", "ogg", 5);
	REGISTER(SFX_MALE_YELL, "male_yell", "ogg", 1);
	REGISTER(SFX_MALE_SCREAM, "male_scream", "wav", 1);
	REGISTER(SFX_FEMALE_HIT, "female_hit", "ogg", 3);
	REGISTER(SFX_FEMALE_YELL, "female_yell", "ogg", 1);
	REGISTER(SFX_GET, "get", "wav", 1);
	REGISTER(SFX_GRUNT, "grunt", "wav", 1);
	REGISTER(SFX_HAIR_SHOT, "hair_shot", "wav", 1);
	REGISTER(SFX_LEAVE_CHAT, "leave_chat", "mp3", 1);
	REGISTER(SFX_READY, "ready", "mp3", 1);
	REGISTER(SFX_LOSE, "lose", "wav", 1);
	REGISTER(SFX_WIN, "win", "wav", 1);
	REGISTER(SFX_WATER, "water", "wav", 1);
	REGISTER(SFX_STONE_THROW, "stone_throw", "wav", 1);
	REGISTER(SFX_STIMPACK, "stimpack", "wav", 1);
	REGISTER(SFX_BACK_ATTACK, "back_attack", "wav", 1);
	REGISTER(SFX_SMOKE, "smog", "wav", 1);
	REGISTER(SFX_CL_SET, "set_move", "wav", 1);
	REGISTER(SFX_ENTER, "enter", "wav", 1);
	REGISTER(SFX_HEADSHOT, "headshot", "wav", 2);
	REGISTER(SFX_LEAP, "leap", "wav", 1);
	REGISTER(SFX_INFO, "info", "wav", 1);
	REGISTER(SFX_FLASH, "flash", "wav", 2);
	REGISTER(SFX_VOICE_RAMPAGE, "rampage", "mp3", 1);
	REGISTER(SFX_VOICE_DOMINATING, "dominating", "mp3", 1);
	REGISTER(SFX_VOICE_UNSTOPPABLE, "unstoppable", "mp3", 1);
	REGISTER(SFX_VOICE_GODLIKE, "godlike", "mp3", 1);
	REGISTER(SFX_BELL, "bell", "wav", 1);
	REGISTER(SFX_GOD_COME, "god_come", "wav", 1);
	REGISTER(SFX_GHOST, "ghost", "wav", 1);
	REGISTER(SFX_PORTAL_OPEN, "portal_open", "ogg", 1);
	REGISTER(SFX_PORTAL_PASSING, "portal_passing", "ogg", 1);
	REGISTER(SFX_GROUND_ATTACK, "ground_attack", "wav", 1);
	REGISTER(SFX_GROUND_ATTACK_VOICE, "ground_attack_voice", "wav", 1);
	REGISTER(SFX_BOOGER_THROW, "booger_throw", "wav", 1);
	REGISTER(SFX_BOOGER_HIT, "booger_hit", "wav", 1);
	REGISTER(SFX_PROVOKATION, "provocation", "ogg", 1);


	_device = OpenDevice();
	
	strcpy_s(_fileFullName, "sound\\");
	_fileNamePtr = _fileFullName + strlen(_fileFullName);
}

opzSFXMgr::~opzSFXMgr(void)
{
}

void opzSFXMgr::Play(sfx_t sfx)
{
	if(!_device || sfx == SFX_NULL) return;
	switch(_fileNumbers[sfx])
	{
	case 1:
		sprintf_s(_fileNamePtr, FULL_NAME_BUFFER_LENGTH - 7, "%s.%s",
			_fileNames[sfx], _extNames[sfx]);
		break;

	default:
		assert(_fileNumbers[sfx] > 1);
		sprintf_s(_fileNamePtr, FULL_NAME_BUFFER_LENGTH - 7, "%s%d.%s",
			_fileNames[sfx], rand() % _fileNumbers[sfx], _extNames[sfx]);
		break;
	}
	OutputStreamPtr stream(
		OpenSound(_device, _fileFullName, false));
	assert(stream != NULL);
	if(stream == NULL)
		return;
	stream->play();
	_streams.push_back(stream);
}

void opzSFXMgr::Update()
{
	for(std::list<OutputStreamPtr>::iterator i = _streams.begin();
		i != _streams.end();)
	{
		if((*i)->isPlaying())
			++i;
		else
		{
			(*i)->stop();
			*i = NULL;
			i = _streams.erase(i);
		}
	}
}