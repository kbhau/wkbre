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
			&& (shape_from & 0b10101010) == (tiles_from[i] & 0b10101010)) {
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
)
{
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
)
{
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
)
{
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
)
{
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
)
{
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




// Fits tiles to match exact shapes.
bool Match_tiles_by_tile_type(TextureLayerTransition* transition)
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
					//Randomize_tile(change);
				} // tile in group
			}
		}

		/*if (strcmp(transition->transition_group, "ROCKYGRASS2-D_EARTH1") == 0) {
			for (int i = 0; i < bufsize; ++i) {
				if (changebuf[i].tile_id == 2) {
					changebuf[i].tile_id = 9;
				}
			}
		}*/
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
