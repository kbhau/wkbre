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

#include "../global.h"

ITile *itiles = 0;
int numTilesX, numTilesZ, numTilesS;
DynList<goref> oobobjs;

void InitITiles()
{
	numTilesX = mapwidth - mapedge*2;
	numTilesZ = mapheight - mapedge*2;
	numTilesS = numTilesX * numTilesZ;
	itiles = new ITile[numTilesS];
}

void PutObjsInITiles(GameObject *o)
{
	if(!itiles) return;
	for(DynListEntry<GameObject> *e = o->children.first; e; e = e->next)
		PutObjsInITiles(&e->value);
	int tx = o->position.x / 5, tz = o->position.z / 5;
	if((tz < 0) || (tz >= numTilesZ) || (tx < 0) || (tx >= numTilesX))
	{
		oobobjs.add();
		oobobjs.last->value = o;
		o->itile = 0; o->itileole = oobobjs.last;
	}
	else
	{
		ITile *t = &(itiles[tz*numTilesX+tx]);
		t->objs.add();
		t->objs.last->value = o;
		o->itile = t; o->itileole = t->objs.last;
	}
}

void ResetITiles()
{
	if(!itiles) return;
	for(int i = 0; i < numTilesS; i++)
		itiles[i].objs.clear();
	PutObjsInITiles(levelobj);
}

void GOPosChanged(GameObject *o)
{
	if(!itiles) return;
	int x, z;
	x = o->position.x / 5; z = o->position.z / 5;
	if((z < 0) || (z >= numTilesZ) || (x < 0) || (x >= numTilesX))
	{
		if(o->itile)
		{
			o->itile->objs.move(o->itileole, &oobobjs);
			o->itile = 0;
		}
		return;
	}
	ITile *nt = &(itiles[z*numTilesX+x]);
	if(nt != o->itile)
	{
		if(o->itile)
			o->itile->objs.move(o->itileole, &nt->objs);
		else
			oobobjs.move(o->itileole, &nt->objs);
		o->itile = nt;
	}
}

void ListObjsInTiles(int tx, int tz, int l, int w, GrowList<goref> *r)
{
	for(int z = tz; z < (tz+w); z++)
	for(int x = tx; x < (tx+l); x++)
	{
		if((x < 0) || (x >= numTilesX) || (z < 0) || (z >= numTilesZ))
			continue;
		ITile *t = &(itiles[z*numTilesX+x]);
		for(DynListEntry<goref> *e = t->objs.first; e; e = e->next)
			r->add(e->value);
	}
}
