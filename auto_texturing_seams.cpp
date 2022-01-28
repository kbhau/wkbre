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



// Bit pattern: N NE E SE S SW W NW

uchar tiles_to[] = {
	0b00000111,
	0b00000001,
	0b11000111,
	0b10000011,
	0b11000011,
	0b11100001,
	0b00000001,
	0b01000000,
	0b00000001,
	0b11101111,
};

uchar tiles_from[] = {
	0b01110000,
	0b01111100,
	0b00010000,
	0b00111000,
	0b00011000,
	0b00001100,
	0b00011100,
	0b00011100,
	0b11111110,
	0b00010000,
};

inline void Shape_flip_x(uchar& s)
{

	s = (s & 0b10001000)
		| ((s & 0b01000000) >> 6) | ((s & 0b00000001) << 6)
		| ((s & 0b00100000) >> 4) | ((s & 0b00000010) << 4)
		| ((s & 0b00010000) >> 2) | ((s & 0b00000100) << 2);
}

inline void Shape_flip_z(uchar& s)
{
	s = (s & 0b00100010)
		| ((s & 0b10000000) >> 4) | ((s & 0b00001000) << 4)
		| ((s & 0b01000000) >> 2) | ((s & 0b00010000) << 2)
		| ((s & 0b00000100) >> 2) | ((s & 0b00000001) << 2);
}

inline void Shape_rotate(uchar& s)
{
	// this is wrong side, should rotate counter clockwise as it's the shape around the tile, not the tile
	//s = ((s & 0b11111100) >> 2) | ((s & 0b00000011) << 6);
	s = ((s & 0b11000000) >> 6) | ((s & 0b00111111) << 2);
}


int Fit_tile_precise(uchar shape, uchar* shapes)
{
	for (int i = 0; i < 10; ++i) {
		if (shape == shapes[i]) {
			return i;
		}
	}
	return -1;
}

int Fit_tile_precise(uchar shape_to, uchar shape_from)
{
	for (int i = 0; i < 10; ++i) {
		if (shape_to == tiles_to[i]
			&& shape_from == tiles_from[i]) {
			return i;
		}
	}
	return -1;
}

int Fit_tile_precise(
	uchar shape_to,
	uchar shape_from,
	bool match_from,
	int& rotation
)
{
	int tid = -1;
	for (int r = 0; r < 4; ++r) {
		if (match_from) {
			tid = Fit_tile_precise(shape_to, shape_from);
		} else {
			tid = Fit_tile_precise(shape_to, tiles_to);
		}
		if (tid != -1) {
			rotation = r;
			return tid;
		}
		Shape_rotate(shape_to);
		Shape_rotate(shape_from);
	}
	return tid;
}

int Fit_tile(uchar shape, uchar* shapes)
{
	for (int i = 0; i < 10; ++i) {
		if ((shape & 0b10101010) == (shapes[i] & 0b10101010)) {
			return i;
		}
	}
	return -1;
}

