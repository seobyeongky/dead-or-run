#include "sandstorm.h"
#include "coord.h"
#include "res.h"
#include "speed_def.h"
#include "util.h"

Sandstorm::Sandstorm(size_t nr_dust, Camera * camera_ptr) :
	_sprite(*sprite_map[L"sandstorm"]),
    _dust_vec(nr_dust),
	_vertices(Points, nr_dust),
    _max_lifetime(5000),
	_curvature_accel(0.3f),
	_camera_ptr(camera_ptr),
	_top(0.f)
{
	_sprite.setColor(Color(255, 255, 255, 150));
	CalculateSprite();
}

void Sandstorm::Update(float min_y, float storm_dist)
{
	CalculateSprite();
	CalculateTop(min_y, storm_dist);
	
	float window_width = static_cast<float>(window.getSize().x);

    for (std::size_t i = 0; i < _dust_vec.size(); ++i)
    {
        dust_t & p = _dust_vec[i];
        p.rest_lifetime -= delta_time;

        if (p.rest_lifetime <= 0) ResetDust(i);

		Vertex & v = _vertices[i];
		v.position.x += p.speed * (delta_time / MSPPIXEL_F);
		v.position.y += (v.position.x - window_width / 2.f) / MSPPIXEL_F * _curvature_accel;
    }
}

void Sandstorm::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(_sprite);

    states.transform *= getTransform();

    states.texture = nullptr;

    target.draw(_vertices, states);
}

void Sandstorm::CalculateSprite()
{
	float h = window.getSize().y - _top;
	_sprite.setPosition(0.f,
		static_cast<float>(window.getSize().y) - h);
	SetScaleToSize(&_sprite,
		static_cast<float>(window.getSize().x),
		h);
}

void Sandstorm::CalculateTop(float min_y, float storm_dist)
{
	float a = _camera_ptr->GetCurY() - min_y;
	float b = _camera_ptr->GetBottomOffset();
	float c = storm_dist;
	float h = b - c + a;
	_top = window.getSize().y - h + 200.f;
}