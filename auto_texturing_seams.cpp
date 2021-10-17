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



// Bit pattern: 0b0000NESW

uchar tiles_to[] = {
	0b00000001,
	0b00000000,
	0b00001001,
	0b00001001,
	0b00001001,
	0b00001100,
	0b00001000,
	0b00001000,
	0b00000000,
	0b00001001,
};

uchar tiles_from[] = {
	0b00000100,
	0b00000110,
	0b00000000,
	0b00000110,
	0b00000010,
	0b00000010,
	0b00000110,
	0b00000011,
	0b00000110,
	0b00000000,
};

inline void Shape_flip_x(uchar& s)
{
	s = s & 0b11111010 | ((s & 0b00000100) >> 2) | ((s & 0b00000001) << 2);
}

inline void Shape_flip_z(uchar& s)
{
	s = s & 0b11110101 | ((s & 0b00001000) >> 2) | ((s & 0b00000010) << 2);
}

inline void Shape_rotate(uchar& s)
{
	//s = ((s & 0b00000001) << 3) | (s >> 1); // this is wrong side, should rotate counter clockwise as it's the shape around the tile, not the tile
	s = ((s & 0b00001000) >> 3) | (s << 1) & 0b00001111;
}

int Fit_tile(uchar shape, uchar* shapes)
{
	for (int i = 0; i < 10; ++i) {
		if (shape == shapes[i]) {
			return i;
		}
	}
	return -1;
}

int Fit_tile(uchar shape_to, uchar shape_from)
{
	for (int i = 0; i < 10; ++i) {
		if (shape_to == tiles_to[i] && shape_from == tiles_from[i]) {
			return i;
		}
	}
	return -1;
}

int Fit_tile(
	uchar shape_to,
	uchar shape_from,
	bool match_from,
	int& rotation
) {
	int tid;
	for (int r = 0; r < 4; ++r) {
		if (match_from) {
			tid = Fit_tile(shape_to, shape_from);
		} else {
			tid = Fit_tile(shape_to, tiles_to);
		}
		if (tid != -1) {
			rotation = r;
			return tid;
		}
		Shape_rotate(shape_to);
		Shape_rotate(shape_from);
	}
}

int Fit_tile(
	uchar shape_to,
	uchar shape_from,
	bool match_from,
	int& rotation,
	bool& flipx,
	bool& flipz
) {
	int tid;

	// Try all rotations.
	tid = Fit_tile(shape_to, shape_from, match_from, rotation);
	if (tid != -1) {
		return tid;
	}

	// Flip X.
	flipx = true;
	Shape_flip_x(shape_to);
	Shape_flip_x(shape_from);

	// Try all rotations.
	tid = Fit_tile(shape_to, shape_from, match_from, rotation);
	if (tid != -1) {
		return tid;
	}

	// Flip Z.
	flipz = true;
	Shape_flip_z(shape_to);
	Shape_flip_z(shape_from);

	// Try all rotations.
	tid = Fit_tile(shape_to, shape_from, match_from, rotation);
	if (tid != -1) {
		return tid;
	}

	// Flip X back.
	flipx = false;
	Shape_flip_x(shape_to);
	Shape_flip_x(shape_from);

	// Try all rotations.
	tid = Fit_tile(shape_to, shape_from, match_from, rotation);
	return tid;

	// No match found - give up.
}



bool Fill_islands(TextureLayerTransition* transition)
{
	int i;
	bool ok = false;
	bool oook = true;

	while (!ok) {
		ok = true;
		for (int tz = 1; tz < mapwidth - 1; ++tz) {
			for (int tx = 1; tx < mapheight - 1; ++tx) {
				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);

				char* group_this;
				char* group_other;
				if (Tile_in_group(tile, transition->from_group)) {
					group_this = transition->from_group;
					group_other = transition->to_group;
				} else if (Tile_in_group(tile, transition->to_group)) {
					group_this = transition->to_group;
					group_other = transition->from_group;
				} else {
					continue;
				}

				auto countmatch = 0;

				for (int ntz = -1; ntz <= 1; ++ntz) {
					for (int ntx = -1; ntx <= 1; ++ntx) {

						if (ntx != 0 && ntz != 0) {
							continue;
						}

						auto tile2 = Get_tile(tile, ntx, ntz);
						/*if (!tile2) {
							continue;
						}*/

						countmatch += Tile_in_group(tile2, group_other);
					}
				}

				if (countmatch > 2) {
					Set_current_texture_by_name(group_other);
					ChangeTileTexture(tile, Get_curtex());
					tile->rot = rand() % 4;
					tile->xflip = rand() % 2;
					tile->zflip = rand() % 2;

					ok = false;
				}

			} // tiles
		}

		if (!ok) {
			oook = false;
		}
	}

	return oook;
}

