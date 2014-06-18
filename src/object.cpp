#include "object.h"
#include "res.h"
#include "util.h"
#include "runner.h"
#include "speed_def.h"

//////////////// ITEM //////////////////
const Sprite * Item::GetSprite(item_id_t item_id)
{
	switch(item_id)
	{
	case ITEM_STONE: return sprite_map[L"stone"];
	case ITEM_BIG_WONTON: return sprite_map[L"big_wonton"];
	case ITEM_CURRY_RICE:return sprite_map[L"curry_rice"];
	case ITEM_DOUBLE_JAJANG: return sprite_map[L"double_jajang"];
	case ITEM_JAJANG: return sprite_map[L"jajang"];
	case ITEM_JOB_RICE: return sprite_map[L"job_rice"];
	case ITEM_JONG_HO_FOOD:return sprite_map[L"jong_ho_food"];
	case ITEM_RAMYUN: return sprite_map[L"ramyun"];
	case ITEM_RICE: return sprite_map[L"rice"];
	case ITEM_TEOK_RAMYUN: return sprite_map[L"teok_ramyun"];
	case ITEM_WONTON: return sprite_map[L"wonton"];
	default:
		ErrorMsg(L"Item : GetSprite : 알 수 없는 item_id(%d)", item_id);
	}
	return nullptr;
}

sfx_t Item::GetSFX(item_id_t item_id)
{
	switch(item_id)
	{
	case ITEM_STONE: break;
	case ITEM_BIG_WONTON:
	case ITEM_CURRY_RICE:
	case ITEM_DOUBLE_JAJANG:
	case ITEM_JAJANG:
	case ITEM_JOB_RICE:
	case ITEM_JONG_HO_FOOD:
	case ITEM_RAMYUN:
	case ITEM_RICE:
	case ITEM_TEOK_RAMYUN:
	case ITEM_WONTON: return SFX_EAT;
	default:
		ErrorMsg(L"Item : GetSFX : 알 수 없는 item_id(%d)", item_id);
	}
	return SFX_NULL;
}

Item::Item(item_id_t item_id, const pos_t & pos)
	: _sprite(), _pos(pos)
{
	_sprite = *GetSprite(item_id);
	_sprite.setOrigin(25.f, 25.f);
	_sprite.setPosition(pos);
}

////////////////////// STONE /////////////////////
Stone::Stone(const pos_t & pos, const vector_t & vcity)
	: _sprite(*sprite_map[L"stone"]),
	_set_pos(pos), _pos(pos), _vcity(vcity), _set_time(present_time)
{
	_sprite.setOrigin(25.f, 25.f);
	ResetPos(pos);
}

void Stone::SetPos(const pos_t & pos)
{
	ResetPos(pos);
	_set_pos = _pos;
	_set_time = present_time;
}

void Stone::ResetPos(const pos_t & pos)
{
	_pos = pos;
	_sprite.setPosition(_pos);
}

void Stone::CorrectPos()
{
	vector_t pos
		= _set_pos + _vcity * ((present_time - _set_time) / MSPPIXEL_F);
	
	ResetPos(pos);
}

//////////////////////// HAIR ///////////////////////

Hair::Hair()
	: _sprite(*sprite_map[L"hair"]), _owner(nullptr),
	_set_pos(), _pos(),
	_set_time(present_time),
	_first_pos()
{
	_sprite.setOrigin(	_sprite.getLocalBounds().width / 2.f,
						_sprite.getLocalBounds().height / 2.f + 10.f);
}

void Hair::SetFirstPos(const pos_t & fpos)
{
	_first_pos = fpos;
}

void Hair::SetOwner(Runner * owner)
{
	_owner = owner;
}

void Hair::SetPos(const pos_t & pos)
{
	ResetPos(pos);
	_set_pos = _pos;
	_set_time = present_time;
}

float Hair::GetLength() const
{
	if (_owner)
	{
		pos_t owner_pos = _owner->GetPos();
		return GetSize(owner_pos - _pos);
	}
	else
	{
		return GetSize(_first_pos - _pos);
	}
}

void Hair::ResetPos(const pos_t & pos)
{
	_pos = pos;

	pos_t origin;
	if(_owner == nullptr) origin = _first_pos;
	else origin = _owner->GetPos();
	float angle = GetDir(_pos - origin);
	_sprite.setPosition((_pos + origin) / 2.f);
	_sprite.setRotation(ToDegree(angle));
	_sprite.setScale(.7f, GetSize(_pos - origin) / _sprite.getLocalBounds().height);
}

void Hair::CorrectPos()
{
	vector_t pos
		= _set_pos + _vcity * ((present_time - _set_time) / MSPPIXEL_F);
	
	ResetPos(pos);
}



/////////////// PORTAL /////////////////

Portal::Portal(const pos_t & pos, float dir)
	: _rects(),
	_sprite(*sprite_map[L"portal"]),
	_accum_time(0),
	_index(0),
	_pos(pos)
{
	auto trect = _sprite.getTextureRect();
	auto base = Vector2i(trect.left, trect.top);

	int n = 7;
	for(int i = 0; i < n; ++i)
	{
		_rects.push_back(base + IntRect(i * 135, 0, 135, 180));
	}
	
	_sprite.setPosition(_pos);
	_sprite.setTextureRect(_rects[0]);
	_sprite.setOrigin(72.5f, 100.f);

	_sprite.setRotation(ToDegree(dir));
	_sprite.setScale(0.7f, 0.7f);
}

void Portal::Draw(sf::RenderTarget& target)
{
	Update();

	target.draw(_sprite);
}

void Portal::CorrectPos()
{
}

void Portal::Update()
{
	_accum_time += delta_time;
	while (_accum_time > 100)
	{
		_accum_time -= 100;

		_index++;
		if (_index >= _rects.size()) _index = 0;

		_sprite.setTextureRect(_rects[_index]);
	}
}


////////////////////// MagicBooger /////////////////////
MagicBooger::MagicBooger(const pos_t & pos, const vector_t & vcity)
	: _sprite(*sprite_map[L"booger"]),
	_set_pos(pos), _pos(pos), _vcity(vcity), _set_time(present_time)
{
	_sprite.setOrigin(15.f, 30.f);
	
	_sprite.setRotation(ToDegree(GetDir(vcity)));
	
	ResetPos(pos);
}

void MagicBooger::SetPos(const pos_t & pos)
{
	ResetPos(pos);
	_set_pos = _pos;
	_set_time = present_time;
}

void MagicBooger::ResetPos(const pos_t & pos)
{
	_pos = pos;
	_sprite.setPosition(_pos);
}

void MagicBooger::CorrectPos()
{
	vector_t pos
		= _set_pos + _vcity * ((present_time - _set_time) / MSPPIXEL_F);
	
	ResetPos(pos);
}


///////////////////// GROUDN_ATTACK ////////////////

GroundAttack::GroundAttack(const pos_t & pos)
	: _pos(pos), _begin_time(present_time),
	_sprite(*sprite_map[L"ground-attack"])
{
	_sprite.setOrigin(100.f, 70.f);
	_sprite.setPosition(_pos);
}

void GroundAttack::Draw(RenderTarget & target)
{
	if (present_time > _begin_time + DURATION / 2)
	{
		int a = present_time - _begin_time - DURATION / 2;
		int b = DURATION / 2;
		_sprite.setColor(Color(255, 255, 255, 255 * a / b));
	}

	target.draw(_sprite);
}

void GroundAttack::CorrectPos()
{
}