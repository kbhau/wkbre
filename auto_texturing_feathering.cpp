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



// Use first to apply base texture layers.
void Texture_feather_seams()
{
	Texture_read_layer_files();
	Create_change_buffer();

	int ti;
	bool tile_ok;
	
	// Loop over all layers - loop over already visited tiles to allow overwriting.
	for (int fi = 0; fi < feathers.len; ++fi) {
		auto* feather = &feathers[fi];
		auto* tex_to = Get_texture_group(feather->to);

		for (int i = 0; i < feather->iterations; ++i) {
			for (int tz = 0; tz < mapwidth; ++tz)
			for (int tx = 0; tx < mapheight; ++tx)
			{
				ti = tz * mapwidth + tx;
				auto* tile = &maptiles[ti];

				// This tile needs to be "from"
				if (!Tile_in_group(tile, feather->from)) {
					continue;
				}

				// Check neighbours to be either "from" or "to"
				tile_ok = true;
				for (int ntz = -feather->border; ntz <= feather->border; ++ntz)
				for (int ntx = -feather->border; ntx <= feather->border; ++ntx)
				{
					// Skip if same tile.
					if (ntz == 0 && ntx == 0) {
						continue;
					}

					// Make circle.
					if (feather->border > 3)
					if (ntz * ntz + ntx * ntx > feather->border * feather->border) {
						continue;
					}

					// Check tile.
					auto* tile2 = Get_tile(tile, ntx, ntz);
					if (!tile2) {
						continue;
					}
					if (!Tile_in_group(tile2, feather->from)
						&& !Tile_in_group(tile2, feather->to))
					{
						tile_ok = false;
						goto exloop;
					}
				}
			exloop:
				if (!tile_ok) {
					continue;
				}

				// Check neighbours to include at least one "to".
				tile_ok = false;
				for (int ntz = -feather->seek_radius; ntz <= feather->seek_radius; ++ntz)
				for (int ntx = -feather->seek_radius; ntx <= feather->seek_radius; ++ntx)
				{
					// Skip if same tile.
					if (ntz == 0 && ntx == 0) {
						continue;
					}

					// Make circle.
					if (feather->border > 3)
					if (ntz * ntz + ntx * ntx > feather->seek_radius * feather->seek_radius) {
						continue;
					}

					// Check tile.
					auto* tile2 = Get_tile(tile, ntx, ntz);
					if (!tile2) {
						continue;
					}
					if (Tile_in_group(tile2, feather->to)) {
						tile_ok = true;
						goto exloop2;
					}
				}
			exloop2:
				if (!tile_ok) {
					continue;
				}

				// Check probability.
				if (double(rand()) / RAND_MAX >= feather->probability) {
					continue;
				}

				changebuf[ti].tile_id = rand() % tex_to->tex->len;
			}

			Apply_change_buffer(feather->to);
		} // iterations
	} // Feathers loop


	Free_change_buffer();
	Texture_cleanup();
}
