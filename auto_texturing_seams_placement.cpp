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


// Turns single fully surrounded tiles into one of predefined shapes.
// Alternatively fills in the tile.
void Explode_islands(TextureLayerTransition* transition)
{
	// TODO.
	// Note: adds need to check for transition tiles in other functinos.
}


// Adds transition tiles around tiles surrounded from 3 sides.
void Explode_peninsulas(TextureLayerTransition* transition)
{
	// TODO.
	// Note: adds need to check for transition tiles in other functinos.
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
			for (int tx = 1; tx < mapheight - 1; ++tx) {
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


	while (!ok) {
		ok = true;

		// Fill islands that can't be resolved nicely.
		for (int tz = 1; tz < mapwidth - 1; ++tz)
			for (int tx = 1; tx < mapheight - 1; ++tx) {
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
bool Place_transitions(TextureLayerTransition* transition)
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
					Tile_in_group(tile, transition->to_group)) {

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
					change->tile_id = rand() % 10;
					change->flipx = rand() % 2;
					change->flipz = false;
					change->rotation = rand() % 4;
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
