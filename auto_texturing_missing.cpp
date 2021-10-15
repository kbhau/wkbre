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



void Texture_replace_missing_transitions()
{
	Texture_read_layer_files();
	Create_change_buffer();
	
	printf("replace missing 1\n");

	int i;
	int x;
	int z;
	int counter = 0;
	bool ok = false;

	while (!ok) {
		ok = true;

		for (int li = 0; li < replacements.len; ++li) {

			auto& layer = replacements[li];
			auto* tex = Get_texture_group(layer.replacement);

			for (int tz = 1; tz < mapwidth - 1; ++tz) {
				for (int tx = 1; tx < mapheight - 1; ++tx) {
				
					i = tz * mapwidth + tx;
					auto* tile = &(maptiles[i]);
					if (!Tile_in_group(tile, layer.group_a)) {
						continue;
					}

					for (int ntz = -1; ntz < 2; ++ntz) {
						for (int ntx = -1; ntx < 2; ++ntx) {
							if (Tile_in_group(Get_tile(tile, ntx, ntz), layer.group_b)) {
								changebuf[i].tile_id = rand() % tex->tex->len;
								ok = false;
								goto exloop;
							}
						}
					}
				exloop:
					ok = false;
				}
			}
			Apply_change_buffer(layer.replacement);
		}

		if (++counter >= 100) {
			printf("replacing unresolved - infinite loop in layer replacement\n");
			break;
		}

	}

	printf("replace missing 2\n");

	Free_change_buffer();
	Texture_cleanup();
}