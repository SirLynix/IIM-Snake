#pragma once

#include "sh_snake.hpp"
#include "cl_resources.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

// La classe Snake représente un serpent en jeu, ainsi que toutes ses pièces
// celui-ci possède toujours une taille de trois à l'apparition, et peut grandir,
// se déplacer dans une direction précise, et réapparaitre en remettant sa taille à zéro

class ClientSnake : public Snake
{
public:
	using Snake::Snake;

	// Gère l'affichage du serpent (ainsi que l'orientation des sprites qui le composent en fonction de la direction de chaque partie du corps)
	void Draw(sf::RenderTarget& renderTarget, Resources& resources) const;
};
