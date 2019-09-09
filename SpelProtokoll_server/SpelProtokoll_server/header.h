#pragma once
#include <thread>
#include <iostream>
#include <sys/types.h>
#include <winsock.h>
#include <string>
#include <thread>
#include <chrono>
#include "structs.h"

class bombTimer
{
public:
	bombTimer();
	void start(Coordinate pos, SOCKET* clientSocket, int* playerState, Coordinate* playerPos, int** field);
	~bombTimer();
private:
	int** field;
};

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

class connectThread {

public:
	connectThread();
	~connectThread();
	void start();
	playerInstance** pi;
	std::thread playerThread[4];
};





