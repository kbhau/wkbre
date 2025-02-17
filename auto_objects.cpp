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


#include "FastNoiseLite.h"
#include "auto_texturing.h"

float npower = 1.0;

auto Get_noise(char* noise_name)
{
	ObjectDistributionNoise* pars = nullptr;
	for (int i = 0; i < noises.len; ++i) {
		if (strcmp(noises[i].noise_name, noise_name) == 0) {
			pars = &noises[i];
			break;
		}
	}
	if (!pars) {
		printf("Could not find noise [%s]\n", noise_name);
		auto np = std::unique_ptr<FastNoiseLite>();
		np = nullptr;
		return np;
	}

	auto n = std::make_unique<FastNoiseLite>(pars->seed);

	n->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	n->SetFractalType(FastNoiseLite::FractalType_FBm);
	n->SetFrequency(pars->frequency);
	n->SetFractalOctaves(pars->octaves);
	n->SetFractalLacunarity(pars->lacunarity);
	n->SetFractalGain(pars->gain);
	n->SetFractalWeightedStrength(pars->weighted_strength);
	npower = pars->power;

	return n;
}


double Get_probability(
	FastNoiseLite* noise,
	float probability_min,
	float probability_max,
	float power,
	int x,
	int z
) {
	auto res = (noise->GetNoise((double)x, (double)z) + 1.f) / 2.f;
	res = pow(min(1.0, max(0.0, res)), power);
	return res * (probability_max - probability_min) + probability_min;
}



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



void Remove_distributable_objects()
{
	int d_i;
	ObjectDistribution* d;
	//printf("Num distributions=[%d]", distributions.len);
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
		//printf("Removing [%s]\n", d->object_name);
		RemoveObjOfType2(levelobj, def);
	}
}



void Remove_distributable_objects_menu()
{
	// Reads the config file.
	Texture_read_layer_files();

	Remove_distributable_objects();

	// Cleans the config memory.
	Texture_cleanup();
}



void Distribute_objects(bool pre_clean)
{
	// Object placement offset - the map is offset when w =/= h.
	float xoffset = 0.f;
	float zoffset = 0.f;
	if (mapwidth < mapheight) {
		xoffset = (mapheight - mapwidth);
		//printf("xoffset=[%f]\n", xoffset);
	} else if (mapwidth > mapheight) {
		zoffset = (mapwidth - mapheight);
		//printf("zoffset=[%f]\n", zoffset);
	}

	// Reads the config file.
	Texture_read_layer_files();
	Create_occupation_buffer();

	// Pre-clean.
	if (pre_clean) {
		Remove_distributable_objects();
	}

	int tile_i;
	int d_i;
	ObjectDistribution* d;

	// Find parent object.
	GameObject* neutral_player = nullptr;
	for (DynListEntry<GameObject>* e = levelobj->children.first; e; e = e->next) {
		if (e->value.objdef->type == CLASS_PLAYER
			&& wcscmp(e->value.name, L"Neutral Player") == 0)
		{
			neutral_player = &e->value;
			break;
		}
	}
	if (neutral_player == nullptr) {
		printf("Distribute Object: Could not find neutral player!\n");
		return;
	}

	// Loop through distributions.
	for (int di = 0; di < distributions.len; ++di) {
		d = &distributions[di];
		auto n = Get_noise(d->noise_name);
		if (!n) {
			continue;
		}

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
		for (int tz = mapedge; tz < mapheight - mapedge; ++tz)
		for (int tx = mapedge; tx < mapwidth - mapedge; ++tx)
		{
			tile_i = tz * mapwidth + tx;

			// Skip if tile occupied.
			if (occbuf[tile_i]) {
				continue;
			}

			// Check tile group and set probability.
			auto* tile = &(maptiles[tile_i]);
			float prob = 0.f;
			if (Tile_in_group(tile, d->tile_group)) {
				prob = Get_probability(
					n.get(),
					d->probability_min,
					d->probability_max,
					npower,
					tx,
					tz
				);
			} else if (Tile_in_transition_group(tile, d->tile_group)) {
				prob = Get_probability(
					n.get(),
					d->probability_min / 2.f,
					d->probability_max / 2.f,
					npower,
					tx,
					tz
				);
			} else {
				continue;
			}

			// Check tile properties.
			float slope = 0;
			float height = 0;
			int j = tz * (mapwidth) + tx;
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
			if (height < d->height_min || height >= d->height_max
				|| slope < d->slope_min || slope >= d->slope_max)
			{
				continue;
			}
			/*if (double(rand()) / RAND_MAX < 0.01) {
				printf("Slope=[%f] Prob=[%f]\n", slope, prob);
			}*/

			// Create the object.
			if (double(rand()) / RAND_MAX < prob) {
				
				float innerrange = 1.0 - 2.0 * d->border_distance;
				float tileedge = 5.0;
				GameObject* o = CreateObject(def, neutral_player);
				o->position.x = (tx - mapedge) + xoffset + d->border_distance
					+ (double(rand()) / RAND_MAX) * innerrange;
				o->position.z = (mapwidth - tz - mapedge) - zoffset - d->border_distance
					- (double(rand()) / RAND_MAX) * innerrange;
				o->position.x *= tileedge;
				o->position.z *= tileedge;
				o->position.y = GetHeight(o->position.x, o->position.z);
				o->orientation = Vector3(0, double(rand() % 360) / 57.295779513, 0);

				// Mark tile as occupied.
				if (tile_i >= bufsize) {
					printf("ERROR: tile_i=[%d] bufsize=[%d]\n", tile_i, bufsize);
				}
				occbuf[tile_i] = true;
			}
		}
	} // distributions

	// Cleans the config memory.
	Free_occupation_buffer();
	Texture_cleanup();
}
