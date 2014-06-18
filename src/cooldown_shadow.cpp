#include "cooldown_shadow.h"
#include "coord.h"
#include "util.h"

/// CONTANTS ///
const float SIN_3_4_PI = 0.70710f;
const float COS_3_4_PI = -SIN_3_4_PI;

const float L = 50.f;
const float HALF_L = L / 2.f;
const float HALF_DIAGONAL = L / 2.f * sqrtf(2.f);

Vertex points[6] = {
	Vertex(Vector2f(HALF_L, HALF_L)),
	Vertex(Vector2f(HALF_L, 0)),
	Vertex(Vector2f(0, 0)),
	Vertex(Vector2f(0, L)),
	Vertex(Vector2f(L, L)),
	Vertex(Vector2f(L, 0)) };

/////////////////


const vector<Vertex> CooldownShadow::frame = 
	vector<Vertex>(points, points + sizeof(points) / sizeof(points[0]));

CooldownShadow::CooldownShadow(const Color & color)
	: _color(color), _vertices(PrimitiveType::TrianglesFan)
{
}

void CooldownShadow::SetRatio(float ratio)
{
	if(ratio < 0.f) return;
	float angle = ratio * 2.f * PI;
	
	enum {
		LEFT, TOP, RIGHT, BOTTOM
	} dir;

	if (GetAsyncKeyState(VK_RETURN))
	{
		angle = angle;
	}

	size_t n = 0;
	if (angle < PI_4)
	{
		n = 6;
		dir = TOP;
	}
	else if (angle < PI_4 + PI_2)
	{
		n = 5;
		dir = RIGHT;
	}
	else if (angle < PI_4 + PI_2 + PI_2)
	{
		n = 4;
		dir = BOTTOM;
	}
	else if (angle < PI_4 + PI_2 + PI_2 + PI_2)
	{
		n = 3;
		dir = LEFT;
	}
	else if (angle < 2 * PI)
	{
		n = 2;
		dir = TOP;
	}
	else return;

	// n basic points, and 1 line-collide point
	_vertices.resize(n + 1);

	// first, copy basic points
	for (size_t i = 0; i < n; ++i)
	{
		_vertices[i].position = frame[i].position;
		_vertices[i].color = _color;
	}

	// and add a collide point on top of VertexArray
	Vector2f cpos;
	switch (dir)
	{
		case LEFT:
		{
			float dangle = angle - (PI + PI_4);
			float sin_a = sinf(dangle);
			float cos_a = cosf(dangle);
			cpos.x = 0.f;
			cpos.y = L - HALF_DIAGONAL * sin_a / (SIN_3_4_PI * cos_a - COS_3_4_PI * sin_a);
		}
		break;

		case TOP:
		{
			float dangle = angle;
			if (angle > PI) dangle -= 2.f * PI;
			cpos.x = HALF_L + tanf(dangle) * HALF_L;
			cpos.y = 0.f;
		}
		break;

		case RIGHT:
		{
			float dangle = angle - PI_4;
			float sin_a = sinf(dangle);
			float cos_a = cosf(dangle);
			cpos.x = L;
			cpos.y = HALF_DIAGONAL * sin_a / (SIN_3_4_PI * cos_a - COS_3_4_PI * sin_a);
		}
		break;

		case BOTTOM:
		{
			float dangle = angle - PI_2 - PI_4;
			float sin_a = sinf(dangle);
			float cos_a = cosf(dangle);
			cpos.x = L - HALF_DIAGONAL * sin_a / (SIN_3_4_PI * cos_a - COS_3_4_PI * sin_a);
			cpos.y = L;
		}
		break;
	}

	_vertices[n].position = cpos;
	_vertices[n].color = _color;
}

void CooldownShadow::draw(RenderTarget& target, RenderStates states) const
{
	states.texture = nullptr;
	target.draw(_vertices, states);
}