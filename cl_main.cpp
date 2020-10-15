#include "sh_constants.hpp"
#include "sh_protocol.hpp"
#include "cl_resources.hpp"
#include "cl_grid.hpp"
#include "cl_snake.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <optional>
#include <WinSock2.h>
#include <ws2tcpip.h>

struct GameState
{
	std::optional<ClientGrid> clientGrid;
	std::vector<ClientSnake> clientSnakes;
};

const int windowWidth = CellSize * GridWidth;
const int windowHeight = CellSize * GridHeight;

void game(SOCKET sock);
void handle_message(const std::vector<std::uint8_t>& message, std::size_t offset, GameState& gameState);
bool receive_message(SOCKET sock, std::vector<std::uint8_t>& pendingData, GameState& gameState);

int main()
{
	// Initialisation du générateur aléatoire
	// Note : en C++ moderne on dispose de meilleurs outils pour générer des nombres aléatoires,
	// mais ils sont aussi plus verbeux / complexes à utiliser, ce n'est pas très intéressant ici.
	std::srand(std::time(nullptr));

	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data); //< MAKEWORD compose un entier 16bits à partir de deux entiers 8bits utilisés par WSAStartup pour connaître la version à initialiser

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		// En cas d'erreur avec Winsock, la fonction WSAGetLastError() permet de récupérer le dernier code d'erreur
		// Sous POSIX l'équivalent est errno
		std::cerr << "failed to open socket (" << WSAGetLastError() << ")\n";
		return EXIT_FAILURE;
	}

	BOOL option = 1;
	if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&option), sizeof(option)) == SOCKET_ERROR)
	{
		std::cerr << "failed to disable Nagle's algorithm (" << WSAGetLastError() << ")\n";
		return EXIT_FAILURE;
	}

	for (;;)
	{
		std::string ipAddress;
		std::cout << "Please enter server address:" << std::endl;
		std::cin >> ipAddress;

		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(AppPort);

		if (inet_pton(AF_INET, ipAddress.data(), &serverAddress.sin_addr.s_addr) != 1)
		{
			std::cerr << "Invalid IP address" << std::endl;
			continue;
		}

		if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
		{
			std::cerr << "failed to connect" << std::endl;
			continue;
		}

		break;
	}

	game(sock);

	closesocket(sock);

	WSACleanup();
}

void game(SOCKET sock)
{
	// Chargement des assets du jeu
	Resources resources;
	if (!LoadResources(resources))
		return;

	// Création et ouverture d'une fenêtre
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Snake");
	window.setVerticalSyncEnabled(true);

	// Étant donné que l'origine de tous les objets est au centre, il faut décaler la caméra d'autant pour garder
	// une logique de grille à l'affichage
	sf::Vector2f viewSize(windowWidth, windowHeight);
	sf::Vector2f viewCenter = viewSize / 2.f - sf::Vector2f(CellSize, CellSize) / 2.f;
	window.setView(sf::View(viewCenter, viewSize));

	GameState gameState;

	std::vector<std::uint8_t> pendingData;

	while (window.isOpen())
	{
		// On traite les événements fenêtre qui se sont produits depuis le dernier tour de boucles
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				// L'utilisateur souhaite fermer la fenêtre, fermons-la
				case sf::Event::Closed:
					window.close();
					break;

				// Une touche a été enfoncée par l'utilisateur
				case sf::Event::KeyPressed:
				{
					std::optional<SnakeDirection> direction;

					// On met à jour la direction si la touche sur laquelle l'utilisateur a appuyée est une flèche directionnelle
					switch (event.key.code)
					{
						case sf::Keyboard::Up:
							direction = SnakeDirection::Up;
							break;

						case sf::Keyboard::Down:
							direction = SnakeDirection::Down;
							break;

						case sf::Keyboard::Left:
							direction = SnakeDirection::Left;
							break;

						case sf::Keyboard::Right:
							direction = SnakeDirection::Right;
							break;

						default:
							break;
					}

					// On applique la direction, si modifiée, au serpent
					if (direction)
					{
						std::vector<std::uint8_t> packet;
						std::size_t sizeOffset = packet.size();
						Serialize_u16(packet, 0);
						Serialize_u8(packet, static_cast<std::uint8_t>(Opcode::C_UpdateDirection));
						Serialize_u8(packet, static_cast<std::uint8_t>(*direction));

						Serialize_u16(packet, sizeOffset, packet.size() - sizeof(std::uint16_t));

						if (send(sock, reinterpret_cast<const char*>(packet.data()), packet.size(), 0) == SOCKET_ERROR)
							std::cerr << "failed to send data to server (" << WSAGetLastError() << ")" << std::endl;
					}
					break;
				}

				default:
					break;
			}
		}

		if (!receive_message(sock, pendingData, gameState))
		{
			// Got disconnected
			window.close();
			break;
		}

		// On remplit la scène d'une couleur plus jolie pour les yeux
		window.clear(sf::Color(247, 230, 151));

		// On affiche les éléments statiques
		if (gameState.clientGrid)
			gameState.clientGrid->Draw(window, resources);

		// On affiche le serpent
		for (ClientSnake& snake : gameState.clientSnakes)
			snake.Draw(window, resources);

		// On actualise l'affichage de la fenêtre
		window.display();
	}
}

