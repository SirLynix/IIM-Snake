#pragma once

// Ce fichier contient une structure de tous les assets du jeu ainsi qu'une fonction pour les charger

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

struct Resources
{
	sf::Sprite apple;
	sf::Sprite snakeHead;
	sf::Sprite snakeBody;
	sf::Sprite snakeBodyCorner;
	sf::Sprite snakeTail;
	sf::Texture tiles;
};

// Charge toutes les ressources, renvoie true si elles ont pu toutes être chargées et false autrement
bool LoadResources(Resources& resources);