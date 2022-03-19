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
void Texture_apply_main_layers()
{
	float slope;
	float maxslope = 0;
	float avslope = 0;
	int avtiles = 0;
	float height;
	int i;

	Texture_read_layer_files();

	// Todo: if slow then precompute layers 

	// Loop over all layers - loop over already visited tiles to allow overwriting.
	for (int li = 0; li < main_layers.len; ++li) {
		auto& layer = main_layers[li];


		for (int tz = 0; tz < mapwidth; ++tz) {
			for (int tx = 0; tx < mapheight; ++tx) {

				// Compute slope and height based on corner vertices.
				slope = 0;
				height = 0;
				i = tz * (mapwidth + 1) + tx;
				uchar ul = himap_byte[i];
				uchar ur = himap_byte[i + 1];
				uchar dl = himap_byte[i + mapwidth + 1];
				uchar dr = himap_byte[i + mapwidth + 1 + 1];
				Get_edge_slope(ul, ur, slope);
				Get_edge_slope(ur, dr, slope);
				Get_edge_slope(dr, dl, slope);
				Get_edge_slope(dl, ul, slope);
				Get_diag_slope(ul, dr, slope);
				Get_diag_slope(dl, ur, slope);

				height = (ul + ur + dl + dr) * 0.25 * maphiscale;
				slope = slope * 57.295779513;


				if (slope > maxslope) {
					maxslope = slope;
				}
				avslope += slope;
				avtiles += 1;

				if (height >= layer.height_min && height < layer.height_max
					&& slope >= layer.slope_min && slope < layer.slope_max)
				{
					auto* mt = &(maptiles[tz * mapwidth + tx]);
					Set_current_texture_by_name(layer.group_name);
					ChangeTileTexture(mt, Get_curtex());
					/*mt->rot = rand() % 4;
					mt->xflip = rand() % 2;
					mt->zflip = rand() % 2;*/
					mt->rot = 0;// rand() % 4;
					mt->xflip = false;// rand() % 2;
					mt->zflip = false;// rand() % 2;
				}
			}
		}
		//printf("layer smin=%d smax=%d\n", layer.slope_min, layer.slope_max);
	}
	//printf("maxslope: %.5f\n", maxslope);
	//printf("avslope: %.5f\n", avslope / avtiles);

	Texture_cleanup();
}
