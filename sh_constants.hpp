#pragma once

// Ce fichier contient des constantes pouvant être utiles à la fois côté serveur et client

// Taille d'une cellule en pixels
const int cellSize = 32;
const short port = 14769;

// Taille de la grille
const int gridWidth = 40;
const int gridHeight = 24;

// Nombre de secondes entre deux avancement du serpent
const float tickDelay = 1.f / 4.f; //< quatre mouvements par seconde