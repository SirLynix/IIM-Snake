#pragma once

#include <cstdint>

// Comme sf::Color fait partie de SFML-Graphics, nous ne pouvons pas l'utiliser c�t� serveur. D�finissons notre propre structure � la place
struct Color
{
	std::uint8_t r, g, b;
};