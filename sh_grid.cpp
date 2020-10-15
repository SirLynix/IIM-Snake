#include "sh_grid.hpp"
#include "sh_constants.hpp"
#include <cassert>

Grid::Grid(int width, int height) :
m_height(height),
m_width(width)
{
	// On change la taille du vecteur pour qu'il stocke suffisamment de cellules
	// et on les initialise par défaut à None
	m_content.resize(width * height, CellType::None);
}

CellType Grid::GetCell(int x, int y) const
{
	assert(x >= 0 && x < m_width);
	assert(y >= 0 && y < m_height);

	return m_content[y * m_width + x];
}

int Grid::GetHeight() const
{
	return m_height;
}

int Grid::GetWidth() const
{
	return m_width;
}

void Grid::SetCell(int x, int y, CellType cellType)
{
	assert(x >= 0 && x < m_width);
	assert(y >= 0 && y < m_height);

	m_content[y * m_width + x] = cellType;
}

void Grid::SetupWalls()
{
	// On rajoute des murs en haut
	for (int x = 0; x < m_width; ++x)
		SetCell(x, 0, CellType::Wall);

	// en bas...
	for (int x = 0; x < m_width; ++x)
		SetCell(x, m_height - 1, CellType::Wall);

	// à gauche...
	for (int y = 0; y < m_height; ++y)
		SetCell(0, y, CellType::Wall);

	// à droite...
	for (int y = 0; y < m_height; ++y)
		SetCell(m_width - 1, y, CellType::Wall);

	// ces soirées-là ...
}
