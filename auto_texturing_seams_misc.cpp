// wkbre - WK (Battles) recreated game engine
// Copyright (C) 2015-2016 Adrien Geets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "auto_texturing.h"



uchar Get_shape(MapTile* tile, char* group)
{
	uchar s = 0;
	if (tile->z > 0) {
		s |= Tile_in_group(Get_N_tile(tile), group) << 7;
		if (tile->x > 0) {
			s |= Tile_in_group(Get_NW_tile(tile), group) << 0;
		}
		if (tile->x < mapwidth - 1) {
			s |= Tile_in_group(Get_NE_tile(tile), group) << 6;
		}
	}
	if (tile->z < mapheight - 1) {
		s |= Tile_in_group(Get_S_tile(tile), group) << 3;
		if (tile->x > 0) {
			s |= Tile_in_group(Get_SW_tile(tile), group) << 2;
		}
		if (tile->x < mapwidth - 1) {
			s |= Tile_in_group(Get_SE_tile(tile), group) << 4;
		}
	}
	if (tile->x > 0) {
		s |= Tile_in_group(Get_W_tile(tile), group) << 1;
	}
	if (tile->x < mapwidth - 1) {
		s |= Tile_in_group(Get_E_tile(tile), group) << 5;
	}
	return s;
}


void Randomize_tile(TileTexChange* t)
{
	switch (t->tile_id) {
	case 0:
		t->flipx = rand() % 2;
		break;
	case 1:
	case 2:
	case 3:
	case 8:
	case 9:
		if (rand() % 2) {
			t->rotation += 2;
			t->rotation %= 4;
			t->flipx = !t->flipx;
			t->flipz = !t->flipz;
		}
		break;
	case 4:
	case 6:
		if (rand() % 2) {
			t->tile_id += 1;
			t->flipz = !t->flipz;
		}
		break;
	case 5:
	case 7:
		if (rand() % 2) {
			t->tile_id -= 1;
			t->flipz = !t->flipz;
		}
		break;
	}
}
