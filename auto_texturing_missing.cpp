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



bool Apply_replacements()
{
	int i;
	bool ok = true;
	bool tileok;

	for (int li = 0; li < replacements.len; ++li) {

		auto& layer = replacements[li];
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

	return ok;
}


bool Resolve_knots()
{
	int i;
	bool ok = true;
	bool tileok;
	bool layerok;
	int counter = 0;

	char* other_group;
	for (int li = 0; li < knots.len; ++li) {

		auto& layer = knots[li];
		auto* tex = Get_texture_group(layer.group_name);

		layerok = false;
		while (!layerok) {
			layerok = true;
			for (int tz = 0; tz < mapwidth; ++tz) {
				for (int tx = 0; tx < mapheight; ++tx) {

					i = tz * mapwidth + tx;
					auto* tile = &(maptiles[i]);
					if (!Tile_in_group(tile, layer.group_name)) {
						continue;
					}

					tileok = true;
					other_group = nullptr;
					for (int ntz = -1; ntz < 2; ++ntz) {
						for (int ntx = -1; ntx < 2; ++ntx) {

							/*if (ntx != 0 && ntz != 0) {
								continue;
							}*/

							auto tile2 = Get_tile(tile, ntx, ntz);
							if (!tile2) {
								continue;
							}

							if (!Tile_in_group(tile2, layer.group_name)) {
								if (!other_group) {
									for (int j = 0; j < transitions.len; ++j) {
										if (strcmp(transitions[j].from_group, layer.group_name) == 0
											&& Tile_in_group(tile2, transitions[j].to_group))
										{
											other_group = transitions[j].to_group;
											break;
										}
										if (strcmp(transitions[j].to_group, layer.group_name) == 0
											&& Tile_in_group(tile2, transitions[j].from_group))
										{
											other_group = transitions[j].from_group;
											break;
										}
									}
									tileok = false;
									goto exloop;
								} else if (!Tile_in_group(tile2, other_group)) {
									tileok = false;
									goto exloop;
								}
							}
						}
					}
				exloop:
					if (!tileok) {
						layerok = false;
						ok = false;
						for (int ntz = -1; ntz < 2; ++ntz) {
							for (int ntx = -1; ntx < 2; ++ntx) {

								if (ntx != 0 && ntz != 0) {
									continue;
								}

								auto tile2 = Get_tile(tile, ntx, ntz);
								if (!tile2) {
									continue;
								}

								if (!other_group) {
									changebuf[tile2->z * mapwidth + tile2->x].tile_id =
										rand() % tex->tex->len;
									continue;
								}
								
								if (!Tile_in_group(tile2, other_group)) {
									changebuf[tile2->z * mapwidth + tile2->x].tile_id =
										rand() % tex->tex->len;
								}
							}
						}
					}

				} // tiles
			}

			Apply_change_buffer(layer.group_name);

			if (++counter >= knot_iterations) {
				break;
			}
		}

	}

	return ok;
}


void Texture_replace_missing_transitions()
{
	Texture_read_layer_files();
	Create_change_buffer();

	bool ok1 = false;
	bool ok2 = false;
	int counter1 = 0;
	int counter2 = 0;
	while (!ok1) {
		ok1 = true;

		ok1 = Resolve_knots();

		while (!ok2) {
			ok2 = Apply_replacements();

			if (++counter2 >= replacement_iterations) {
				printf("replacements max iterations reached\n");
				break;
			}
		}
		
		if (++counter1 >= knot_iterations) {
			printf("knots max iterations reached\n");
			break;
		}
	}

	Free_change_buffer();
	Texture_cleanup();
}