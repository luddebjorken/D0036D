#include "header.h"
/***
Class for handling inital client connection. Creates game instance object and gives clients separate threads for input (through playerInstance).
Members:
	void start(): Starts connection

	game* gameInstance: Pointer to current gameInstance
	playerInstance** pi: Array of pointers to playerInstances
	std::thread playerThread[4]: Threads in which playerInstances are running

Author: Ludvig Bj�rk F�rare
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
/***
Method for starting up game and handling incoming TCP connections
Pre: none
Post: none
Defines gameInstance, playerInstance and playerThreads
*/
void connectThread::start() {
	//Sets up server socket
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12312);
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
				std::cout << "Client connected!" << std::endl;
				pi[playersConnected] = new playerInstance(gameInstance);

				//Creates listening thread for player
				playerThread[playersConnected] = std::thread(&playerInstance::connect, pi[playersConnected], clientSocket, playersConnected);
				playerThread[playersConnected].detach();

				//Sets player state in game
				gameInstance->clientSocket[playersConnected] = clientSocket;
				gameInstance->playerState[playersConnected] = ALIVE;

				playersConnected++;
			}
		}
		else 
		{
			std::cout << "Lobby full!" << std::endl;
			break;
		}
	}

	
}
