#pragma once
#include "structs.h"
#include <iostream>
#include <WS2tcpip.h>
#include <thread>
#include <chrono>
class game
{
public:
	game();
	~game();
	void requestMove(int id, Coordinate delta);
	void placeBomb(int id);
	int** field;
	bool running;
	SOCKET clientSocket[4];
	int playerState[4]; //0 = disconnected, 1 = alive, 2 = dead
private:
	Coordinate playerPos[4];
	Coordinate test;
};

class bombTimer
{
public:
	bombTimer();
	void start(Coordinate pos, SOCKET* clientSocket, int* playerState, Coordinate* playerPos, int** field);
	~bombTimer();
private:
	int** field;
};

