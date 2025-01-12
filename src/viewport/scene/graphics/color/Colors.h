#pragma once

#include "Color.h"

// Predefined colors
namespace Colors {
	const auto WHITE					= Color(255, 255, 255);
	const auto BLACK					= Color(  0,   0,   0);

	const auto RED						= Color(255,   0,   0);
	const auto YELLOW					= Color(255, 255,   0);
	const auto GREEN					= Color(  0, 255,   0);
	const auto CYAN 					= Color(  0, 255, 255);
	const auto BLUE						= Color(  0,   0, 255);
	const auto MAGENTA					= Color(255,   0, 255);

	const auto BG_COLOR					= Color( 60,  60,  60);
	const auto GRID_COLOR				= Color( 80,  80,  80);
	const auto MESH_FACE_COLOR			= Color(220, 220, 220);
	const auto MESH_EDGE_COLOR			= Color( 50,  50,  50);
	const auto MESH_VERT_COLOR			= Color( 25,  25,  25);
	const auto MESH_SELECT_COLOR		= Color(230, 140,  70);
	const auto RAY_COLOR				= YELLOW;
	const auto TEXT_COLOR				= Color(192, 192, 192);
	const auto LIGHT_SUN				= Color(230, 230, 230);
	const auto LIGHT_AMBIENT			= Color( 13,  13,  13);
	const auto SKY						= Color(128, 128, 128);
}