bool Match_tiles1(TextureLayerTransition* transition)
{
	int i;
	bool ok = false;
	bool oook = true;
	int counter = 0;

	int rotation = 0;
	bool flipx = false;
	bool flipz = false;

	while (!ok) {
		ok = true;
		bool okfrom;
		bool okto;

		for (int tz = 1; tz < mapwidth - 1; ++tz) {
			for (int tx = 1; tx < mapheight - 1; ++tx) {

				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);
				if (Tile_in_group(tile, transition->from_group)) {

					// Check if is on the border.
					okfrom = false;
					okto = false;
					for (int ntz = -1; ntz <= 1; ++ntz) {
						for (int ntx = -1; ntx <= 1; ++ntx) {

							if (ntx != 0 && ntz != 0) {
								continue;
							}

							auto* tile2 = Get_tile(tile, ntx, ntz);

							auto isfrom = Tile_in_group(tile2, transition->from_group);
							auto isto = Tile_in_group(tile2, transition->to_group);
							okfrom |= isfrom;
							okto |= isto;

							if (!isfrom && !isto) {
								goto next_tile;
							}
						}
					}
					if (!okfrom || !okto) {
						goto next_tile;
					}

					// Do the fitting.
					uchar shape_to = 0;
					uchar shape_from = 0;

					shape_to |= Tile_in_group(Get_N_tile(tile), transition->to_group) << 3;
					shape_to |= Tile_in_group(Get_E_tile(tile), transition->to_group) << 2;
					shape_to |= Tile_in_group(Get_S_tile(tile), transition->to_group) << 1;
					shape_to |= Tile_in_group(Get_W_tile(tile), transition->to_group) << 0;

					int tid = Fit_tile(
						shape_to,
						shape_from,
						false,
						rotation,
						flipx,
						flipz
					);

					if (tid == -1) {
						continue;
					}

					auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
					change->tile_id = tid;
					change->rotation = rotation;
					change->flipx = flipx;
					change->flipz = flipz;
					ok = false;

				} // tile in group
			next_tile:
				ok = ok;
			}
		}

		Apply_change_buffer(transition->transition_group);

		if (!ok) {
			oook = false;
		}

		if (++counter >= fix_seams_iterations) {
			break;
		}
	}

	return oook;
}

bool Match_tiles2(TextureLayerTransition* transition)
{
	int i;
	bool ok = false;
	bool oook = true;
	int counter = 0;

	int rotation = 0;
	bool flipx = false;
	bool flipz = false;

	while (!ok) {
		ok = true;

		for (int tz = 1; tz < mapwidth - 1; ++tz) {
			for (int tx = 1; tx < mapheight - 1; ++tx) {

				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);
				if (Tile_in_group(tile, transition->transition_group)) {

					uchar shape_to = 0;
					uchar shape_from = 0;

					shape_to |= Tile_in_group(Get_N_tile(tile), transition->to_group) << 3;
					shape_to |= Tile_in_group(Get_E_tile(tile), transition->to_group) << 2;
					shape_to |= Tile_in_group(Get_S_tile(tile), transition->to_group) << 1;
					shape_to |= Tile_in_group(Get_W_tile(tile), transition->to_group) << 0;
					shape_from |= Tile_in_group(Get_N_tile(tile), transition->from_group) << 3;
					shape_from |= Tile_in_group(Get_E_tile(tile), transition->from_group) << 2;
					shape_from |= Tile_in_group(Get_S_tile(tile), transition->from_group) << 1;
					shape_from |= Tile_in_group(Get_W_tile(tile), transition->from_group) << 0;

					int tid = Fit_tile(
						shape_to,
						shape_from,
						true,
						rotation,
						flipx,
						flipz
					);

					if (tid == -1) {
						continue;
					}

					auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
					change->tile_id = tid;
					change->rotation = rotation;
					change->flipx = flipx;
					change->flipz = flipz;
					ok = false;

				} // tile in group
			}
		}

		Apply_change_buffer(transition->transition_group);

		if (!ok) {
			oook = false;
		}

		if (++counter >= fix_seams_iterations) {
			break;
		}
	}

	return oook;
}


// Use after finishing all main layers to place texture transistions.
void Texture_fix_seams()
{
	int i;
	bool ok = false;
	int counter = 0;

	Texture_read_layer_files();
	Create_change_buffer();

	// Loop over all transition layers.
	// 
	// First pass places transition tiles based only on "to" tiles,
	// the second one can disambiguate using the transition tiles
	// or remaining "from" tiles.
	printf("fix seams begin\n");

	while (!ok) {
		for (int t = 0; t < transitions.len; ++t)
		{
			auto* transition = &(transitions[t]);

			Fill_islands(transition);
			Match_tiles1(transition);
			Match_tiles2(transition);
		}

		if (++counter >= fix_seams_iterations) {
			printf("fix seams max iterations reached\n");
			break;
		}
	}
	printf("fix seams end\n");

	Free_change_buffer();
	Texture_cleanup();

	// Loop over all tiles - match corners - ID 8, 9 - needed?
}

// todo - match on transition tiles instead
// todo - match on all things ?
// todo - if can't find both shapes then match on tex only
// todo - fix mismatchy tiles - maybe another set for fixes


