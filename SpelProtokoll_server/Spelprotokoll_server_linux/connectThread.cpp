#include "header.h"
/*
CLASS: CONNECTTHREAD
Class for handling inital client connection. Creates game instance object and gives clients separate threads for input (through playerInstance).
Members:
	void start(): Starts connection

	game* gameInstance: Pointer to current gameInstance
	playerInstance** pi: Array of pointers to playerInstances
	std::thread playerThread[4]: Threads in which playerInstances are running

Author: Ludvig Björk Förare
Version: 1.0
Date: 181016
*/

int main() {
	connectThread t;
	t.start();
}

connectThread::connectThread() {
}

connectThread::~connectThread() {
}
//Method for starting up game and handling incoming TCP connections
//Pre: none
//Post: none
//Defines gameInstance, playerInstance and playerThreads
void connectThread::start() {
	//Sets up server socket
	int serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(44444);
	addr.sin_addr.s_addr = INADDR_ANY;
	socklen_t addrSize = sizeof(addr);

	if (serverSocket <= 0) {
		std::cout << "Failed to open socket! Exiting..." << std::endl;
		return;
	}

	//Binds and listens to server socket
	bind(serverSocket, (sockaddr*)&addr, addrSize);
	listen(serverSocket, SOMAXCONN);


	//Defines gameInstance and playerInstances
	gameInstance = new game();
	pi = new playerInstance*[4];


	int clientSocket;
	int playersConnected = 0;

	while (true) {
		std::cout << "Awating players..." << std::endl;
		//Takes incomming connections
		if (playersConnected < 4) {
			if (clientSocket = accept(serverSocket, (sockaddr*)&addr, &addrSize)) {

				//Defines player instance
				pi[playersConnected] = new playerInstance(gameInstance);
				std::cout << "Client connected!" << std::endl;

				//Creates listening thread for player
				playerThread[playersConnected] = std::thread(&playerInstance::connect, pi[playersConnected], clientSocket, playersConnected);
				playerThread[playersConnected].detach();

				//Sets player state in game
				gameInstance->clientSocket[playersConnected] = clientSocket;
				gameInstance->playerState[playersConnected] = ALIVE;

				playersConnected++;

				/*
				//Checks if any players have disconnected
				int disconnects = 0;
				for (size_t i = 0; i < playersConnected; i++)
				{
					if (!pi[i]->connected) {
						delete(pi[i]);
						for (size_t j = i; j < playersConnected; j++)
						{
							if (pi[j] != nullptr) {
								if (pi[j]->connected) {
									pi[i] = pi[j];
									pi[j] = nullptr;
								}
							}
						}
						disconnects++;
					}
				}
				playersConnected -= disconnects;
				*/

			}
		}
		else {
			std::cout << "Lobby full!" << std::endl;
			break;
		}
	}

	
}