void handle_message(const std::vector<std::uint8_t>& message, std::size_t offset, GameState& gameState)
{
	Opcode opcode = static_cast<Opcode>(Unserialize_u8(message, offset));
	switch (opcode)
	{
		case Opcode::S_GameState:
		{
			std::uint8_t snakeCount = Unserialize_u8(message, offset);

			gameState.clientSnakes.clear();
			gameState.clientSnakes.reserve(snakeCount);

			for (std::uint8_t i = 0; i < snakeCount; ++i)
			{
				Color color = Unserialize_color(message, offset);
				std::uint16_t snakeBodyParts = Unserialize_u16(message, offset);
				std::vector<sf::Vector2i> snakeBody(snakeBodyParts);
				for (sf::Vector2i& pos : snakeBody)
				{
					pos.x = Unserialize_i8(message, offset);
					pos.y = Unserialize_i8(message, offset);
				}

				gameState.clientSnakes.emplace_back(std::move(snakeBody), sf::Vector2i(1, 0), color);
			}
			break;
		}

		case Opcode::S_GridState:
		{
			int gridWidth = Unserialize_u8(message, offset);
			int gridHeight = Unserialize_u8(message, offset);

			gameState.clientGrid.emplace(gridWidth, gridHeight);

			std::size_t fullCellCount = Unserialize_u16(message, offset);
			for (std::size_t i = 0; i < fullCellCount; ++i)
			{
				int x = Unserialize_u8(message, offset);
				int y = Unserialize_u8(message, offset);
				CellType cellType = static_cast<CellType>(Unserialize_u8(message, offset));

				gameState.clientGrid->SetCell(x, y, cellType);
			}
			break;
		}

		case Opcode::S_GridUpdate:
		{
			int x = Unserialize_u8(message, offset);
			int y = Unserialize_u8(message, offset);
			CellType cellType = static_cast<CellType>(Unserialize_u8(message, offset));

			gameState.clientGrid->SetCell(x, y, cellType);
			break;
		}
	}
}

bool receive_message(SOCKET sock, std::vector<std::uint8_t>& pendingData, GameState& gameState)
{
	WSAPOLLFD pollDescriptor;
	pollDescriptor.fd = sock;
	pollDescriptor.events = POLLRDNORM;
	pollDescriptor.revents = 0;

	int activeSockets = WSAPoll(&pollDescriptor, 1, 0);
	if (activeSockets == SOCKET_ERROR)
	{
		std::cerr << "failed to poll sockets (" << WSAGetLastError() << ")\n";
		return false;
	}

	if (activeSockets > 0)
	{
		char buffer[1024];
		int byteRead = recv(sock, buffer, sizeof(buffer), 0);
		if (byteRead == SOCKET_ERROR || byteRead == 0)
		{
			// Une erreur s'est produite ou le nombre d'octets lus est de zéro, indiquant une déconnexion
			// on adapte le message en fonction.
			if (byteRead == SOCKET_ERROR)
				std::cerr << "failed to read from server (" << WSAGetLastError() << "), disconnecting..." << std::endl;
			else
				std::cout << "server disconnected" << std::endl;

			return false;
		}

		std::size_t oldSize = pendingData.size();
		pendingData.resize(oldSize + byteRead);
		std::memcpy(&pendingData[oldSize], buffer, byteRead);

		while (pendingData.size() >= sizeof(std::uint16_t))
		{
			// -- Réception du message --

			// On déserialise la taille du message
			std::uint16_t messageSize;
			std::memcpy(&messageSize, &pendingData[0], sizeof(messageSize));

			messageSize = ntohs(messageSize);

			if (pendingData.size() - sizeof(messageSize) < messageSize)
				break;

			// Handle message
			handle_message(pendingData, sizeof(messageSize), gameState);

			// On retire la taille que nous de traiter des données en attente
			std::size_t handledSize = sizeof(messageSize) + messageSize;
			pendingData.erase(pendingData.begin(), pendingData.begin() + handledSize);
		}
	}

	return true;
}
