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

#ifndef DEF_AUTO_TEX
#define DEF_AUTO_TEX



#include "global.h"



void ChangeTileTexture(MapTile* tile, MapTexture* newtex);
void SelectNextRandomMapTile();
void Set_curtexgrp(MapTextureGroup* __curtexgrp);
void Set_curtex(MapTexture* __curtex);
MapTexture* Get_curtex();

struct TexturingAction
{
	char action_type;
};

struct TextureLayerMain : TexturingAction
{
	char* group_name;
	int height_min;
	int height_max;
	int slope_min;
	int slope_max;
};

struct TextureLayerIntermediate : TexturingAction
{
	char* group_a;
	char* group_b;
	char* replacement;
};

struct TextureLayerFeathering : TexturingAction
{
	char* from;
	char* to;
	int border;
	int seek_radius;
	float probability;
	int iterations;
};

struct TextureLayerInner : TexturingAction
{
	char* parent_group_name;
	char* inner_group_name;
	int border_radius;
	int height_min;
	int height_max;
	int slope_min;
	int slope_max;
};

struct TextureLayerTransition
{
	char* from_group;
	char* to_group;
	char* transition_group;
	bool match_from;
};

struct ObjectDistribution
{
	char* tile_group;
	char* object_name;
	char* noise_name;
	float probability_min;
	float probability_max;
	float border_distance;
	int height_min;
	int height_max;
	int slope_min;
	int slope_max;
};

struct ObjectDistributionNoise
{
	char* noise_name;
	float frequency;
	float gain;
	float lacunarity;
	float weighted_strength;
	int octaves;
	int seed;
};

struct TileTexChange
{
	int tile_id;
	int rotation;
	bool flipx;
	bool flipz;
};




//extern GrowList<TextureLayerMain> main_layers;
//extern GrowList<TextureLayerIntermediate> replacements;
//extern GrowList<TextureLayerFeathering> feathers;
//extern GrowList<TextureLayerInner> inner_layers;

extern GrowList<TexturingAction*> actions;
extern GrowList<TextureLayerTransition> transitions;
extern GrowList<ObjectDistribution> distributions;
extern GrowList<ObjectDistributionNoise> noises;

extern int replacement_iterations;
extern int fix_seams_iterations;
extern int fix_seams_by_edges_iterations;
extern int fill_islands_min_neighbours;
extern float sun_factor;

extern float sqrt2;




void Texture_read_layer_files();
void Texture_cleanup();


extern float tileedgelen;
inline void Get_edge_slope(uchar a, uchar b, float& slope)
{
	float tempslope;
	tempslope = atan(abs(a * maphiscale - b * maphiscale) / tileedgelen);
	if (tempslope > slope) {
		slope = tempslope;
	}
}
inline void Get_diag_slope(uchar a, uchar b, float& slope)
{
	float tempslope;
	tempslope = atan(abs(a * maphiscale - b * maphiscale) / (tileedgelen * sqrt2));
	if (tempslope > slope) {
		slope = tempslope;
	}
}


inline MapTile* Get_tile(MapTile* from, int x, int z)
{
	int zz = from->z + z; int xx = from->x + x;
	if (zz < 0 || zz >= mapheight || xx < 0 || xx >= mapwidth) {
		return nullptr;
	}
	return &(maptiles[zz * mapwidth + xx]);
}
inline MapTile* Get_N_tile(const MapTile* from)
{
	return &(maptiles[(from->z - 1) * mapwidth + from->x]);
}
inline MapTile* Get_NE_tile(const MapTile* from)
{
	return &(maptiles[(from->z - 1) * mapwidth + from->x + 1]);
}
inline MapTile* Get_E_tile(const MapTile* from)
{
	return &(maptiles[from->z * mapwidth + from->x + 1]);
}
inline MapTile* Get_SE_tile(const MapTile* from)
{
	return &(maptiles[(from->z + 1) * mapwidth + from->x + 1]);
}
inline MapTile* Get_S_tile(const MapTile* from)
{
	return &(maptiles[(from->z + 1) * mapwidth + from->x]);
}
inline MapTile* Get_SW_tile(const MapTile* from)
{
	return &(maptiles[(from->z + 1) * mapwidth + from->x - 1]);
}
inline MapTile* Get_W_tile(const MapTile* from)
{
	return &(maptiles[from->z * mapwidth + from->x - 1]);
}
inline MapTile* Get_NW_tile(const MapTile* from)
{
	return &(maptiles[(from->z - 1) * mapwidth + from->x - 1]);
}

MapTextureGroup* Get_texture_group(const char* name);
MapTexture* Get_texture(const char* group_name, int id);
void Set_current_texture_by_name(const char* name);
bool Tile_in_group(const MapTile* tile, const char* group_name);



extern int bufsize;
extern TileTexChange* changebuf;

void Create_change_buffer();
void Free_change_buffer();
void Apply_change_buffer(char* tex_grp, char* alt = nullptr);



// ----------------------------------------------------------------------------
// Seams.
// ----------------------------------------------------------------------------

// Helpers.
inline void Shape_flip_x(uchar& s)
{
	s = (s & 0b10001000)
		| ((s & 0b01000000) >> 6) | ((s & 0b00000001) << 6)
		| ((s & 0b00100000) >> 4) | ((s & 0b00000010) << 4)
		| ((s & 0b00010000) >> 2) | ((s & 0b00000100) << 2);
}
inline void Shape_flip_z(uchar& s)
{
	s = (s & 0b00100010)
		| ((s & 0b10000000) >> 4) | ((s & 0b00001000) << 4)
		| ((s & 0b01000000) >> 2) | ((s & 0b00010000) << 2)
		| ((s & 0b00000100) >> 2) | ((s & 0b00000001) << 2);
}
inline void Shape_rotate(uchar& s)
{
	// this is wrong side, should rotate counter clockwise as it's the shape around the tile, not the tile
	//s = ((s & 0b11111100) >> 2) | ((s & 0b00000011) << 6);
	s = ((s & 0b11000000) >> 6) | ((s & 0b00111111) << 2);
}

// Place transition group.
void Explode_islands(TextureLayerTransition* transition);
void Explode_peninsulas(TextureLayerTransition* transition);
bool Fill_islands(TextureLayerTransition* transition);
bool Place_transitions(TextureLayerTransition* transition);
bool Prune_transitions(TextureLayerTransition* transition);

// Fit by tile type.
int Fit_tile_precise(uchar shape, uchar* shapes);
int Fit_tile_precise(uchar shape_to, uchar shape_from);
int Fit_tile_precise(uchar shape_to, uchar shape_from,
	bool match_from, int& rotation);
int Fit_tile(uchar shape, uchar* shapes);
int Fit_tile(uchar shape_to, uchar shape_from);
int Fit_tile(uchar shape_to, uchar shape_from,
	bool match_from, int& rotation);
int Fit_tile(uchar shape_to, uchar shape_from, bool match_from, bool precise,
	int& rotation, bool& flipx, bool& flipz);
double Get_shape_score(uchar tile, uchar stencil);
void Get_best_tile(uchar shape_to, uchar shape_from,
	int& tile, double& _score);
int Get_best_tile(uchar shape_to, uchar shape_from,
	int& rotation, bool& flipx, bool& flipz);

uchar Get_shape(MapTile* tile, char* group);
void Randomize_tile(TileTexChange* t);
bool Match_tiles_by_tile_type(TextureLayerTransition* transition);

// Fit by edges.
bool Match_tiles_by_edges(TextureLayerTransition* transition);

#endif
