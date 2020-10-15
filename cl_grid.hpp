#pragma once

#include "sh_grid.hpp"
#include "cl_resources.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

// Une enum class est comme une enum en C++ classique, � l'exception du fait qu'il est obligatoire d'�crire le nom de l'enum
// pour acc�der � ses �l�ments (CellType::Apple plut�t que juste Apple), et qu'il n'est pas possible de convertir implicitement
// la valeur en entier (il suffit d'un static_cast pour cela).

// La classe grid repr�sente les �l�ments immobiles du terrain, comme les pommes et les murs, dans une grille d'une certaine taille
class ClientGrid : public Grid
{
public:
	using Grid::Grid;

	// Affiche le contenu de la grille
	void Draw(sf::RenderTarget& renderTarget, Resources& resources) const;
};
