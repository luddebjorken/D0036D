#include "header.h"

#pragma comment(lib, "Ws2_32.lib")

/*
CLASS: PLAYERINSTANCE
Class for listening for incomming TCP transmissions and responding to requests from a client
Members:
	void connect(int clientSocket, int id): Handles player connect sequence, starts gameinput when done
	void gameInput(): Handles incoming TCP requests from client

	game * gameInstance: Pointer to current game instance

	char* name: Name of player
	MsgHead defaultHead: Default message head. Also used to store player ID
	int clientSocket: Client socket

Author: Ludvig Björk Förare
Version: 1.0
Date: 181016
*/

playerInstance::playerInstance()
{
}

playerInstance::playerInstance(game* gameInstance)
{
	this->gameInstance = gameInstance;
}

playerInstance::~playerInstance()
{
}

//Method for handling connect sequence with client
//Pre: int clientSocket: client TCP socket, int id: player ID
//Post: none
//Saves socket to (this)clientSocket, saves ID in defaultHead
void playerInstance::connect(int clientSocket, int id) {
	this->clientSocket = clientSocket;

	//Sets id
	defaultHead.id = id;

	//Receives joinMessage
	char buffer[256] = {'\0*'};
	recv(clientSocket, buffer, sizeof(buffer), NULL);
	JoinMsg joinRequest;
	std::memcpy(&joinRequest, buffer, buffer[0]);

	this->name = joinRequest.name;

	//Sends join response
	MsgHead joinResponse = defaultHead;
	joinResponse.type = JOIN;
	send(clientSocket, (char*)&joinResponse, joinResponse.length, 0);
	defaultHead.seq_no++; //Increases seq_no
	gameInput(); //Starts listening for game input
}

//Method for listening and responding to client input
//Pre: none
//Post: none
void playerInstance::gameInput() {
	char buffer[256] = { '\0*' };
	while (gameInstance->playerState[defaultHead.id]!=DEAD) { //While alive
		recv(clientSocket, buffer, sizeof(buffer), NULL);

		//Reads message head
		MsgHead head;
		std::memcpy(&head, buffer, 16);

		//Checks if ID is correct (failsafe)
		if (head.id == defaultHead.id) {

			//Switches message type
			switch (head.type) {

			case START: {
				gameInstance->sendStart();
				break;
			}
			case LOADED: {
				gameInstance->markLoaded(defaultHead.id);
				break;
			}
			case EVENT:
				//Reads eventMessage
				EventMsg eventMessage;
				std::memcpy(&eventMessage, buffer, 20);

				//Switches event type
				switch (eventMessage.type) {
				case MOVE: {
					//Reads move message
					MoveEvent moveMessage;
					std::memcpy(&moveMessage, buffer, head.length);
					gameInstance->requestMove(defaultHead.id, moveMessage.pos); //Requests move in given direction from gameInstance
					break;
				}
				case PLACEBOMB: {
					gameInstance->placeBomb(eventMessage.head.id); //Places bomb @ location of player
					break;
				}
				}
				break;
			}
		}
	}
	std::cout << "You have been disconnected!" << std::endl;
}



