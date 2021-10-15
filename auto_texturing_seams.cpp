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

bool Make_transition_tile(
	MapTile* tile,
	TextureLayerTransition* transition,
	bool match_from,
	bool fill_islands = false
) {
	int rotation = 0;
	bool flipx = false;
	bool flipz = false;


	// Need a pre-pass to find out if this is a border at all.
	// Should work only with clear neighbourhood.
	// More complex situations currently need manual resolution.

	// Pre-pass needs a 3x3 box, other checks just + shape.

	int i;
	MapTile* ttile;


	// Fill islands pre-pass.
	if (fill_islands) {
		auto countmatch = 0;

		int x = tile->x;
		int z = tile->z - 1;
		i = z * mapwidth + x;
		countmatch += Tile_in_group(&(maptiles[i]), transition->to_group);
		x = tile->x + 1;
		z = tile->z;
		i = z * mapwidth + x;
		countmatch += Tile_in_group(&(maptiles[i]), transition->to_group);
		x = tile->x;
		z = tile->z + 1;
		i = z * mapwidth + x;
		countmatch += Tile_in_group(&(maptiles[i]), transition->to_group);
		x = tile->x - 1;
		z = tile->z;
		i = z * mapwidth + x;
		countmatch += Tile_in_group(&(maptiles[i]), transition->to_group);

		if (countmatch > 2) {
			char* name = transition->to_group;
			/*if (strcmp(transition->match_group, transition->from_group) == 0) {
				name = transition->to_group;
			} else {
				name = transition->to_group;
			}*/

			Set_current_texture_by_name(name);
			ChangeTileTexture(tile, Get_curtex());
			tile->rot = rand() % 4;
			tile->xflip = rand() % 2;
			tile->zflip = rand() % 2;

			return true;
		}
		return false;
	}

	//printf("pre\n");

	if (!match_from) { // No need for second pass as it's working only on transition tiles.
		auto okfrom = false;
		auto okto = false;
		for (int x = tile->x - 1; x < tile->x + 2; ++x) {
			for (int z = tile->z - 1; z < tile->z + 2; ++z) {
				if (z == tile->z && x == tile->x) {
					continue;
				}

				i = z * mapwidth + x;
				ttile = &(maptiles[i]);

				auto isfrom = Tile_in_group(ttile, transition->from_group);
				auto isto = Tile_in_group(ttile, transition->to_group);
				if (!isfrom && !isto) {
					return false;
				}
				okfrom |= isfrom;
				okto |= isto;
			}
		}
		if (!(okfrom && okto)) {
			return false;
		}
	}

	//printf("main\n");



	// Build shapes.

	uchar shape_to = 0;
	uchar shape_from = 0;

	int x = tile->x;
	int z = tile->z - 1;
	i = z * mapwidth + x;
	shape_to |= Tile_in_group(&(maptiles[i]), transition->to_group) << 3;
	x = tile->x + 1;
	z = tile->z;
	i = z * mapwidth + x;
	shape_to |= Tile_in_group(&(maptiles[i]), transition->to_group) << 2;
	x = tile->x;
	z = tile->z + 1;
	i = z * mapwidth + x;
	shape_to |= Tile_in_group(&(maptiles[i]), transition->to_group) << 1;
	x = tile->x - 1;
	z = tile->z;
	i = z * mapwidth + x;
	shape_to |= Tile_in_group(&(maptiles[i]), transition->to_group) << 0;

	if (match_from) {
		x = tile->x;
		z = tile->z - 1;
		i = z * mapwidth + x;
		shape_from |= Tile_in_group(&(maptiles[i]), transition->from_group) << 3;
		x = tile->x + 1;
		z = tile->z;
		i = z * mapwidth + x;
		shape_from |= Tile_in_group(&(maptiles[i]), transition->from_group) << 2;
		x = tile->x;
		z = tile->z + 1;
		i = z * mapwidth + x;
		shape_from |= Tile_in_group(&(maptiles[i]), transition->from_group) << 1;
		x = tile->x - 1;
		z = tile->z;
		i = z * mapwidth + x;
		shape_from |= Tile_in_group(&(maptiles[i]), transition->from_group) << 0;
	}

	//printf("%d %d\n", shape_to, shape_from);

	// Fit tile.

	int tid = Fit_tile(
		shape_to,
		shape_from,
		match_from,
		rotation,
		flipx,
		flipz
	);

	if (tid == -1) {
		return false;
	}

	auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
	change->tile_id = tid;
	change->rotation = rotation;
	change->flipx = flipx;
	change->flipz = flipz;
	return true;
}



// Use after finishing all main layers to place texture transistions.
void Texture_fix_seams()
{
	int i;

	Texture_read_layer_files();
	Create_change_buffer();

	changebuf = (TileTexChange*)malloc(sizeof(TileTexChange) * bufsize);
	for (int i = 0; i < bufsize; ++i) {
		changebuf[i].tile_id = -1;
	}

	// Loop over all transition layers.
	// 
	// First pass places transition tiles based only on "to" tiles,
	// the second one can disambiguate using the transition tiles
	// or remaining "from" tiles.
	printf("fix seams begin\n");
	for (int t = 0; t < transitions.len; ++t)
	{
		auto* transition = &(transitions[t]);

		// Loop over all tiles - fill islands.
		printf("fix seams 1\n");
		bool changed = true;
		while (changed) {
			changed = false;
			for (int tz = 1; tz < mapwidth - 1; ++tz) {
				for (int tx = 1; tx < mapheight - 1; ++tx) {
					i = tz * mapwidth + tx;
					auto* tile = &(maptiles[i]);
					if (Tile_in_group(tile, transition->from_group)) {
						changed |= Make_transition_tile(tile, transition, false, true);
					}
				}
			}
		}

		// Loop over all tiles - match "to" tiles only.
		printf("fix seams 2\n");
		for (int tz = 1; tz < mapwidth - 1; ++tz) {
			for (int tx = 1; tx < mapheight - 1; ++tx) {
				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);
				if (Tile_in_group(tile, transition->from_group)) {
					Make_transition_tile(tile, transition, false);
				}
			}
		}
		Apply_change_buffer(transition->transition_group);

		// Loop over all tiles - match "from" tiles too.
		printf("fix seams 3\n");
		for (int tz = 1; tz < mapwidth - 1; ++tz) {
			for (int tx = 1; tx < mapheight - 1; ++tx) {
				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);
				if (Tile_in_group(tile, transition->transition_group)) {
					Make_transition_tile(tile, transition, true);
				}
			}
		}
		Apply_change_buffer(transition->transition_group);

		// Loop over all tiles - match "from" tiles too.
		printf("fix seams 4\n");
		for (int tz = 1; tz < mapwidth - 1; ++tz) {
			for (int tx = 1; tx < mapheight - 1; ++tx) {
				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);
				if (Tile_in_group(tile, transition->transition_group)) {
					Make_transition_tile(tile, transition, true);
				}
			}
		}
		Apply_change_buffer(transition->transition_group);

		printf("fix seams 5\n");

		// No need to match transition tiles pass.
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


