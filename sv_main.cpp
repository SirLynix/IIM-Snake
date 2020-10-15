#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "sh_constants.hpp"
#include <SFML/System/Clock.hpp> //< Gestion du temps avec la SFML
#include <cassert> //< assert
#include <iostream> //< std::cout/std::cerr
#include <string> //< std::string / std::string_view
#include <thread> //< std::thread
#include <vector> //< std::vector
#include <winsock2.h> //< Header principal de Winsock
#include <ws2tcpip.h> //< Header pour le modèle TCP/IP, permettant notamment la gestion d'adresses IP

// Sous Windows il faut linker ws2_32.lib (Propriétés du projet => Éditeur de lien => Entrée => Dépendances supplémentaires)
// Ce projet est également configuré en C++17 (ce n'est pas nécessaire à winsock)

/*
//////
Squelette d'un serveur de Snake
//////
*/

// On déclare un prototype des fonctions que nous allons définir plus tard
// (en C++ avant d'appeler une fonction il faut dire au compilateur qu'elle existe, quitte à la définir après)
int server(SOCKET sock);
void tick();

int main()
{
	// Initialisation de Winsock en version 2.2
	// Cette opération est obligatoire sous Windows avant d'utiliser les sockets
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data); //< MAKEWORD compose un entier 16bits à partir de deux entiers 8bits utilisés par WSAStartup pour connaître la version à initialiser

	// La création d'une socket se fait à l'aide de la fonction `socket`, celle-ci prend la famille de sockets, le type de socket,
	// ainsi que le protocole désiré (0 est possible en troisième paramètre pour laisser le choix du protocole à la fonction).
	// Pour IPv4, on utilisera AF_INET et pour IPv6 AF_INET6
	// Ici on initialise donc une socket TCP
	// Sous POSIX la fonction renvoie un entier valant -1 en cas d'erreur
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
		std::cerr << "failed to disable Naggle's algorithm (" << WSAGetLastError() << ")\n";
		return EXIT_FAILURE;
	}

	int r = server(sock);

	// Comme dans le premier code, on n'oublie pas de fermer les sockets dès qu'on en a plus besoin
	closesocket(sock);

	// Et on arrête l'application réseau également.
	WSACleanup();

	return r; //< On retourne le code d'erreur de la fonction server / client
}

