#include "playerInstance.h"

#pragma comment(lib, "Ws2_32.lib")

playerInstance::playerInstance()
{
}

playerInstance::playerInstance(game* gameInstance)
{
	this->gameInstance = gameInstance;
	std::cout << "Playerinstance created!" << std::endl;
}




playerInstance::~playerInstance()
{
	closesocket(clientSocket);
}

void playerInstance::connect(SOCKET clientSocket, int id) {
	this->clientSocket = clientSocket;
	std::cout << "playerInstance started" << std::endl;

	//Sets id
	defaultHead.id = id;

	//Receives joinMessage
	char buffer[256] = {'\0*'};

	recv(clientSocket, buffer, sizeof(buffer), NULL);
	JoinMsg joinRequest;
	memcpy(&joinRequest, buffer, buffer[0]);
	buffer[256] = { '\0*' };

	std::cout << "Player " << joinRequest.name << " connected!" << std::endl;

	//Sends join response (just a msgHead)
	MsgHead joinResponse = defaultHead;
	joinResponse.type = JOIN;
	send(clientSocket, (char*)&joinResponse, joinResponse.length, 0);
	defaultHead.seq_no++; //Increases seq_no
	gameInput();
}


void playerInstance::gameInput() {
	std::cout << "Listening for input on thread " << defaultHead.id << std::endl;
	char buffer[256] = { '\0*' };
	while (true) {
		recv(clientSocket, buffer, sizeof(buffer), NULL);
		MsgHead head;
		memcpy(&head, buffer, 16);
		std::cout << "Message received! size: " << head.length << " | sequence: " << head.seq_no << " | id: " << head.id << " | type: " << head.type << std::endl; //DEBUG
		if (head.length = 42) {
			closesocket(clientSocket);
		}
		if (head.id == defaultHead.id) {
			std::cout << EVENT << std::endl;
			switch (head.type) {
			case EVENT:
				
				std::cout << "EVENT" << std::endl;
				EventMsg eventMessage;
				memcpy(&eventMessage, buffer, 20);
				
				switch (eventMessage.type) {
				case MOVE: {
					std::cout << "Move" << std::endl;
					MoveEvent moveMessage;
					memcpy(&moveMessage, buffer, head.length);
					gameInstance->requestMove(defaultHead.id, moveMessage.pos);
					break;
				}
				case PLACEBOMB: {
					std::cout << "PlaceBomb" << std::endl;
					gameInstance->placeBomb(eventMessage.head.id);
					break;
				}
				}
				break;
			}
		}
		else {
			std::cout << "Invalid ID!" << std::endl;
		}
	}
}



