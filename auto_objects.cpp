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



// Easier to copy this than expose it...
void RemoveObjOfType2(GameObject* o, CObjectDefinition* d)
{
	DynListEntry<GameObject>* nx;
	for (DynListEntry<GameObject>* e = o->children.first; e; e = nx) {
		nx = e->next;
		RemoveObjOfType2(&e->value, d);
	}
	if (o->objdef == d)
		RemoveObject(o);
}



bool Tile_in_transition_group(MapTile* tile, const char* group_name)
{
	if (!tile->mt->grp) {
		return false;
	}
	auto name = tile->mt->grp->name;

	return strstr(name, group_name) != nullptr;
}



void Remove_distribution_objects()
{
	int d_i;
	ObjectDistribution* d;
	for (int di = 0; di < distributions.len; ++di) {
		d = &distributions[di];

		// Find object definition.
		CObjectDefinition* def = nullptr;
		int def_i = -1;
		for (int i = 0; i < CLASS_NUM; ++i) {
			def_i = FindObjDef(i, d->object_name);
			if (def_i != -1) {
				break;
			}
		}
		if (def_i == -1) {
			printf("Distribute Object: Could not remove object [%s]\n", d->object_name);
			continue;
		}
		def = &objdef[def_i];
		RemoveObjOfType2(levelobj, def);
	}
}



void Distribute_objects()
{
	// Reads the config file.
	Texture_read_layer_files();

	// Pre-clean.
	Remove_distribution_objects();

	int tile_i;
	int d_i;
	ObjectDistribution* d;

	// Find parent object.
	GameObject* neutral_player = nullptr;
	for (DynListEntry<GameObject>* e = levelobj->children.first; e; e = e->next)
	if (e->value.objdef->type == CLASS_PLAYER
		&& wcscmp(e->value.name, L"Neutral Player") == 0)
	{
		neutral_player = &e->value;
		break;
	}
	if (neutral_player == nullptr) {
		printf("Distribute Object: Could not find neutral player!\n");
		return;
	}

	// Loop through distributions.
	for (int di = 0; di < distributions.len; ++di) {
		d = &distributions[di];

		// Find object definition.
		CObjectDefinition* def = nullptr;
		int def_i = -1;
		for (int i = 0; i < CLASS_NUM; ++i) {
			def_i = FindObjDef(i, d->object_name);
			if (def_i != -1) {
				break;
			}
		}
		if (def_i == -1) {
			printf("Distribute Object: Could not find object [%s]\n", d->object_name);
			continue;
		}
		def = &objdef[def_i];

		/*for (int i = 0; i < typeObjDef.len; ++i) {
			strObjDef.getdp(i), 

			printf("[%s]\n", objdef[i].name);
			if (strcmp(d->object_name, objdef[i].name) == 0) {
				CObjectDefinition* def = &objdef[i];
				break;
			}
		}*/
		

		// Distribute this object.
		for (int tz = mapedge; tz < mapwidth - 1 - mapedge; ++tz)
		for (int tx = mapedge; tx < mapheight - 1 - mapedge; ++tx)
		{
			// Check tle group and set probability.
			tile_i = tz * mapwidth + tx;
			auto* tile = &(maptiles[tile_i]);
			float prob = 0.f;
			if (Tile_in_group(tile, d->tile_group)) {
				prob = d->probability;
			} else if (Tile_in_transition_group(tile, d->tile_group)) {
				prob = d->probability / 2.f;
			} else {
				continue;
			}

			// Check tile properties.
			float slope = 0;
			float height = 0;
			int j = tz * (mapwidth + 1) + tx;
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
			if (height < d->height_min && height >= d->height_max
				&& slope < d->slope_min && slope >= d->slope_max)
			{
				continue;
			}

			// Create the object.
			if (double(rand()) / RAND_MAX < prob) {
				float tileedge = 5.0;
				GameObject* o = CreateObject(def, neutral_player);
				o->position.x = (tx - mapedge) + d->border_distance
					+ (double(rand()) / RAND_MAX) * (1.0 - 2.0 * d->border_distance);
				o->position.z = (mapwidth - tz - mapedge) - d->border_distance
					- (double(rand()) / RAND_MAX) * (1.0 - 2.0 * d->border_distance);
				o->position.x *= tileedge;
				o->position.z *= tileedge;
				o->position.y = GetHeight(o->position.x, o->position.z);
				o->orientation = Vector3(0, double(rand() % 360) / 57.295779513, 0);
			}
		}
	} // distributions

	// Cleans the config memory.
	Texture_cleanup();
}
