#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include "structs.h"
#pragma comment (lib, "ws2_32.lib")
using namespace std;

class windowPrinter
{
public:
	windowPrinter();
	~windowPrinter();
	void drawPlayer(int player, int x, int y, byte direction);
	void fillRect(int x, int y, int w, int h, int color, int delay);
	void drawRect(int x, int y, int w, int h, int color, int delay);
	void clearRect(int x, int y, int w, int h);
	void drawBoard();
	void drawBoxes(int** field);
	void fillEllipse(int x, int y, int color);
	void clear();
private:
	int sock;
	sockaddr_in addr;

	int** field;
	Coordinate* playerPos;
};

