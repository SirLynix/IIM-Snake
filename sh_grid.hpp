#pragma once

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

	// Récupère le contenu d'une cellule de la grille à une position définie
	CellType GetCell(int x, int y) const;
	int GetHeight() const;
	int GetWidth() const;

	// Défini le contenu d'une cellule de la grille à une position définie
	void SetCell(int x, int y, CellType cellType);
	void SetupWalls();

protected:
	std::vector<CellType> m_content;
	int m_height;
	int m_width;
};
