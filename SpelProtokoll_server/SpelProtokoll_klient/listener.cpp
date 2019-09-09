#include "header.h"


listener::listener(SOCKET socket, unsigned int id, windowPrinter* wp)
{
	this->sock = socket;
	this->wp = wp;
	this->id = id;
}


listener::~listener()
{
	
}

void listener::listen() {
	while (true) {
		char buffer[512] = { "\0*" };
		if (recv(sock, buffer, sizeof(buffer), 0)==0) {
			std::cout << "Error: No server response, exiting..." << std::endl;
			closesocket(sock);
			exit(0);
			break;
		}
		MsgHead head;
		memcpy(&head, buffer, 16);
		std::cout << "Message received! size: " << head.length << " | sequence: " << head.seq_no << " | id: " << head.id << " | type: " << head.type << std::endl; //DEBUG
		if (head.id == id) {
			switch (head.type) {
				case START: {
					start = true;
					break;
				}
				case LOADED: {
					std::cout << "LOADED" << std::endl;
					loaded = true;
					break;
				}
				case CHANGE:
					std::cout << "Change" << std::endl;
					ChangeMsg eventMessage;
					memcpy(&eventMessage, buffer, 20);
					switch (eventMessage.type) {
					case NEWPLAYER: {
						NewPlayerMsg joinMsg;
						memcpy(&joinMsg, buffer, head.length);
						break;
					}
					case PLAYERLEAVE: {
						PlayerLeaveMsg leaveMsg;
						memcpy(&leaveMsg, buffer, head.length);
						break;
					}
					case NEWPLAYERPOSITION: {
						NewPlayerPositionMsg posMsg;
						memcpy(&posMsg, buffer, head.length);
						std::cout << "Newpos " << posMsg.id << " : " << posMsg.pos.x << ":" << posMsg.pos.y << std::endl; //DEBUG
						wp->moveplayer(posMsg.id, posMsg.pos, posMsg.rot);
						break;
					}
					case BOMBPLACE: {
						BombPlaceMsg bombMsg;
						memcpy(&bombMsg, buffer, head.length);
						std::cout << "Bomb " << bombMsg.pos.x << ":" << bombMsg.pos.y << std::endl; //DEBUG
						wp->placeBomb(bombMsg.pos);
						break;
					}
					case EXPLOSION: {
						BombPlaceMsg explodeMsg;
						memcpy(&explodeMsg, buffer, head.length);
						std::cout << "Explosion " << explodeMsg.pos.x << ":" << explodeMsg.pos.y << std::endl; //DEBUG
						wp->explode(explodeMsg.pos);
						break;
					}
					}
					break;
			}
		}
	}
}

void listener::printPlayers() {

}
