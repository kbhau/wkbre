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
			if (!Tile_in_group(tile, layer.group_a)) {
				continue;
			}

			tileok = true;
			for (int ntz = -1; ntz < 2; ++ntz) {
				for (int ntx = -1; ntx < 2; ++ntx) {
					if (ntx != 0 && ntz != 0) {
						continue;
					}
					auto tile2 = Get_tile(tile, ntx, ntz);
					if (!tile2) {
						continue;
					}
					if (Tile_in_group(tile2, layer.group_b)) {
						changebuf[tile2->z * mapwidth + tile2->x].tile_id =
							rand() % tex->tex->len;
						tileok = false;
						//goto exloop;
					}
				}
			}
			//exloop:
			if (!tileok) {
				ok = false;
				changebuf[i].tile_id = rand() % tex->tex->len;
			}
		}
	}

	Apply_change_buffer(layer.replacement);
}

