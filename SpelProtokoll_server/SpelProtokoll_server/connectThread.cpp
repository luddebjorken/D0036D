#include "header.h"

int main() {
	connectThread t;
	t.start();
}

connectThread::connectThread() {
}

connectThread::~connectThread() {
}

void connectThread::start() {
	//Starts WSA
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (res == 0) {
		std::cout << "WSAStartup successful" << std::endl;
	}
	else {
		std::cout << "Error WSAStartup" << std::endl;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, NULL);
	sockaddr_in addr;
	addr.sin_family = AF_INET; //IPv4
	addr.sin_port = htons(44444);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	int addrSize = sizeof(addr);

	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Failed to open socket!" << std::endl;
		//return;
	}
	bind(serverSocket, (sockaddr*)&addr, addrSize);
	listen(serverSocket, SOMAXCONN);

	SOCKET clientSocket;
	int playersConnected = 0;

	game* gameInstance = new game();
	pi = new playerInstance*[4];
	for (size_t i = 0; i < 4; i++)
	{
		pi[i] = new playerInstance(gameInstance);
	}

	

	while (true) {
		std::cout << "Awating players..." << std::endl;

		if (clientSocket = accept(serverSocket, (sockaddr*)&addr, &addrSize)) {
			std::cout << "Client connected!" << std::endl;
			//Creates listening thread for player
			playerThread[playersConnected] = std::thread(&playerInstance::connect, pi[playersConnected], clientSocket, playersConnected);
			playerThread[playersConnected].detach();
			gameInstance->clientSocket[playersConnected] = clientSocket;
			gameInstance->playerState[playersConnected] = 1;

			playersConnected++;
		}
	}

	
}
