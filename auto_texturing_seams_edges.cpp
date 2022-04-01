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



struct Tile_edges
{
	// n, e, s, w
	uchar e[4];
};

Tile_edges edges[10] = {
	{0b1100, 0b0000, 0b0011, 0b1111},
	{0b1100, 0b0000, 0b0000, 0b0011},
	{0b1111, 0b1100, 0b0011, 0b1111},
	{0b1110, 0b0000, 0b0000, 0b0111},
	{0b1111, 0b1100, 0b0000, 0b0111},
	{0b1111, 0b1110, 0b0000, 0b0011},
	{0b1111, 0b1000, 0b0000, 0b0011},
	{0b1111, 0b1100, 0b0000, 0b0001},
	{0b1000, 0b0000, 0b0000, 0b0001},
	{0b1111, 0b1110, 0b0111, 0b1111},
};



class Tile_edge_shape
{
public:
	

public:
	void Get_edges(
		const MapTile* tile,
		TextureLayerTransition* transition
	);
	TileTexChange Get_tex_change();
	void Read_tile(const MapTile* tile);

private:
	uchar n = 0;
	uchar e = 0;
	uchar s = 0;
	uchar w = 0;
	uchar sn = 0;
	uchar se = 0;
	uchar ss = 0;
	uchar sw = 0;
	uchar dir = 0;
	bool flip_x = false;
	uchar tile_id = 0;

	int bscore = -1;
	uchar bid = 0;
	uchar bdir = 0;
	bool bx = false;

private:
	uchar Get_edge(
		MapTile* tile,
		TextureLayerTransition* transition,
		int start_dir
	);
	uchar Mirror_edge(uchar e);

	void Rotate();
	void Flip_x();
	int Get_edge_score(int me, int neighbour);
	void Get_score();
};



uchar Tile_edge_shape::Mirror_edge(uchar e)
{
	return
		  ((e & 0b00001000) >> 3)
		| ((e & 0b00000100) >> 1)
		| ((e & 0b00000010) << 1)
		| ((e & 0b00000001) << 3)
		| ( e & 0b11110000);
}

void Tile_edge_shape::Rotate()
{
	/*if (flip_x) {
		auto tmp = ss;
		ss = sw;
		sw = sn;
		sn = se;
		se = tmp;
	} else {
		auto tmp = sn;
		sn = sw;
		sw = ss;
		ss = se;
		se = tmp;
	}*/

	++dir;
	dir %= 4;
	auto tmp = sn;
	sn = sw;
	sw = ss;
	ss = se;
	se = tmp;

	/*auto tmp = sn;
	sn = se;
	se = ss;
	ss = sw;
	sw = tmp;*/
}

void Tile_edge_shape::Flip_x()
{
	// Flip around X axis, which means swap n and s.

	auto tmp = sn;
	sn = Mirror_edge(ss);
	ss = Mirror_edge(tmp);
	se = Mirror_edge(se);
	sw = Mirror_edge(sw);

	flip_x = !flip_x;
}



uchar Tile_edge_shape::Get_edge(
	MapTile* tile,
	TextureLayerTransition* transition,
	int start_dir
) {
	if (Tile_in_group(tile, transition->from_group)) {
		return 240;
	}
	if (Tile_in_group(tile, transition->to_group)) {
		return 255;
	}

	auto eg = edges[tile->mt->id];
	int dir = start_dir - tile->rot;

	// Double flip is equivalent to rotating 180 deg.
	if (tile->xflip && tile->zflip) dir -= 2;

	// Bring back to [0..3].
	while (dir < 0) dir += 4;

	// Account for flips. (x and z seem swapped in editor for some reason)
	if ((bool)tile->xflip) {
		if (dir == 2) {
			dir = 0;
		} else if (dir == 0) {
			dir = 2;
		}
	} else if ((bool)tile->zflip) {
		if (dir == 3) {
			dir = 1;
		} else if (dir == 1) {
			dir = 3;
		}
	}
	
	// Get the bits.
	auto e = eg.e[dir];
	if ((bool)tile->xflip ^ (bool)tile->zflip) {
		e = Mirror_edge(e);
	}

	return Mirror_edge(e);
}

void Tile_edge_shape::Get_edges(
	const MapTile* tile,
	TextureLayerTransition* transition
) {
	if (tile->z > 0) {
		n = Get_edge(Get_N_tile(tile), transition, 2);
	} else {
		n = 0;
	}
	if (tile->x < mapwidth - 1) {
		e = Get_edge(Get_E_tile(tile), transition, 3);
	} else {
		e = 0;
	}
	if (tile->z < mapheight - 1) {
		s = Get_edge(Get_S_tile(tile), transition, 0);
	} else {
		s = 0;
	}
	if (tile->x > 0) {
		w = Get_edge(Get_W_tile(tile), transition, 1);	
	} else {
		w = 0;
	}
}



