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


// ----------------------------------------------------------------------------
// VARS
// ----------------------------------------------------------------------------

GrowList<TextureLayerMain> main_layers;
GrowList<TextureLayerIntermediate> replacements;
GrowList<TextureLayerInner> inner_layers;
GrowList<TextureLayerTransition> transitions;
GrowList<TextureLayerFeathering> feathers;
GrowList<ObjectDistribution> distributions;

int replacement_iterations = 1;
int fix_seams_iterations = 1;
int fill_islands_min_neighbours = 3;
int fix_seams_by_edges_iterations = 4;

float sqrt2 = sqrt(2);

int bufsize = 0;
TileTexChange* changebuf;

float tileedgelen = 5;



// ----------------------------------------------------------------------------
// ENUMS
// ----------------------------------------------------------------------------

enum Tlf_mode_enum {
	VARS = 0,
	VARSF,
	VARS2,
	LAYERS,
	FEATHERING,
	INNERS,
	TRANSITIONS,
	REPLACEMENTS,
	DISTRIBUTIONS
};



// ----------------------------------------------------------------------------
// FUNCS
// ----------------------------------------------------------------------------

void Texture_read_layer_files()
{
	// Try to open file.
	auto* f = fopen("redata\\texture_layers.tlf", "r");

	// No file, no fun.
	if (!f) {
		return;
	}

	// Set vars, allocate resources.
	auto buf = 200;
	auto namelen = 50;
	char* line = (char*)malloc(sizeof(char) * buf);
	int mode = -1; // -1 = undefined, 0 = vars, 1 = layers, 2 = inners, 3 = transitions
	GrowStringList varsk;
	GrowList<int> varsv;
	GrowStringList fvarsk;
	GrowList<float> fvarsv;

	auto getval = [&varsk, &varsv](char* token)->int {
		for (int i = 0; i < varsk.len; ++i) {
			if (strcmp(token, varsk.getdp(i)) == 0) {
				return varsv[i];
			}
		}
		return atoi(token);
	};
	auto getvalf = [&fvarsk, &fvarsv](char* token)->float
	{
		for (int i = 0; i < fvarsk.len; ++i) {
			if (strcmp(token, fvarsk.getdp(i)) == 0) {
				return fvarsv[i];
			}
		}
		return atof(token);
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
				mode = VARS;
			} else if (strcmp(line, "!VARSF\n") == 0) {
				mode = VARSF;
			} else if (strcmp(line, "!VARS2\n") == 0) {
				mode = VARS2;
			} else if (strcmp(line, "!LAYERS\n") == 0) {
				mode = LAYERS;
			} else if (strcmp(line, "!INNERS\n") == 0) {
				mode = INNERS;
			} else if (strcmp(line, "!FEATHERING\n") == 0) {
				mode = FEATHERING;
			} else if (strcmp(line, "!TRANSITIONS\n") == 0) {
				mode = TRANSITIONS;
			} else if (strcmp(line, "!REPLACEMENTS\n") == 0) {
				mode = REPLACEMENTS;
			} else if (strcmp(line, "!DISTRIBUTIONS\n") == 0) {
				mode = DISTRIBUTIONS;
			} else {
				mode = -1;
			}
			continue;
		}

		// Read vars.
		if (mode == VARS) {
			varsk.add(strtok(line, "="));
			varsv.add(getval(strtok(nullptr, "\n")));
			continue;
		}

		// Read vars.
		if (mode == VARSF) {
			fvarsk.add(strtok(line, "="));
			fvarsv.add(getvalf(strtok(nullptr, "\n")));
			continue;
		}

		// Read vars 2.
		if (mode == VARS2) {
			char* token = strtok(line, "=");
			if (strcmp(token, "REPLACEMENT_ITERATIONS") == 0) {
				replacement_iterations = getval(strtok(nullptr, "\n"));
			} else if (strcmp(token, "FIX_SEAMS_ITERATIONS") == 0) {
				fix_seams_iterations = getval(strtok(nullptr, "\n"));
			} else if (strcmp(token, "FILL_ISLANDS_MIN_NEIGHBOURS") == 0) {
				fill_islands_min_neighbours = getval(strtok(nullptr, "\n"));
			}
			continue;
		}

		// Read layers.
		if (mode == LAYERS) {
			char* name = (char*)malloc(sizeof(char) * namelen); strcpy(name, strtok(line, ","));
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

		// Read inners.
		if (mode == INNERS) {
			char* parent_name = (char*)malloc(sizeof(char) * namelen); strcpy(parent_name, strtok(line, ","));
			char* inner_name = (char*)malloc(sizeof(char) * namelen); strcpy(inner_name, strtok(nullptr, ","));
			int border_radius = getval(strtok(nullptr, ","));
			int height_min = getval(strtok(nullptr, ","));
			int height_max = getval(strtok(nullptr, ","));
			int slope_min = getval(strtok(nullptr, ","));
			int slope_max = getval(strtok(nullptr, "\n"));
			inner_layers.add(TextureLayerInner{
				parent_name,
				inner_name,
				border_radius,
				height_min,
				height_max,
				slope_min,
				slope_max
			});
			continue;
		}

		// Read knots.
		if (mode == FEATHERING) {
			char* from = (char*)malloc(sizeof(char) * namelen); strcpy(from, strtok(line, ","));
			char* to = (char*)malloc(sizeof(char) * namelen); strcpy(to, strtok(nullptr, ","));
			int border = getval(strtok(nullptr, ","));
			int seek_radius = getval(strtok(nullptr, ","));
			float prob = getvalf(strtok(nullptr, ","));
			int iterations = getval(strtok(nullptr, "\n"));
			feathers.add(TextureLayerFeathering{
				from,
				to,
				border,
				seek_radius,
				prob,
				iterations
			});
			continue;
		}

		// Read replacements.
		if (mode == REPLACEMENTS) {
			char* grp_a = (char*)malloc(sizeof(char) * namelen); strcpy(grp_a, strtok(line, ","));
			char* grp_b = (char*)malloc(sizeof(char) * namelen); strcpy(grp_b, strtok(nullptr, ","));
			char* repl = (char*)malloc(sizeof(char) * namelen); strcpy(repl, strtok(nullptr, "\n"));
			replacements.add(TextureLayerIntermediate{
				grp_a,
				grp_b,
				repl
			});
			continue;
		}

		// Read transitions.
		if (mode == TRANSITIONS) {
			char* trans_grp = (char*)malloc(sizeof(char) * namelen); strcpy(trans_grp, strtok(line, ","));
			char* from_grp = (char*)malloc(sizeof(char) * namelen); strcpy(from_grp, strtok(nullptr, ","));
			char* to_grp = (char*)malloc(sizeof(char) * namelen); strcpy(to_grp, strtok(nullptr, ","));
			char* match_from = (char*)malloc(sizeof(char) * namelen); strcpy(match_from, strtok(nullptr, "\n"));
			transitions.add(TextureLayerTransition{
				from_grp,
				to_grp,
				trans_grp,
				match_from[0] == 'T'
			});
			free(match_from);
			continue;
		}

		// Read distributions.
		if (mode == DISTRIBUTIONS) {
			char* tile_grp = (char*)malloc(sizeof(char) * namelen); strcpy(tile_grp, strtok(line, ","));
			char* obj_name = (char*)malloc(sizeof(char) * namelen); strcpy(obj_name, strtok(nullptr, ","));
			float prob = getvalf(strtok(nullptr, ","));
			float border = getvalf(strtok(nullptr, ","));
			int height_min = getval(strtok(nullptr, ","));
			int height_max = getval(strtok(nullptr, ","));
			int slope_min = getval(strtok(nullptr, ","));
			int slope_max = getval(strtok(nullptr, "\n"));
			distributions.add(ObjectDistribution{
				tile_grp,
				obj_name,
				prob,
				border,
				height_min,
				height_max,
				slope_min,
				slope_max
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
		//printf("M:%s,%d,%d,%d,%d\n", it.group_name, it.height_min, it.height_max, it.slope_min, it.slope_max);
	}
	for (int i = 0; i < replacements.len; ++i) {
		auto& it = replacements[i];
		//printf("R:%s,%s,%s\n", it.group_a, it.group_b, it.replacement);
	}
	for (int i = 0; i < feathers.len; ++i) {
		auto& it = feathers[i];
		//printf("K:%s\n", it.group_name);
	}
	for (int i = 0; i < inner_layers.len; ++i) {
		auto& it = inner_layers[i];
		//printf("I:%s,%s,%d,%d,%d,%d,%d\n", it.parent_group_name, it.inner_group_name, it.border_radius, it.height_min, it.height_max, it.slope_min, it.slope_max);
	}
	for (int i = 0; i < transitions.len; ++i) {
		auto& it = transitions[i];
		//printf("T:%s,%s,%s,%d\n", it.transition_group, it.from_group, it.to_group, it.match_from);
	}
	for (int i = 0; i < distributions.len; ++i) {
		auto& it = distributions[i];
		//printf("T:%s,%s,%f,%f\n", it.tile_group, it.object_name, it.probability, it.border_distance);
	}
	//printf("replacement iterations: %d\n", replacement_iterations);
	//printf("knot iterations: %d\n", knot_iterations);
	//printf("fix seams iterations: %d\n", fix_seams_iterations);
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

	for (int i = 0; i < feathers.len; ++i) {
		free(feathers[i].from);
		free(feathers[i].to);
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

	for (int i = 0; i < distributions.len; ++i) {
		free(distributions[i].tile_group);
		free(distributions[i].object_name);
	}

	main_layers.clear();
	replacements.clear();
	feathers.clear();
	inner_layers.clear();
	transitions.clear();
	distributions.clear();
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
		//printf("nogrp: %s\n", group_name);
		return nullptr;
	}
	if (group->tex->len <= id) {
		//printf("notex: %s: %d\n", group_name, id);
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
	//printf("Set_current_texture_by_name notex\n");
}

bool Tile_in_group(const MapTile* tile, const char* group_name)
{
	//try {
	//	printf(tile->mt->grp->name);
	//}
	//catch (...) {
	//	printf("err\n");
	//}
	if (!tile->mt->grp) {
		//printf("\nx: %d z: %d err nogrp\n", tile->x, tile->z);
		return false;
	}
	auto name = tile->mt->grp->name;
	//if (!(name[0] >= 'A' && name[0] <= 'Z'
	//	&& name[1] >= 'A' && name[1] <= 'Z'
	//	&& name[2] >= 'A' && name[2] <= 'Z'))
	//{
	//	//printf("\nx: %d z: %d err grp rubbish\n", tile->x, tile->z);
	//	return false;
	//}
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

void Apply_change_buffer(char* tex_grp, char* alt)
{
	for (int i = 0; i < bufsize; ++i) {
		auto& change = changebuf[i];

		if (change.tile_id == -1) {
			continue;
		}

		char* grp = tex_grp;
		if (change.tile_id == -2) {
			if (alt == nullptr) {
				alt = tex_grp;
			}
			grp = alt;
		}

		auto* tile = &(maptiles[i]);
		auto tex = Get_texture(grp, change.tile_id);
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



// ----------------------------------------------------------------------------
// TODO
// ----------------------------------------------------------------------------

/*

x feather edges - iterations control per line

- fix seams - edges pass
	- split cpp

- fix seams - better iteration control
- noise samplers
- object distribution noise sampled probability

*/ 
