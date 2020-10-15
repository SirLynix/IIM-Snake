#pragma once

#include <vector>

// Une enum class est comme une enum en C++ classique, � l'exception du fait qu'il est obligatoire d'�crire le nom de l'enum
// pour acc�der � ses �l�ments (CellType::Apple plut�t que juste Apple), et qu'il n'est pas possible de convertir implicitement
// la valeur en entier (il suffit d'un static_cast pour cela).

enum class CellType
{
	Apple,
	Wall,
	None
};

// La classe grid repr�sente les �l�ments immobiles du terrain, comme les pommes et les murs, dans une grille d'une certaine taille
class Grid
{
public:
	Grid(int width, int height);

	// R�cup�re le contenu d'une cellule de la grille � une position d�finie
	CellType GetCell(int x, int y) const;
	int GetHeight() const;
	int GetWidth() const;

	// D�fini le contenu d'une cellule de la grille � une position d�finie
	void SetCell(int x, int y, CellType cellType);
	void SetupWalls();

protected:
	std::vector<CellType> m_content;
	int m_height;
	int m_width;
};
