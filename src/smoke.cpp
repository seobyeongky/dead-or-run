#include "smoke.h"
#include "res.h"
#include "speed_def.h"
#include "util.h"

Smoke::Smoke(const pos_t & pos, const vector_t & vcity, bool transparent)
	: _sprite(*sprite_map[L"smoke"]),
	_pos(pos),
	_vcity(vcity),
	_set_time(present_time)
{
	_sprite.setOrigin(98.f, 92.f);
	SetScaleToSize(&_sprite, SMOKE_RADIUS * 2.f, SMOKE_RADIUS * 2.f);
	if(transparent)
	{
		_sprite.setColor(Color(255, 255, 255, 110));
	}
}

void Smoke::Update()
{
	_sprite.setPosition(
		_pos + _vcity * ((present_time - _set_time) / MSPPIXEL_F) );
}

void Smoke::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(_sprite);
}