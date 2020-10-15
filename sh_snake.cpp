#include "sh_snake.hpp"
#include "sh_constants.hpp"
#include <cassert>

Snake::Snake(const sf::Vector2i& spawnPosition, const sf::Vector2i& direction, const Color& color) :
m_color(color),
m_followingDir(direction)
{
	Respawn(spawnPosition, direction);
}

Snake::Snake(std::vector<sf::Vector2i> body, const sf::Vector2i& followingDirection, const Color& color) :
m_color(color),
m_followingDir(followingDirection),
m_body(std::move(body))
{
}

void Snake::Advance()
{
	for (std::size_t i = m_body.size() - 1; i != 0; i--)
	{
		auto& pos = m_body[i];
		pos = m_body[i - 1];
	}

	m_body[0] += m_followingDir;
}

const std::vector<sf::Vector2i>& Snake::GetBody() const
{
	return m_body;
}

const Color& Snake::GetColor() const
{
	return m_color;
}

sf::Vector2i Snake::GetCurrentDirection() const
{
	return m_body[0] - m_body[1];
}

sf::Vector2i Snake::GetFollowingDirection() const
{
	return m_followingDir;
}

sf::Vector2i Snake::GetHeadPosition() const
{
	return m_body[0];
}

void Snake::Grow()
{
	std::size_t lastPartIndex = m_body.size() - 1;
	sf::Vector2i lastPartDirection = m_body[lastPartIndex] - m_body[lastPartIndex - 1];

	m_body.push_back(m_body.back() + lastPartDirection);
}

void Snake::Respawn(const sf::Vector2i& spawnPosition, const sf::Vector2i& direction)
{
	m_body.clear();
	m_body.push_back(spawnPosition);
	m_body.push_back(spawnPosition - direction);
	m_body.push_back(spawnPosition - direction * 2);
	m_followingDir = direction;
}

void Snake::SetBody(const std::vector<sf::Vector2i>& body)
{
	assert(body.size() >= 3);
	m_body = body;
}

void Snake::SetFollowingDirection(const sf::Vector2i& direction)
{
	assert(direction.x == 0 || direction.y == 0);
	m_followingDir = direction;
}

bool Snake::TestCollision(const sf::Vector2i& position, bool testHead)
{
	for (std::size_t i = (testHead) ? 0 : 1; i < m_body.size(); ++i)
	{
		if (m_body[i] == position)
			return true;
	}

	return false;
}