int Fit_tile(uchar shape_to, uchar shape_from)
{
	for (int i = 0; i < 10; ++i) {
		if ((shape_to & 0b10101010) == (tiles_to[i] & 0b10101010)
			&& (shape_from & 0b10101010) == (tiles_from[i] & 0b10101010))
		{
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
	int tid = -1;
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
	return tid;
}

int Fit_tile(
	uchar shape_to,
	uchar shape_from,
	bool match_from,
	bool precise,
	int& rotation,
	bool& flipx,
	bool& flipz
) {
	int tid;

	// Try all rotations.
	tid = precise ?
		Fit_tile_precise(shape_to, shape_from, match_from, rotation)
		: Fit_tile(shape_to, shape_from, match_from, rotation);
	if (tid != -1) {
		return tid;
	}

	//return tid;
	// Flip X.
	flipx = true;
	Shape_flip_x(shape_to);
	Shape_flip_x(shape_from);

	// Try all rotations.
	tid = precise ?
		Fit_tile_precise(shape_to, shape_from, match_from, rotation)
		: Fit_tile(shape_to, shape_from, match_from, rotation);
	if (tid != -1) {
		return tid;
	}

	// Flip Z.
	flipz = true;
	Shape_flip_z(shape_to);
	Shape_flip_z(shape_from);

	// Try all rotations.
	tid = precise ?
		Fit_tile_precise(shape_to, shape_from, match_from, rotation)
		: Fit_tile(shape_to, shape_from, match_from, rotation);
	if (tid != -1) {
		return tid;
	}

	// Flip X back.
	flipx = false;
	Shape_flip_x(shape_to);
	Shape_flip_x(shape_from);

	// Try all rotations.
	tid = precise ?
		Fit_tile_precise(shape_to, shape_from, match_from, rotation)
		: Fit_tile(shape_to, shape_from, match_from, rotation);
	if (tid != -1) {
		return tid;
	}

	// No match found - give up.
	flipz = false;
	return tid;
}



double Get_shape_score(
	uchar tile,
	uchar stencil
) {
	double score = 0.0;
	//double score_max = 1.0;
	uchar product = tile & stencil;
	for (int i = 0; i < 8; ++i) {
		if (((1U << i) & product) > 0U) {
			score += 1.0;
		}
		/*if (((1U << i) & stencil) > 0U) {
			score_max += 1.0;
		}*/
	}
	return score;// / score_max;
}

void Get_best_tile(
	uchar shape_to,
	uchar shape_from,
	int& tile,
	double& _score
) {
	double bestscore = -100;
	int besttile = -1;
	double score;
	for (int i = 0; i < 10; ++i) {
		switch (i) {
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			continue;
		}
		score = Get_shape_score(shape_from, tiles_from[i]);
		score += Get_shape_score(shape_to, tiles_to[i]);
		score += Get_shape_score(
			~(shape_from | shape_to),
			~(tiles_from[i] | tiles_to[i])
		);
		/*score -= Get_shape_score(shape_from, tiles_to[i]) * 5.0;
		score -= Get_shape_score(shape_to, tiles_from[i]) * 5.0;*/
		if (score > bestscore) {
			bestscore = score;
			besttile = i;
		}
	}
	_score = bestscore;
	tile = besttile;
}

int Get_best_tile(
	uchar shape_to,
	uchar shape_from,
	int& rotation,
	bool& flipx,
	bool& flipz
) {
	double bestscore = -100;
	int besttile = -1;
	double score;
	int tile;

	int r = 0;
	bool mx = false;
	bool mz = false;

	// No transform.
	for (int r = 0; r < 4; ++r) {
		Get_best_tile(shape_to, shape_from, tile, score);
		if (score > bestscore) {
			bestscore = score;
			besttile = tile;
			rotation = r;
			flipx = mx;
			flipz = mz;
		}
		Shape_rotate(shape_to);
		Shape_rotate(shape_from);
	}

	// X.
	mx = true;
	Shape_flip_x(shape_to);
	Shape_flip_x(shape_from);
	for (int r = 0; r < 4; ++r) {
		Get_best_tile(shape_to, shape_from, tile, score);
		if (score > bestscore) {
			bestscore = score;
			besttile = tile;
			rotation = r;
			flipx = mx;
			flipz = mz;
		}
		Shape_rotate(shape_to);
		Shape_rotate(shape_from);
	}

	// X and Z.
	mz = true;
	Shape_flip_z(shape_to);
	Shape_flip_z(shape_from);
	for (int r = 0; r < 4; ++r) {
		Get_best_tile(shape_to, shape_from, tile, score);
		if (score > bestscore) {
			bestscore = score;
			besttile = tile;
			rotation = r;
			flipx = mx;
			flipz = mz;
		}
		Shape_rotate(shape_to);
		Shape_rotate(shape_from);
	}

	// Z.
	mx = false;
	Shape_flip_x(shape_to);
	Shape_flip_x(shape_from);
	for (int r = 0; r < 4; ++r) {
		Get_best_tile(shape_to, shape_from, tile, score);
		if (score > bestscore) {
			bestscore = score;
			besttile = tile;
			rotation = r;
			flipx = mx;
			flipz = mz;
		}
		Shape_rotate(shape_to);
		Shape_rotate(shape_from);
	}

	return besttile;
}




uchar Get_shape(MapTile* tile, char* group)
{
	uchar s = 0;
	s |= Tile_in_group(Get_N_tile(tile), group) << 7;
	s |= Tile_in_group(Get_NE_tile(tile), group) << 6;
	s |= Tile_in_group(Get_E_tile(tile), group) << 5;
	s |= Tile_in_group(Get_SE_tile(tile), group) << 4;
	s |= Tile_in_group(Get_S_tile(tile), group) << 3;
	s |= Tile_in_group(Get_SW_tile(tile), group) << 2;
	s |= Tile_in_group(Get_W_tile(tile), group) << 1;
	s |= Tile_in_group(Get_NW_tile(tile), group) << 0;
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


// Fills cases where tile about to get changed into transition piece is
// surrounded by other tiles from 3 or 4 sides. No such transition exists.
bool Fill_islands(TextureLayerTransition* transition)
{
	int i;
	int iters = 0;
	bool ok = false;
	bool oook = true;

	char* group_this;
	char* group_other;
	if (transition->match_from) {
		group_this = transition->from_group;
		group_other = transition->to_group;
	} else {
		group_this = transition->to_group;
		group_other = transition->from_group;
	}


	// Try to expand tiles that would disappear.
	for (int ii = 0; ii < fix_seams_iterations; ++ii)
	for (int tz = 1; tz < mapwidth - 1; ++tz)
	for (int tx = 1; tx < mapheight - 1; ++tx)
	{
		i = tz * mapwidth + tx;
		auto* tile = &(maptiles[i]);

		// Process only the matching group.
		if (!Tile_in_group(tile, group_this)) {
			continue;
		}

		// Count bordering sides.
		auto countmatch = 0;
		for (int ntz = -1; ntz <= 1; ++ntz)
		for (int ntx = -1; ntx <= 1; ++ntx)
		{
			// Skip diagonals.
			if (ntx != 0 && ntz != 0) {
				continue;
			}

			auto tile2 = Get_tile(tile, ntx, ntz);

			countmatch += Tile_in_group(tile2, group_other);
		}

		// If above threshold, change neighbour tiles.
		if (countmatch >= fill_islands_min_neighbours) {
			for (int ntz = -1; ntz <= 1; ++ntz)
			for (int ntx = -1; ntx <= 1; ++ntx)
			{
				auto tile2 = Get_tile(tile, ntx, ntz);

				if (Tile_in_group(tile2, group_other)) {
					Set_current_texture_by_name(group_this);
					ChangeTileTexture(tile2, Get_curtex());
					tile2->rot = rand() % 4;
					tile2->xflip = rand() % 2;
					tile2->zflip = rand() % 2;
				}
			}

			ok = false;
		}
	}


	while (!ok) {
		ok = true;

		// Fill islands that can't be resolved nicely.
		for (int tz = 1; tz < mapwidth - 1; ++tz)
		for (int tx = 1; tx < mapheight - 1; ++tx)
		{
			i = tz * mapwidth + tx;
			auto* tile = &(maptiles[i]);

			// Process only the matching group.
			if (!Tile_in_group(tile, group_this)) {
				continue;
			}

			// Count bordering sides.
			auto countmatch = 0;
			for (int ntz = -1; ntz <= 1; ++ntz)
			for (int ntx = -1; ntx <= 1; ++ntx) {
				// Skip diagonals.
				if (ntx != 0 && ntz != 0) {
					continue;
				}

				auto tile2 = Get_tile(tile, ntx, ntz);

				countmatch += Tile_in_group(tile2, group_other);
			}

			// If above threshold, change neighbour tiles.
			if (countmatch >= fill_islands_min_neighbours) {
				for (int ntz = -1; ntz <= 1; ++ntz)
				for (int ntx = -1; ntx <= 1; ++ntx) {
					auto tile2 = Get_tile(tile, ntx, ntz);

					if (Tile_in_group(tile2, group_other)) {
						Set_current_texture_by_name(group_this);
						ChangeTileTexture(tile2, Get_curtex());
						tile2->rot = rand() % 4;
						tile2->xflip = rand() % 2;
						tile2->zflip = rand() % 2;
					}
				}

				ok = false;
			}
		}

		// Signal changes.
		if (!ok) {
			oook = false;
		}
	}

	return oook;
}

// Changes border tiles to transition group without fitting them.
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

		for (int tz = 1; tz < mapwidth - 1; ++tz) {
			for (int tx = 1; tx < mapheight - 1; ++tx) {

				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);
				if (transition->match_from ?
					Tile_in_group(tile, transition->from_group) :
					Tile_in_group(tile, transition->to_group))
				{

					// Check if is on the border.
					uchar shape_from = Get_shape(tile, transition->from_group);
					if (shape_from == 0) {
						continue;
					}
					uchar shape_to = Get_shape(tile, transition->to_group);
					if (shape_to == 0) {
						continue;
					}

					// Change into transition tile.
					auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
					change->tile_id = 0;
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

// Fix transitions without connection to both groups.
bool Prune_transitions(TextureLayerTransition* transition)
{
	int i;

	char* group_this;
	char* group_other;
	if (transition->match_from) {
		group_this = transition->from_group;
		group_other = transition->to_group;
	} else {
		group_this = transition->to_group;
		group_other = transition->from_group;
	}

	for (int tz = 1; tz < mapwidth - 1; ++tz) {
		for (int tx = 1; tx < mapheight - 1; ++tx) {

			i = tz * mapwidth + tx;
			auto* tile = &(maptiles[i]);
			if (Tile_in_group(tile, transition->transition_group)) {
				uchar shape_from = Get_shape(tile, group_this);
				if (shape_from == 0) {
					auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
					change->tile_id = rand() % 4;// tile->mt->grp->tex->len;
					change->rotation = rand() % 4;
					change->flipx = rand() % 2;
					change->flipz = rand() % 2;
				}
			} // tile in group
		}
	}
	Apply_change_buffer(group_other);

	for (int tz = 1; tz < mapwidth - 1; ++tz) {
		for (int tx = 1; tx < mapheight - 1; ++tx) {
			i = tz * mapwidth + tx;
			auto* tile = &(maptiles[i]);
			if (Tile_in_group(tile, transition->transition_group)) {
				uchar shape_to = Get_shape(tile, group_other);
				if (shape_to == 0) {//shape_from --- ???
					auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
					change->tile_id = rand() % 4;// tile->mt->grp->tex->len;
					change->rotation = rand() % 4;
					change->flipx = rand() % 2;
					change->flipz = rand() % 2;
				}
			} // tile in group
		}
	}
	Apply_change_buffer(group_this);

	for (int tz = 1; tz < mapwidth - 1; ++tz) {
		for (int tx = 1; tx < mapheight - 1; ++tx) {
			i = tz * mapwidth + tx;
			auto* tile = &(maptiles[i]);
			if (Tile_in_group(tile, transition->transition_group)) {
				uchar shape_tr = Get_shape(tile, transition->transition_group);
				if ((shape_tr & 0b10101010) == 0b10101010) {
					auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
					change->tile_id = rand() % 4;// tile->mt->grp->tex->len;
					change->rotation = rand() % 4;
					change->flipx = rand() % 2;
					change->flipz = rand() % 2;
				}
			} // tile in group
		}
	}
	Apply_change_buffer(group_other);

	return true;
}

// Fits tiles to match exact shapes.
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

					uchar shape_to = Get_shape(tile, transition->to_group);
					uchar shape_from = Get_shape(tile, transition->from_group);

					int tid = Get_best_tile(
						shape_to,
						shape_from,
						rotation,
						flipx,
						flipz
					);

					/*int tid = Fit_tile(
						shape_to,
						shape_from,
						true,
						true,
						rotation,
						flipx,
						flipz
					);

					if (tid == -1) {
						tid = Fit_tile(
							shape_to,
							shape_from,
							true,
							false,
							rotation,
							flipx,
							flipz
						);
					}

					if (tid == -1) {
						tid = Fit_tile(
							shape_to,
							shape_from,
							false,
							false,
							rotation,
							flipx,
							flipz
						);
					}*/

					if (tid == -1) {
						tid = -2;
						ok = false;
					}

					auto* change = &(changebuf[tile->z * mapwidth + tile->x]);
					change->tile_id = tid;
					change->rotation = rotation;
					change->flipx = flipx;
					change->flipz = flipz;
					Randomize_tile(change);
				} // tile in group
			}
		}

		if (strcmp(transition->transition_group, "ROCKYGRASS2-D_EARTH1") == 0) {
			for (int i = 0; i < bufsize; ++i) {
				if (changebuf[i].tile_id == 2) {
					changebuf[i].tile_id = 9;
				}
			}
		}
		Apply_change_buffer(transition->transition_group, transition->from_group);

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

	printf("fix seams begin\n");

	while (!ok) {
		ok = true;

		for (int t = 0; t < transitions.len; ++t)
		{
			auto* transition = &(transitions[t]);
			ok &= Fill_islands(transition);
		}
		printf("fill\n");

		for (int t = 0; t < transitions.len; ++t)
		{
			auto* transition = &(transitions[t]);
			ok &= Match_tiles1(transition);
		}
		printf("match 1\n");

		for (int t = 0; t < transitions.len; ++t)
		{
			auto* transition = &(transitions[t]);
			ok &= Prune_transitions(transition);
		}
		printf("prune\n");	
			
		if (++counter >= fix_seams_iterations) {
			printf("fix seams max iterations reached\n");
			break;
		}
	}

	for (int t = 0; t < transitions.len; ++t)
	{
		auto* transition = &(transitions[t]);
		ok &= Match_tiles2(transition);
	}
	printf("match 2\n");

	printf("fix seams end\n");

	Free_change_buffer();
	Texture_cleanup();

	// Loop over all tiles - match corners - ID 8, 9 - needed?
}

// todo - match on transition tiles instead
// todo - match on all things ?
// todo - if can't find both shapes then match on tex only
// todo - fix mismatchy tiles - maybe another set for fixes


