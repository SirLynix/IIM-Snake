#pragma once

#include "cl_resources.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>

// Une enum class est comme une enum en C++ classique, à l'exception du fait qu'il est obligatoire d'écrire le nom de l'enum
// pour accéder à ses éléments (CellType::Apple plutôt que juste Apple), et qu'il n'est pas possible de convertir implicitement
// la valeur en entier (il suffit d'un static_cast pour cela).

enum class CellType
{
	Apple,
	Wall,
	None
};

// La classe grid représente les éléments immobiles du terrain, comme les pommes et les murs, dans une grille d'une certaine taille
class Grid
{
public:
	Grid(int width, int height);

	// Affiche le contenu de la grille
	void Draw(sf::RenderTarget& renderTarget, Resources& resources) const;

	// Récupère le contenu d'une cellule de la grille à une position définie
	CellType GetCell(int x, int y) const;

	// Défini le contenu d'une cellule de la grille à une position définie
	void SetCell(int x, int y, CellType cellType);

private:
	std::vector<CellType> m_content;
	int m_height;
	int m_width;
};
