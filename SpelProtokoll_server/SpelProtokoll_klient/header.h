#pragma once

#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#include <cstring>
#include <string>
#include <thread>
#include <Windows.h>
#include <ctime>

#include "structs.h"


class windowPrinter
{
public:
	windowPrinter(unsigned int id);
	~windowPrinter();

	void fillRect(Coordinate pos, int w, int h, int color, int delay);
	void fillRect(int x, int y, int w, int h, int color, int delay);
	void drawRect(Coordinate pos, int w, int h, int color, int delay);
	void fillEllipse(Coordinate pos, int color);

	void clear();
	void clearRect(Coordinate pos, int w, int h);

	void drawBoard();
	void drawBoxes(int** field); 
	void drawPlayer(int playerID, Coordinate pos, byte direction);
	void drawBomb(Coordinate pos);

	void moveplayer(unsigned int id, Coordinate newPos, int direction);
	void placeBomb(Coordinate pos);
	void explode(Coordinate pos);
private:
	int sock;
	sockaddr_in addr;
	unsigned int id;
	int** field;
	Coordinate* playerPos;
	Coordinate grid(Coordinate in);
};


class listener
{
public:
	listener(SOCKET socket, unsigned int id, windowPrinter* wp);
	~listener();
	void listen();
	bool start;
	bool loaded;
private:
	SOCKET sock;
	unsigned int id;
	windowPrinter* wp;
	void printPlayers();
};

class game
{
public:
	game();
	~game();

	void movePlayer(Coordinate pos, Coordinate delta);
	void placeBomb();

	bool findServer();
	void runGame(bool * loaded);

private:
	listener* listenThread;
	MsgHead defaultHead;
	windowPrinter* wp;
	SOCKET sock;
};
