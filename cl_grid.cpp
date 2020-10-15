#include "cl_grid.hpp"
#include "sh_constants.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <cassert>

void ClientGrid::Draw(sf::RenderTarget& renderTarget, Resources& resources) const
{
	// On déclare un bloc graphique pour afficher nos murs
	sf::RectangleShape wallShape(sf::Vector2f(CellSize - 2, CellSize - 2));
	wallShape.setOrigin(CellSize / 2.f, CellSize / 2.f);
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
					resources.apple.setPosition(CellSize * x, CellSize * y);
					renderTarget.draw(resources.apple);
					break;

				case CellType::Wall:
					wallShape.setPosition(CellSize * x, CellSize * y);
					renderTarget.draw(wallShape);
					break;

				default:
					break; //< rien à afficher ici
			}
		}
	}
}

