#include "tilemap.h"
#include "res.h"
#include "util.h"

TileMap::TileMap(size_t nr_max_row)
	: _nr_max_row(nr_max_row),
	_vertices(),
	_cur_offset(0U),
	_nr_row(0U),
	_tileset(*texture_map[L"tileset.png"]),
	_dirt_tile_base_vec(),
	_water_tile_base_vec()
{
	_vertices.setPrimitiveType(Quads);

	Sprite * dirt_tile = sprite_map[L"dirt-tile"];
	_dirt_tile_base_vec =
		Vector2f(	static_cast<float>(dirt_tile->getTextureRect().left),
					static_cast<float>(dirt_tile->getTextureRect().top) );

	Sprite * water_tile = sprite_map[L"water-tile"];
	_water_tile_base_vec =
		Vector2f(	static_cast<float>(water_tile->getTextureRect().left),
					static_cast<float>(water_tile->getTextureRect().top) );
}

void TileMap::Reset()
{
	_vertices.clear();
	_cur_offset = 0U;
	_nr_row = 0U;
}

void TileMap::Add(const tile_row_t & tile_row)
{
	if(_nr_row < _nr_max_row)
	{
		_vertices.resize((_nr_row + 1) * NR_TILE_IN_ROW * 4);
	}

	float y_pos = GetCurY();
	
	for(int i = 0; i < NR_TILE_IN_ROW; ++i)
	{
		Vertex * v = &_vertices[_cur_offset + 4 * i];
		
		float x_pos = static_cast<float>((i - NR_TILE_IN_ROW / 2) * TILE_WIDTH);
		
		v[0].position = Vector2f(x_pos, y_pos - 0.1f);
		v[1].position = Vector2f(x_pos + TILE_WIDTH, y_pos - 0.1f);
		v[2].position = Vector2f(x_pos + TILE_WIDTH, y_pos + TILE_HEIGHT + 0.1f);
		v[3].position = Vector2f(x_pos, y_pos + TILE_HEIGHT + 0.1f);

		switch(tile_row.tiles[i])
		{
		case DIRT_TILE:
		case CACTUS_TILE:
			{				
				int index = rand() % 5;
				v[0].texCoords.x = _dirt_tile_base_vec.x + TILE_WIDTH * index;
				v[0].texCoords.y = _dirt_tile_base_vec.y;
				v[1].texCoords.x = _dirt_tile_base_vec.x + TILE_WIDTH * index + TILE_WIDTH;
				v[1].texCoords.y = _dirt_tile_base_vec.y;
				v[2].texCoords.x = _dirt_tile_base_vec.x + TILE_WIDTH * index + TILE_WIDTH;
				v[2].texCoords.y = _dirt_tile_base_vec.y + TILE_HEIGHT;
				v[3].texCoords.x = _dirt_tile_base_vec.x + TILE_WIDTH * index;
				v[3].texCoords.y = _dirt_tile_base_vec.y + TILE_HEIGHT;
			}
			break;

		case WATER_TILE:
			v[0].texCoords = _water_tile_base_vec;
			v[1].texCoords = _water_tile_base_vec + Vector2f(static_cast<float>(TILE_WIDTH), 0);
			v[2].texCoords = _water_tile_base_vec + Vector2f(static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT));
			v[3].texCoords = _water_tile_base_vec + Vector2f(0, static_cast<float>(TILE_HEIGHT));
			break;

		default:
			ErrorMsg(L"TileMap : Add : 알 수 없는 tile type(%d)", tile_row.tiles[i]);
		}
	}

	++_nr_row;

	_cur_offset += NR_TILE_IN_ROW * 4;
	if(_cur_offset >= _nr_max_row * NR_TILE_IN_ROW * 4)
		_cur_offset = 0U;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &_tileset;

    // draw the vertex array
    target.draw(_vertices, states);
}
