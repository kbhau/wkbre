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



void Texture_apply_inner_layers()
{
	Texture_read_layer_files();
	Create_change_buffer();

	//printf("apply inners 1\n");

	int i;
	int x;
	int z;
	int counter = 0;
	bool pure = false;
	float slope;
	float height;

	for (int li = 0; li < inner_layers.len; ++li) {

		auto& layer = inner_layers[li];
		auto* tex = Get_texture_group(layer.inner_group_name);

		for (int tz = 0; tz < mapwidth; ++tz) {
			for (int tx = 0; tx < mapheight; ++tx) {

				i = tz * mapwidth + tx;
				auto* tile = &(maptiles[i]);
				if (!Tile_in_group(tile, layer.parent_group_name)) {
					continue;
				}

				// Compute slope and height based on corner vertices.
				slope = 0;
				height = 0;
				int j = tz * (mapwidth+1) + tx;
				uchar ul = himap_byte[j];
				uchar ur = himap_byte[j + 1];
				uchar dl = himap_byte[j + mapwidth + 1];
				uchar dr = himap_byte[j + mapwidth + 1 + 1];
				Get_edge_slope(ul, ur, slope);
				Get_edge_slope(ur, dr, slope);
				Get_edge_slope(dr, dl, slope);
				Get_edge_slope(dl, ul, slope);
				Get_diag_slope(ul, dr, slope);
				Get_diag_slope(dl, ur, slope);
				height = (ul + ur + dl + dr) * 0.25 * maphiscale;
				slope = slope * 57.295779513;

				// If does not meet criteria, skip.
				if (height < layer.height_min || height >= layer.height_max
					|| slope < layer.slope_min || slope >= layer.slope_max)
				{
					continue;
				}

				pure = true;
				for (int ntz = -layer.border_radius; ntz <= layer.border_radius; ++ntz) {
					for (int ntx = -layer.border_radius; ntx <= layer.border_radius; ++ntx) {

						// We iterate square but want circle. Square the circle.
						if (layer.border_radius > 3)
						if (ntz * ntz + ntx * ntx > layer.border_radius * layer.border_radius) {
							continue;
						}
						auto tile2 = Get_tile(tile, ntx, ntz);
						if (!tile2) {
							continue;
						}
						if (!Tile_in_group(tile2, layer.parent_group_name)) {
							pure = false;
							goto exloop;
						}
					}
				}
			exloop:
				if (pure) {
					changebuf[i].tile_id = rand() % tex->tex->len;
				}
			}
		}
		Apply_change_buffer(layer.inner_group_name);
	}

	//printf("apply inners 2\n");

	Free_change_buffer();
	Texture_cleanup();
}
