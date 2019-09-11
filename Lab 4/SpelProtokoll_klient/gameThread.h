#pragma once
#include <iostream>
#include <cstring>
#include <string>
#include <thread>

#include "windowPrinter.h"
#include "structs.h"
#include "listener.h"

class game
{
public:
	game();
	~game();
	void movePlayer(Coordinate pos, Coordinate delta);

	bool findServer();
	void runGame();

private:
	listener* listenThread;
	MsgHead defaultHead;
	windowPrinter* wp;
	SOCKET sock;

};

