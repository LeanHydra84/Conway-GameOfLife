#ifndef _RENDERDATA_HPP_
#define _RENDERDATA_HPP_

#include <allegro5/allegro_color.h>


struct RenderData
{
	size_t Width;
	size_t Height;
	ALLEGRO_COLOR ActiveColor;
	ALLEGRO_COLOR InactiveColor;
};

#endif