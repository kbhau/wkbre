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



void Texture_fix_seams()
{
	Texture_read_layer_files();
	Create_change_buffer();


	for (int t = 0; t < transitions.len; ++t) {
		auto* transition = &(transitions[t]);

		Explode_islands(transition);
		Explode_peninsulas(transition);
		Fill_islands(transition);
		Place_transitions(transition);
		//Prune_transitions(transition);

		Match_tiles_by_edges(transition);
	}


	Free_change_buffer();
	Texture_cleanup();
}



// Use after finishing all main layers to place texture transistions.
void Texture_fix_seams_()
{
	int i;
	bool ok = false;
	int counter = 0;

	Texture_read_layer_files();
	Create_change_buffer();

	printf("fix seams begin\n");

	while (!ok) {
		ok = true;

		for (int t = 0; t < transitions.len; ++t)
		{
			auto* transition = &(transitions[t]);
			ok &= Fill_islands(transition);
		}
		printf("fill\n");

		for (int t = 0; t < transitions.len; ++t)
		{
			auto* transition = &(transitions[t]);
			ok &= Place_transitions(transition);
		}
		printf("match 1\n");

		for (int t = 0; t < transitions.len; ++t)
		{
			auto* transition = &(transitions[t]);
			ok &= Prune_transitions(transition);
		}
		printf("prune\n");	
			
		if (++counter >= fix_seams_iterations) {
			printf("fix seams max iterations reached\n");
			break;
		}
	}

	for (int t = 0; t < transitions.len; ++t)
	{
		auto* transition = &(transitions[t]);
		ok &= Match_tiles_by_tile_type(transition);
	}
	printf("match 2\n");

	printf("fix seams end\n");

	Free_change_buffer();
	Texture_cleanup();

	// Loop over all tiles - match corners - ID 8, 9 - needed?
}

// todo - match on transition tiles instead
// todo - match on all things ?
// todo - if can't find both shapes then match on tex only
// todo - fix mismatchy tiles - maybe another set for fixes


