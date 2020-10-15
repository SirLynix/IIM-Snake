#include "cl_grid.hpp"
#include "sh_constants.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <cassert>

Grid::Grid(int width, int height) :
m_height(height),
m_width(width)
{
	// On change la taille du vecteur pour qu'il stocke suffisamment de cellules
	// et on les initialise par défaut à None
	m_content.resize(width * height, CellType::None);

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

void Grid::Draw(sf::RenderTarget& renderTarget, Resources& resources) const
{
	// On déclare un bloc graphique pour afficher nos murs
	sf::RectangleShape wallShape(sf::Vector2f(cellSize - 2, cellSize - 2));
	wallShape.setOrigin(cellSize / 2.f, cellSize / 2.f);
	wallShape.setFillColor(sf::Color(200, 200, 200));
	wallShape.setOutlineColor(sf::Color::Black);
	wallShape.setOutlineThickness(2.f);

	// On itère sur tous les blocs de la grille pour les afficher ou non
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			switch (GetCell(x, y))
			{
				case CellType::Apple:
					resources.apple.setPosition(cellSize * x, cellSize * y);
					renderTarget.draw(resources.apple);
					break;

				case CellType::Wall:
					wallShape.setPosition(cellSize * x, cellSize * y);
					renderTarget.draw(wallShape);
					break;

				default:
					break; //< rien à afficher ici
			}
		}
	}
}

CellType Grid::GetCell(int x, int y) const
{
	assert(x >= 0 && x < m_width);
	assert(y >= 0 && y < m_height);

	return m_content[y * m_width + x];
}

void Grid::SetCell(int x, int y, CellType cellType)
{
	assert(x >= 0 && x < m_width);
	assert(y >= 0 && y < m_height);

	m_content[y * m_width + x] = cellType;
}
