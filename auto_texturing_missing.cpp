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


void Apply_replacement(TextureLayerIntermediate& layer)
{
	int i;
	bool ok = true;
	bool tileok;

	auto* tex = Get_texture_group(layer.replacement);

	for (int tz = 0; tz < mapwidth; ++tz) {
		for (int tx = 0; tx < mapheight; ++tx) {

			i = tz * mapwidth + tx;
			auto* tile = &(maptiles[i]);
			char* wanted;
			if (Tile_in_group(tile, layer.group_a)) {
				wanted = layer.group_b;
			} else if (Tile_in_group(tile, layer.group_b)) {
				wanted = layer.group_a;
			} else {
				continue;
			}

			tileok = true;
			for (int ntz = -layer.radius; ntz <= layer.radius; ++ntz) {
				for (int ntx = -layer.radius; ntx <= layer.radius; ++ntx) {
					if (ntx != 0 && ntz != 0) {
						continue;
					}
					if (ntx * ntx + ntz * ntz > layer.radius * layer.radius) {
						continue;
					}
					auto tile2 = Get_tile(tile, ntx, ntz);
					if (!tile2) {
						continue;
					}
					if (Tile_in_group(tile2, wanted)) {
						auto& tc = changebuf[tile2->z * mapwidth + tile2->x];
						tc.tile_id = rand() % tex->tex->len;
						tc.flipx = rand() % 2;
						tc.flipz = false;
						tc.rotation = rand() % 4;
						tileok = false;
						//goto exloop;
					}
				}
			}
			//exloop:
			if (!tileok) {
				ok = false;
				//changebuf[i].tile_id = rand() % tex->tex->len;
			}
		}
	}

	Apply_change_buffer(layer.replacement);
}

