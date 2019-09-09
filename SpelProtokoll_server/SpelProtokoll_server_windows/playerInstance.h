#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include "structs.h"
#include <thread>
#include "game.h"

class playerInstance
{
public:
	playerInstance();
	playerInstance(game* gameInstance);
	~playerInstance();
	void connect(SOCKET clientSocket, int id);
	void gameInput();
	game * gameInstance;
private:
	MsgHead defaultHead;
	SOCKET clientSocket;
};
