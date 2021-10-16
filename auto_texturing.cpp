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



GrowList<TextureLayerMain> main_layers;
GrowList<TextureLayerIntermediate> replacements;
GrowList<TextureLayerInner> inner_layers;
GrowList<TextureLayerTransition> transitions;

float sqrt2 = sqrt(2);

int bufsize = 0;
TileTexChange* changebuf;



void Texture_read_layer_files()
{
	// Try to open file.
	auto* f = fopen("redata\\texture_layers.tlf", "r");

	// No file, no fun.
	if (!f) {
		return;
	}

	// Set vars, allocate resources.
	auto buf = 100;
	char* line = (char*)malloc(sizeof(char) * buf);
	int mode = -1; // -1 = undefined, 0 = vars, 1 = layers, 2 = inners, 3 = transitions
	GrowStringList varsk;
	GrowList<int> varsv;

	auto getval = [&varsk, &varsv](char* token)->int {
		for (int i = 0; i < varsk.len; ++i) {
			if (strcmp(token, varsk.getdp(i)) == 0) {
				return varsv[i];
			}
		}
		return atoi(token);
	};

	// Loop until end reached.
	while (!feof(f)) {

		// Get next line.
		if (!fgets(line, buf, f)) {
			break;
		}

		// Skip comments and empty lines.
		if (line[0] == '#' || line[0] == '\n') {
			continue;
		}

		// Set mode.
		if (line[0] == '!') {
			if (strcmp(line, "!VARS\n") == 0) {
				mode = 0;
			} else if (strcmp(line, "!LAYERS\n") == 0) {
				mode = 1;
			} else if (strcmp(line, "!INTERMEDIATE\n") == 0) {
				mode = 2;
			} else if (strcmp(line, "!INNERS\n") == 0) {
				mode = 3;
			} else if (strcmp(line, "!TRANSITIONS\n") == 0) {
				mode = 4;
			} else {
				mode = -1;
			}
			continue;
		}

		// Read vars.
		if (mode == 0) {
			varsk.add(strtok(line, "="));
			varsv.add(getval(strtok(nullptr, "\n")));
			continue;
		}

		// Read layers.
		if (mode == 1) {
			char* name = (char*)malloc(sizeof(char) * 30); strcpy(name, strtok(line, ","));
			int height_min = getval(strtok(nullptr, ","));
			int height_max = getval(strtok(nullptr, ","));
			int slope_min = getval(strtok(nullptr, ","));
			int slope_max = getval(strtok(nullptr, "\n"));
			main_layers.add(TextureLayerMain{
				name,
				height_min,
				height_max,
				slope_min,
				slope_max
				});
			continue;
		}

		// Read replacements.
		if (mode == 2) {
			char* grp_a = (char*)malloc(sizeof(char) * 30); strcpy(grp_a, strtok(line, ","));
			char* grp_b = (char*)malloc(sizeof(char) * 30); strcpy(grp_b, strtok(nullptr, ","));
			char* repl = (char*)malloc(sizeof(char) * 30); strcpy(repl, strtok(nullptr, "\n"));
			replacements.add(TextureLayerIntermediate{
				grp_a,
				grp_b,
				repl
				});
			continue;
		}

		// Read inners.
		if (mode == 3) {
			char* parent_name = (char*)malloc(sizeof(char) * 30); strcpy(parent_name, strtok(line, ","));
			char* inner_name = (char*)malloc(sizeof(char) * 30); strcpy(inner_name, strtok(nullptr, ","));
			int border_radius = getval(strtok(nullptr, "\n"));
			inner_layers.add(TextureLayerInner{
				parent_name,
				inner_name,
				border_radius
				});
			continue;
		}

		// Read transitions.
		if (mode == 4) {
			char* trans_grp = (char*)malloc(sizeof(char) * 30); strcpy(trans_grp, strtok(line, ","));
			char* from_grp = (char*)malloc(sizeof(char) * 30); strcpy(from_grp, strtok(nullptr, ","));
			char* to_grp = (char*)malloc(sizeof(char) * 30); strcpy(to_grp, strtok(nullptr, "\n"));
			transitions.add(TextureLayerTransition{
				from_grp,
				to_grp,
				trans_grp
				});
			continue;
		}
	}

	// Free resources.
	free(line);
	fclose(f);

	// Print debug.
	for (int i = 0; i < main_layers.len; ++i) {
		auto& it = main_layers[i];
		printf("M:%s,%d,%d,%d,%d\n", it.group_name, it.height_min, it.height_max, it.slope_min, it.slope_max);
	}
	for (int i = 0; i < replacements.len; ++i) {
		auto& it = replacements[i];
		printf("R:%s,%s,%s\n", it.group_a, it.group_b, it.replacement);
	}
	for (int i = 0; i < inner_layers.len; ++i) {
		auto& it = inner_layers[i];
		printf("I:%s,%s,%d\n", it.parent_group_name, it.inner_group_name, it.border_radius);
	}
	for (int i = 0; i < transitions.len; ++i) {
		auto& it = transitions[i];
		printf("T:%s,%s,%s\n", it.transition_group, it.from_group, it.to_group);
	}
}

