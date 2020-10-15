#pragma once

#include "sh_color.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>

// La classe Snake représente un serpent en jeu, ainsi que toutes ses pièces
// celui-ci possède toujours une taille de trois à l'apparition, et peut grandir,
// se déplacer dans une direction précise, et réapparaitre en remettant sa taille à zéro

class Snake
{
public:
	Snake(const sf::Vector2i& spawnPosition, const sf::Vector2i& direction, const Color& color);
	Snake(std::vector<sf::Vector2i> body, const sf::Vector2i& followingDirection, const Color& color);

	// Fait avancer le serpent dans la direction suivie
	void Advance();

	// Retourne la liste des positions occupées par le serpent
	const std::vector<sf::Vector2i>& GetBody() const;

	const Color& GetColor() const;

	// Retourne la direction actuelle, celle vers laquelle pointe la tête du serpent
	sf::Vector2i GetCurrentDirection() const;
	
	// Retourne la direction suivie, celle vers laquelle le serpent va aller au prochain déplacement
	sf::Vector2i GetFollowingDirection() const;
	
	// Retourne la position de la tête du serpent
	sf::Vector2i GetHeadPosition() const;

	// Agrandit notre serpent en faisant apparaitre une nouvelle pièce après sa queue
	void Grow();
	
	// Fait réapparaitre le serpent (avec sa taille initiale de trois) à l'endroit et la direction indiquée
	void Respawn(const sf::Vector2i& spawnPosition, const sf::Vector2i& direction);

	// Met à jour le corps du serpent avec une liste de positions occupées
	void SetBody(const std::vector<sf::Vector2i>& body);

	// Définit la prochaine direction à suivre (direction doit avoir x ou y à 1/-1 et l'autre à zéro)
	void SetFollowingDirection(const sf::Vector2i& direction);

	// Teste si le serpent possède une partie de son corps sur la cellule à cette position-là
	// on ajoute un petit booléen pour savoir s'il faut tester la tête ou non
	bool TestCollision(const sf::Vector2i& position, bool testHead);

protected:
	Color m_color;
	sf::Vector2i m_followingDir;
	std::vector<sf::Vector2i> m_body; //< doit au moins avoir trois éléments quoiqu'il arrive
};
