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


void Apply_main_layer(TextureLayerMain& layer);
void Apply_inner_layer(TextureLayerInner& layer);
void Feather_seams(TextureLayerFeathering& layer);
void Apply_replacement(TextureLayerIntermediate& layer);


void Texture_run_script()
{
	Texture_read_layer_files();
	Create_change_buffer();

	for (int i = 0; i < actions.len; ++i) {
		printf("%c", actions[i]->action_type);
		switch (actions[i]->action_type) {
		case 'M':
		{
			auto layer = ((TextureLayerMain*)actions[i]);
			Apply_main_layer(*layer);
		}
		break;
		case 'I':
		{
			auto layer = ((TextureLayerInner*)actions[i]);
			Apply_inner_layer(*layer);
		}
		break;
		case 'F':
		{
			auto layer = ((TextureLayerFeathering*)actions[i]);
			Feather_seams(*layer);
		}
		break;
		case 'R':
		{
			auto layer = ((TextureLayerIntermediate*)actions[i]);
			Apply_replacement(*layer);
		}
		break;
		}
	}

	Free_change_buffer();
	Texture_cleanup();
}
