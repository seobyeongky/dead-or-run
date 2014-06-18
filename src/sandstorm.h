#pragma once

#include "internal.h"
#include "camera.h"
#include "res.h"

class Sandstorm : public Drawable, public Transformable
{
public:

			Sandstorm(size_t nr_dust, Camera * camera_ptr);

    void	Update(float min_y, float storm_dist);

private:
    virtual void draw(RenderTarget& target, RenderStates states) const;

private:
    struct dust_t
    {
		float		speed;
        int			rest_lifetime;
    };

private:
	Sprite			_sprite;
	vector<dust_t>	_dust_vec;
	VertexArray		_vertices;
	int				_max_lifetime;
	float			_curvature_accel;
	Camera *		_camera_ptr;
	float			_top;
	
	void		CalculateSprite();
	inline void ResetDust(size_t index);
	void		CalculateTop(float min_y, float storm_dist);
};

inline void Sandstorm::ResetDust(size_t index)
{
	// 0.f <= rnd < 1.0f
	float rnd = static_cast<float>(rand()) / RAND_MAX;

	dust_t & p = _dust_vec[index];
	p.speed = (rand() % 20) + 70.f;
	p.rest_lifetime = 1000 + (rand() % 3000);

	Vertex & v = _vertices[index];
	v.position.x = 0.f;
	v.position.y = rnd * window.getSize().y + (1.f - rnd) * _top;
	v.color = sf::Color(50, 30, 30);
}