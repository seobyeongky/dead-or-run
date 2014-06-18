/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	tilemap.h

	custom SFML Tilemap class의 header file 입니다.
=======================================================*/

#pragma once

#include "internal.h"

#include "tile.h"

class TileMap : public Drawable, public Transformable
{
/*	tile을 그려줍니다. */
public:
	TileMap(size_t nr_max_row);

	void	Reset();
	void	SetNrRow(size_t nr_row) { _nr_row = nr_row; }
	void	Add(const tile_row_t & tile_row);

	float	GetCurY() const { return  GetYFromTileUnit(_nr_row); }

private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

private:
	size_t			_nr_max_row;
    VertexArray		_vertices;
	unsigned int	_cur_offset;
	size_t			_nr_row;
    Texture			_tileset;
	Vector2f		_dirt_tile_base_vec;
	Vector2f		_water_tile_base_vec;
};