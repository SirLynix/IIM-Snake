#include "cl_resources.hpp"
#include "sh_constants.hpp"

bool LoadResources(Resources& resources)
{
	const int tileSize = 64;
	if (!resources.tiles.loadFromFile("assets/snake-tiles.png"))
		return false;

	float origin = tileSize / 2.f;
	float scale = static_cast<float>(CellSize) / static_cast<float>(tileSize);

	resources.apple = sf::Sprite(resources.tiles, sf::IntRect(tileSize, tileSize, tileSize, tileSize));
	resources.apple.setOrigin(origin, origin);
	resources.apple.setScale(scale, scale);

	resources.snakeBody = sf::Sprite(resources.tiles, sf::IntRect(tileSize, 0, tileSize, tileSize));
	resources.snakeBody.setOrigin(origin, origin);
	resources.snakeBody.setScale(scale, scale);

	resources.snakeBodyCorner = sf::Sprite(resources.tiles, sf::IntRect(0, 0, tileSize, tileSize));
	resources.snakeBodyCorner.setOrigin(origin, origin);
	resources.snakeBodyCorner.setScale(scale, scale);

	resources.snakeHead = sf::Sprite(resources.tiles, sf::IntRect(tileSize * 2, 0, tileSize, tileSize));
	resources.snakeHead.setOrigin(origin, origin);
	resources.snakeHead.setScale(scale, scale);

	resources.snakeTail = sf::Sprite(resources.tiles, sf::IntRect(0, tileSize, tileSize, tileSize));
	resources.snakeTail.setOrigin(origin, origin);
	resources.snakeTail.setScale(scale, scale);

	return true;
}