#pragma once

#include <cstdint>

// Ce fichier contient des constantes pouvant être utiles à la fois côté serveur et client

const std::uint16_t AppPort = 14768;

// Taille d'une cellule en pixels
const int CellSize = 32;

// Taille de la grille
const int GridWidth = 40;
const int GridHeight = 24;

// Nombre de secondes entre deux avancement du serpent
const float TickDelay = 1.f / 4.f; //< quatre mouvements par seconde

enum class SnakeDirection
{
	Left,
	Right,
	Up,
	Down
};