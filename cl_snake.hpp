#pragma once

#include "sh_snake.hpp"
#include "cl_resources.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

// La classe Snake repr�sente un serpent en jeu, ainsi que toutes ses pi�ces
// celui-ci poss�de toujours une taille de trois � l'apparition, et peut grandir,
// se d�placer dans une direction pr�cise, et r�apparaitre en remettant sa taille � z�ro

class ClientSnake : public Snake
{
public:
	using Snake::Snake;

	// G�re l'affichage du serpent (ainsi que l'orientation des sprites qui le composent en fonction de la direction de chaque partie du corps)
	void Draw(sf::RenderTarget& renderTarget, Resources& resources) const;
};