int Tile_edge_shape::Get_edge_score(int me, int neighbour)
{
	// Scoring allows us priorities.
	// - Full tiles
	// - Full edges
	// - Matching edges
	// - Matching single bits

	auto bullseye = 100;
	auto single = 1;
	if ((neighbour & 240) == 240) {
		bullseye = 1000;
		single = 6;
	} else if ((neighbour & 15) == 15 || (neighbour & 15) == 0) {
		bullseye = 500;
		single = 3;
	}

	auto val = (~(me ^ neighbour)) & 15;

	if (val == 15) {
		return bullseye;
	}

	auto sum = 0;
	for (int i = 0; i < 4; ++i) {
		if (val & (1 << i)) {
			sum += single;
		}
	}

	return sum;
}

void Tile_edge_shape::Get_score()
{
	auto score = Get_edge_score(sn, n)
		+ Get_edge_score(se, e)
		+ Get_edge_score(ss, s)
		+ Get_edge_score(sw, w);

	if (score > bscore) {
		bscore = score;
		bdir = dir;
		bx = flip_x;
		bid = tile_id;
	}
}

TileTexChange Tile_edge_shape::Get_tex_change()
{
	// ASSUMPTION: Neighbour edges are already set.

	// Only check base and x flip. Other combinations are redundant.
	bdir = 0;
	bid = -1;
	bx = false;
	bscore = -1;
	for (tile_id = 0; tile_id < 10; ++tile_id) {
		sn = edges[tile_id].e[0];
		se = edges[tile_id].e[1];
		ss = edges[tile_id].e[2];
		sw = edges[tile_id].e[3];
		dir = 0;
		for (int i = 0; i < 4; ++i) {
			Get_score();
			Rotate();
		}
		Flip_x();
		for (int i = 0; i < 4; ++i) {
			Get_score();
			Rotate();
		}
	}

	TileTexChange tc;
	tc.tile_id = bid;
	tc.rotation = bdir;
	tc.flipx = bx;
	tc.flipz = false;

	// Without this, corner tiles are misaligned. If you happen to solve the
	// mystery why, contact me. Unless you're not gonna tell me, in which case
	// don't.
	if (tc.flipx && (tc.tile_id == 1 || tc.tile_id == 3 || tc.tile_id == 9)) {
		tc.rotation += 1;
		tc.rotation %= 4;
	}

	return tc;
}



bool Match_tiles_by_edges(TextureLayerTransition* transition)
{
	auto ok = false;

	for (int iter = 0; iter < fix_seams_by_edges_iterations * 4; ++iter) {

		auto offset = iter % 4;
		auto offset2 = (iter + 2) % 4;

		for (int tz = 0; tz < mapheight; ++tz)
		for (int tx = ((tz % 2) ? offset : offset2); tx < mapwidth; tx += 4)
		{
			auto i = tz * mapwidth + tx;
			const auto* tile = &(maptiles[i]);

			if (!Tile_in_group(tile, transition->transition_group)) {
				continue;
			}

			auto shape = new Tile_edge_shape();
			shape->Get_edges(tile, transition);
			auto change = shape->Get_tex_change();
			// TODO: list misfits and change randomly to solid tiles
			Randomize_tile(&change);
			changebuf[tile->z * mapwidth + tile->x] = change;

			delete shape;
		}
		Apply_change_buffer(transition->transition_group, transition->from_group);
	}


	return ok;
}



void Tile_edge_shape::Read_tile(const MapTile* tile)
{
	sn = edges[tile->mt->id].e[0];
	se = edges[tile->mt->id].e[1];
	ss = edges[tile->mt->id].e[2];
	sw = edges[tile->mt->id].e[3];

	if (tile->xflip) {
		Flip_x();
	}
	for (int i = 0; i < tile->rot; ++i) {
		Rotate();
	}

	auto pb = [this](int e)
	{
		printf("[");
		for (int i = 1; i <= 8; ++i) {
			printf("%d", (bool)(e & (1 << (8 - i))));
		}
		printf("]");
	};

	printf("Tile: ");
	pb(sn);
	pb(se);
	pb(ss);
	pb(sw);
	printf("\n");
	printf("Surr: ");
	pb(n);
	pb(e);
	pb(s);
	pb(w);
	printf("\n\n");
}

void Debug_tile(MapTile* tile)
{
	return;
	Texture_read_layer_files();

	printf("Debug tile.\n");
	for (int i = 0; i < transitions.len; ++i) {
		auto transition = &transitions[i];
		/*printf(transition->transition_group);
		printf(",");
		printf(tile->mt->grp->name);
		printf("\n");*/
		if (!Tile_in_group(tile, transition->transition_group)) {
			continue;
		}
		
		auto shape = new Tile_edge_shape();
		shape->Get_edges(tile, transition);
		shape->Read_tile(tile);

		break;
	}

	Texture_cleanup();
}