int server(SOCKET sock)
{
	// On compose une adresse IP (celle-ci sert à décrire ce qui est autorisé à se connecter ainsi que le port d'écoute)
	// Cette adresse IP est associée à un port ainsi qu'à une famille (IPv4/IPv6)
	sockaddr_in bindAddr;
	bindAddr.sin_addr.s_addr = INADDR_ANY;
	bindAddr.sin_port = htons(port); //< Conversion du nombre en big endian (endianness réseau)
	bindAddr.sin_family = AF_INET;

	// On associe notre socket à une adresse / port d'écoute
	if (bind(sock, reinterpret_cast<sockaddr*>(&bindAddr), sizeof(bindAddr)) == SOCKET_ERROR)
	{
		std::cerr << "failed to bind socket (" << WSAGetLastError() << ")\n";
		return EXIT_FAILURE;
	}

	// On passe la socket en mode écoute, passant notre socket TCP en mode serveur, capable d'accepter des connexions externes
	// Le second argument de la fonction est le nombre de clients maximum pouvant être en attente
	if (listen(sock, 10) == SOCKET_ERROR)
	{
		std::cerr << "failed to put socket into listen mode (" << WSAGetLastError() << ")\n";
		return EXIT_FAILURE;
	}

	// On définit une structure pour représenter une liste de client (avec un identifiant numérique)
	unsigned int nextClientId = 1;

	struct Client
	{
		SOCKET socket;
		unsigned int id;
		std::vector<std::uint8_t> pendingData;
	};

	std::vector<Client> clients;

	// On déclare quelques petits outils pour gérer le temps
	sf::Clock clock;

	// Temps entre les ticks (tours de jeu)
	sf::Time tickInterval = sf::seconds(tickDelay);
	sf::Time nextTick = clock.getElapsedTime() + tickInterval;

	// Boucle infinie pour continuer d'accepter des clients
	for (;;)
	{
		// On construit une liste de descripteurs pour la fonctions WSAPoll, qui nous permet de surveiller plusieurs sockets simultanément
		// Ces descripteurs référencent les sockets à surveiller ainsi que les événements à écouter (le plus souvent on surveillera l'entrée,
		// à l'aide de POLLRDNORM). Ceci va détecter les données reçues en entrée par nos sockets, mais aussi les événements de déconnexion.
		// Dans le cas de la socket serveur, cela permet aussi de savoir lorsqu'un client est en attente d'acceptation (et donc que l'appel à accept ne va pas bloquer).

		// Note: on pourrait ne pas reconstruire le tableau à chaque fois, si vous voulez le faire en exercice ;o
		std::vector<WSAPOLLFD> pollDescriptors;
		{
			// La méthode emplace_back construit un objet à l'intérieur du vector et nous renvoie une référence dessus
			// alternativement nous pourrions également construire une variable de type WSAPOLLFD et l'ajouter au vector avec push_back 
			auto& serverDescriptor = pollDescriptors.emplace_back();
			serverDescriptor.fd = sock;
			serverDescriptor.events = POLLRDNORM;
			serverDescriptor.revents = 0;
		}

		// On rajoute un descripteur pour chacun de nos clients actuels
		for (Client& client : clients)
		{
			auto& clientDescriptor = pollDescriptors.emplace_back();
			clientDescriptor.fd = client.socket;
			clientDescriptor.events = POLLRDNORM;
			clientDescriptor.revents = 0;
		}

		// On appelle la fonction WSAPoll (équivalent poll sous Linux) pour bloquer jusqu'à ce qu'un événement se produise
		// au niveau d'une de nos sockets. Cette fonction attend un nombre défini de millisecondes (-1 pour une attente infinie) avant
		// de retourner le nombre de sockets actives.
		int activeSockets = WSAPoll(pollDescriptors.data(), pollDescriptors.size(), 1);
		if (activeSockets == SOCKET_ERROR)
		{
			std::cerr << "failed to poll sockets (" << WSAGetLastError() << ")\n";
			return EXIT_FAILURE;
		}

		// activeSockets peut avoir trois valeurs différentes :
		// - SOCKET_ERROR en cas d'erreur (géré plus haut)
		// - 0 si aucune socket ne s'est activée avant la fin du délai
		// - > 0, avec le nombre de sockets activées
		// Dans notre cas, comme le délai est infini et l'erreur gérée plus haut, nous ne pouvons qu'avec un nombre positifs de sockets

		if (activeSockets > 0)
		{
			// WSAPoll modifie le champ revents des descripteurs passés en paramètre pour indiquer les événements déclenchés
			for (WSAPOLLFD& descriptor : pollDescriptors)
			{
				// Si ce descripteur n'a pas été actif, on passe au suivant
				if (descriptor.revents == 0)
					continue;

				// Ce descripteur a été déclenché, et deux cas de figures sont possibles.
				// Soit il s'agit du descripteur de la socket serveur (celle permettant la connexion de clients), signifiant qu'un nouveau client est en attente
				// Soit une socket client est active, signifiant que nous avons reçu des données (ou potentiellement que le client s'est déconnecté)
				if (descriptor.fd == sock)
				{
					// Nous sommes dans le cas du serveur, un nouveau client est donc disponible
					sockaddr_in clientAddr;
					int clientAddrSize = sizeof(clientAddr);

					SOCKET newClient = accept(sock, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
					if (newClient == INVALID_SOCKET)
					{
						std::cerr << "failed to accept new client (" << WSAGetLastError() << ")\n";
						return EXIT_FAILURE;
					}

					// Rajoutons un client à notre tableau, avec son propre ID numérique
					auto& client = clients.emplace_back();
					client.id = nextClientId++;
					client.socket = newClient;

					// Représente une adresse IP (celle du client venant de se connecter) sous forme textuelle
					char strAddr[INET_ADDRSTRLEN];
					inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, strAddr, INET_ADDRSTRLEN);

					std::cout << "client #" << client.id << " connected from " << strAddr << std::endl;

					// Ici nous pourrions envoyer un message à tous les clients pour indiquer la connexion d'un nouveau client
				}
				else
				{
					// Nous sommes dans le cas où le descripteur représente un client, tâchons de retrouver lequel
					auto clientIt = std::find_if(clients.begin(), clients.end(), [&](const Client& c)
						{
							return c.socket == descriptor.fd;
						});
					assert(clientIt != clients.end());

					Client& client = *clientIt;

					// La socket a été activée, tentons une lecture
					char buffer[1024];
					int byteRead = recv(client.socket, buffer, sizeof(buffer), 0);
					if (byteRead == SOCKET_ERROR || byteRead == 0)
					{
						// Une erreur s'est produite ou le nombre d'octets lus est de zéro, indiquant une déconnexion
						// on adapte le message en fonction.
						if (byteRead == SOCKET_ERROR)
							std::cerr << "failed to read from client #" << client.id << " (" << WSAGetLastError() << "), disconnecting..." << std::endl;
						else
							std::cout << "client #" << client.id << " disconnected" << std::endl;

						// Ici aussi nous pourrions envoyer un message à tous les clients pour notifier la déconnexion d'un client

						// On oublie pas de fermer la socket avant de supprimer le client de la liste
						closesocket(client.socket);
						clients.erase(clientIt);
					}
					else
					{
						// Nous avons reçu un message de la part du client, affichons-le et renvoyons le aux autres clients

						std::size_t oldSize = client.pendingData.size();
						client.pendingData.resize(oldSize + byteRead);
						std::memcpy(&client.pendingData[oldSize], buffer, byteRead);

						while (client.pendingData.size() >= sizeof(std::uint16_t))
						{
							// -- Réception du message --

							// On déserialise la taille du message
							std::uint16_t messageSize;
							std::memcpy(&messageSize, &client.pendingData[0], sizeof(messageSize));

							messageSize = ntohs(messageSize);

							if (client.pendingData.size() - sizeof(messageSize) < messageSize)
								break;

							// On copie le contenu du message depuis les données en attente
							std::string message(messageSize, ' ');
							std::memcpy(&message[0], &client.pendingData[sizeof(messageSize)], messageSize);

							// On retire la taille que nous de traiter des données en attente
							std::size_t handledSize = sizeof(messageSize) + messageSize;
							client.pendingData.erase(client.pendingData.begin(), client.pendingData.begin() + handledSize);

							// -- Gestion du message --

							// Préfixons le message d'un "Client #X - " pour identifier le client
							message = "Client #" + std::to_string(client.id) + " - " + message;

							// On préfixe la taille du message avant celui-ci
							std::vector<std::uint8_t> sendBuffer(sizeof(std::uint16_t) + message.size());

							// On sérialise l'entier 16bits
							std::uint16_t size = htons(message.size());
							std::memcpy(&sendBuffer[0], &size, sizeof(std::uint16_t));

							// On écrit la chaine de caractère
							std::memcpy(&sendBuffer[sizeof(std::uint16_t)], message.data(), message.size());

							std::cout << message << std::endl;
							for (Client& c : clients)
							{
								if (send(c.socket, (char*)sendBuffer.data(), sendBuffer.size(), 0) == SOCKET_ERROR)
								{
									std::cerr << "failed to send message to client #" << c.id << ": (" << WSAGetLastError() << ")\n";
									// Pas de return ici pour éviter de casser le serveur sur l'envoi à un seul client,
									// contentons-nous pour l'instant de logger l'erreur
								}
							}
						}
					}
				}
			}
		}

		sf::Time now = clock.getElapsedTime();

		// On vérifie si assez de temps s'est écoulé pour faire avancer la logique du jeu
		if (now >= nextTick)
		{
			// On met à jour la logique du jeu
			tick();

			// On prévoit la prochaine mise à jour
			nextTick += tickInterval;
		}
	}

	return EXIT_SUCCESS;
}

void tick()
{

}