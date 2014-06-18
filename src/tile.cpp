#include "tile.h"
#include "internal.h"

tile_row_t::tile_row_t()
{
	for(int i = 0; i < NR_TILE_IN_ROW; ++i)
	{
		tiles[i] = DIRT_TILE;
	}
}