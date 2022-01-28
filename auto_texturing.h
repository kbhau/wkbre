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

struct TextureLayerMain
{
	char* group_name;
	int height_min;
	int height_max;
	int slope_min;
	int slope_max;
};

struct TextureLayerIntermediate
{
	char* group_a;
	char* group_b;
	char* replacement;
};

struct TextureLayerFeathering
{
	char* from;
	char* to;
	int border;
	int seek_radius;
	float probability;
	int iterations;
};

struct TextureLayerInner
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
	float probability;
	float border_distance;
	int height_min;
	int height_max;
	int slope_min;
	int slope_max;
};

struct TileTexChange
{
	int tile_id;
	int rotation;
	bool flipx;
	bool flipz;
};




extern GrowList<TextureLayerMain> main_layers;
extern GrowList<TextureLayerIntermediate> replacements;
extern GrowList<TextureLayerFeathering> feathers;
extern GrowList<TextureLayerInner> inner_layers;
extern GrowList<TextureLayerTransition> transitions;
extern GrowList<ObjectDistribution> distributions;

extern int replacement_iterations;
extern int fix_seams_iterations;
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
inline MapTile* Get_N_tile(MapTile* from)
{
	return &(maptiles[(from->z - 1) * mapwidth + from->x]);
}
inline MapTile* Get_NE_tile(MapTile* from)
{
	return &(maptiles[(from->z - 1) * mapwidth + from->x + 1]);
}
inline MapTile* Get_E_tile(MapTile* from)
{
	return &(maptiles[from->z * mapwidth + from->x + 1]);
}
inline MapTile* Get_SE_tile(MapTile* from)
{
	return &(maptiles[(from->z + 1) * mapwidth + from->x + 1]);
}
inline MapTile* Get_S_tile(MapTile* from)
{
	return &(maptiles[(from->z + 1) * mapwidth + from->x]);
}
inline MapTile* Get_SW_tile(MapTile* from)
{
	return &(maptiles[(from->z + 1) * mapwidth + from->x - 1]);
}
inline MapTile* Get_W_tile(MapTile* from)
{
	return &(maptiles[from->z * mapwidth + from->x - 1]);
}
inline MapTile* Get_NW_tile(MapTile* from)
{
	return &(maptiles[(from->z - 1) * mapwidth + from->x - 1]);
}

MapTextureGroup* Get_texture_group(const char* name);
MapTexture* Get_texture(const char* group_name, int id);
void Set_current_texture_by_name(const char* name);
bool Tile_in_group(MapTile* tile, const char* group_name);



extern int bufsize;
extern TileTexChange* changebuf;

void Create_change_buffer();
void Free_change_buffer();
void Apply_change_buffer(char* tex_grp, char* alt = nullptr);



#endif
