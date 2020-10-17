#pragma once

#include <cstdint>

// Comme sf::Color fait partie de SFML-Graphics, nous ne pouvons pas l'utiliser côté serveur. Définissons notre propre structure à la place
struct Color
{
	std::uint8_t r, g, b;
};