#include "cl_snake.hpp"
#include "sh_constants.hpp"
#include <cassert>

// Calcule la rotation à appliquer aux pièces du serpent pour l'affichage
float computeRotationFromDirection(const sf::Vector2i& direction)
{
	if (direction.x > 0)
		return 0.f;
	else if (direction.x < 0)
		return 180.f;
	else if (direction.y > 0)
		return 90.f;
	else
		return -90.f;
}

// Calcule la rotation à appliquer aux coins du serpent pour l'affichage
float computeRotationForCorner(const sf::Vector2i& from, const sf::Vector2i& corner, const sf::Vector2i& to)
{
	if (from.x > corner.x)
	{
		if (corner.y > to.y)
			return -90.f;
		else
			return 0.f;
	}
	else if (from.x < corner.x)
	{
		if (corner.y > to.y)
			return 180.f;
		else
			return 90.f;
	}
	else if (from.y > corner.y)
	{
		if (corner.x > to.x)
			return 90.f;
		else
			return 0.f;
	}
	else
	{
		if (corner.x > to.x)
			return 180.f;
		else
			return -90.f;
	}
}

void ClientSnake::Draw(sf::RenderTarget& renderTarget, Resources& resources) const
{
	sf::Color color;
	color.r = m_color.r;
	color.g = m_color.g;
	color.b = m_color.b;
	color.a = 0xFF;

	for (std::size_t i = 0; i < m_body.size(); ++i)
	{
		const auto& pos = m_body[i];

		float rotation;
		sf::Sprite* sprite;
		if (i == 0)
		{
			rotation = computeRotationFromDirection(GetCurrentDirection());

			sprite = &resources.snakeHead;
		}
		else if (i == m_body.size() - 1)
		{
			sf::Vector2i direction = m_body[i - 1] - m_body[i];
			rotation = computeRotationFromDirection(direction);

			sprite = &resources.snakeTail;
		}
		else
		{
			// Détection des coins, qui nécessitent un traitement différent
			sf::Vector2i direction = m_body[i - 1] - m_body[i + 1];
			if (direction.x == 0 || direction.y == 0)
			{
				rotation = computeRotationFromDirection(direction);
				sprite = &resources.snakeBody;
			}
			else
			{
				rotation = computeRotationForCorner(m_body[i - 1], m_body[i], m_body[i + 1]);
				sprite = &resources.snakeBodyCorner;
			}
		}

		sprite->setColor(color);
		sprite->setPosition(pos.x * CellSize, pos.y * CellSize);
		sprite->setRotation(rotation);

		renderTarget.draw(*sprite);
	}
}
