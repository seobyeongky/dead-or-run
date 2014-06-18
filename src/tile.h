/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	tile.h

	tile
=======================================================*/

#pragma once

#include <cassert>

enum tile_t
{
	DIRT_TILE,	// 기본값
	CACTUS_TILE,
	WATER_TILE
};

#define TILE_WIDTH		35
#define TILE_HEIGHT		35

// 한 행에 타일이 몇개 있는지
#define NR_TILE_IN_ROW	16

struct tile_row_t
{
/*	tile 행*/
	tile_t tiles[NR_TILE_IN_ROW];

	// dirt tile로 초기화
	tile_row_t();
};

inline float GetXFromTileUnit(int tile_unit_x)
{
	assert(0 <= tile_unit_x || tile_unit_x < NR_TILE_IN_ROW);	

	return static_cast<float>((tile_unit_x - NR_TILE_IN_ROW / 2) * TILE_WIDTH);
}

inline float GetYFromTileUnit(int tile_unit_y)
{
	return -static_cast<float>((tile_unit_y + 1) * TILE_HEIGHT);
}