void Texture_cleanup()
{
	for (int i = 0; i < main_layers.len; ++i) {
		free(main_layers[i].group_name);
	}

	for (int i = 0; i < replacements.len; ++i) {
		free(replacements[i].group_a);
		free(replacements[i].group_b);
		free(replacements[i].replacement);
	}

	for (int i = 0; i < inner_layers.len; ++i) {
		free(inner_layers[i].parent_group_name);
		free(inner_layers[i].inner_group_name);
	}

	for (int i = 0; i < transitions.len; ++i) {
		free(transitions[i].from_group);
		free(transitions[i].to_group);
		//free(transitions[i].match_group);
		free(transitions[i].transition_group);
	}

	main_layers.clear();
	replacements.clear();
	inner_layers.clear();
	transitions.clear();
}

MapTextureGroup* Get_texture_group(const char* name)
{
	for (int i = 0; i < maptexgroup.len; ++i) {
		if (strcmp(maptexgroup.getpnt(i)->name, name) == 0) {
			return maptexgroup.getpnt(i);
		}
	}
	return nullptr;
}

MapTexture* Get_texture(const char* group_name, int id)
{
	auto* group = Get_texture_group(group_name);
	if (!group) {
		printf("nogrp: %s\n", group_name);
		return nullptr;
	}
	if (group->tex->len <= id) {
		printf("notex: %s: %d\n", group_name, id);
		return nullptr;
	}
	return group->tex->getpnt(id);
}

void Set_current_texture_by_name(const char* name)
{
	for (int i = 0; i < maptexgroup.len; ++i) {
		if (strcmp(maptexgroup.getpnt(i)->name, name) == 0) {
			Set_curtexgrp(maptexgroup.getpnt(i));
			SelectNextRandomMapTile();
			return;
		}
	}
	printf("Set_current_texture_by_name notex\n");
}

bool Tile_in_group(MapTile* tile, const char* group_name)
{
	//try {
	//	printf(tile->mt->grp->name);
	//}
	//catch (...) {
	//	printf("err\n");
	//}
	if (!tile->mt->grp) {
		printf("\nx: %d z: %d err nogrp\n", tile->x, tile->z);
		return false;
	}
	auto name = tile->mt->grp->name;
	if (!(name[0] >= 'A' && name[0] <= 'Z'
		&& name[1] >= 'A' && name[1] <= 'Z'
		&& name[2] >= 'A' && name[2] <= 'Z'))
	{
		printf("\nx: %d z: %d err grp rubbish\n", tile->x, tile->z);
		return false;
	}
	return strcmp(name, group_name) == 0;
}

void Create_change_buffer()
{
	bufsize = mapwidth * mapheight;
	changebuf = (TileTexChange*)malloc(sizeof(TileTexChange) * bufsize);
	for (int i = 0; i < bufsize; ++i) {
		changebuf[i].tile_id = -1;
	}
}

void Free_change_buffer()
{
	free(changebuf);
}

void Apply_change_buffer(char* tex_grp)
{
	for (int i = 0; i < bufsize; ++i) {
		auto& change = changebuf[i];

		if (change.tile_id == -1) {
			continue;
		}

		auto* tile = &(maptiles[i]);
		auto tex = Get_texture(tex_grp, change.tile_id);
		if (!tex) {
			//printf("\nx: %d z: %d err notex\n", tile->x, tile->z);
			continue;
		}

		Set_current_texture_by_name(tex->grp->name);
		Set_curtex(tex);
		ChangeTileTexture(tile, tex);
		tile->mt = tex;
		tile->rot = change.rotation;
		tile->xflip = change.flipx;
		tile->zflip = change.flipz;
		//printf("x: %d z: %d tid: %d\n", tile->x, tile->z, tid);

		// Reset the buffer value.
		change.tile_id = -1;
	}
}

