#pragma once
#include <thread>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <pthread.h>
#include <chrono>
#include <cstring>
#include "structs.h"
#include <math.h>

class bombTimer
{
public:
	bombTimer();
	~bombTimer();
	void start(Coordinate pos, int* clientSocket, int* playerState, Coordinate* playerPos, char** field);
};

class game
{
public:
	game();
	~game();
	void requestMove(int id, Coordinate delta);
	void placeBomb(int id);
	void sendStart();
	void markLoaded(int id);
	void broadcast(char * message, const unsigned int length);
	char** field;
	int clientSocket[4];
	int playerState[4]; //0 = disconnected, 1 = alive, 2 = dead
	unsigned int seq_num[4];
	Coordinate playerPos[4];
	bool loaded[4];
};


class playerInstance
{
public:
	playerInstance();
	playerInstance(game* gameInstance);
	~playerInstance();
	void connect(int clientSocket, int id);
	void gameInput();

	game * gameInstance;

private:
	char* name;
	MsgHead defaultHead;
	int clientSocket;
};

class connectThread {

public:
	connectThread();
	~connectThread();
	void start();

	game* gameInstance;
	playerInstance** pi;
	std::thread playerThread[4];
};





