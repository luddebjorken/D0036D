#include "header.h"
/*
CLASS: WINDOWPRINTER
Class for printing shapes and objects to GameBoard application over UDP
Members:
	void fillRect(Coordinate pos, int w, int h, int color, int delay): Draws a filled rectangle from given parameters. Waits for 'delay' between every row
	void fillRect(int x, int y, int w, int h, int color, int delay): Draws a filled rectangle from given parameters. Waits for 'delay' between every row
	void drawRect(Coordinate pos, int w, int h, int color, int delay): Draws an empty rectangle from given parameters. Waits for 'delay' between every row
	void fillEllipse(Coordinate pos, int color): Draws a filled circle at position 'pos' with color 'color' (static size).

	void clear(): Clears the entire board
	void clearRect(Coordinate pos, int w, int h): Clears area from given parameters

	void drawBoard(): Draws walls of playfield
	void drawBoxes(int** field):  Draws boxes on playfield according to giver field array
	void drawPlayer(int playerID, Coordinate pos, byte direction): Draws player at position 'pos'. Color is determined from playerID and rotation from 'direction'
	void drawBomb(Coordinate pos): Draws bomb at given position

	void moveplayer(unsigned int id, Coordinate newPos, int direction): Redraws player at position 'newPos'
	void placeBomb(Coordinate pos): Places bomb at position 'pos'
	void explode(Coordinate pos): Simulates and draws explosion at position 'pos'

	Coordinate grid(Coordinate in): Converts grid coordinates (13x13) to pixel coordinates on screen (200x200)

	int sock: UDP socket to communicate with GameBoard application
	sockaddr_in addr: Address to GameBoard
	unsigned int id: Player ID
	int** field: Stores state of playing field
	Coordinate* playerPos: Stores position of players

Author: Ludvig Bj�rk F�rare
Version: 1.0
Date: 181016
*/

//Struct for TCP
struct message {
	byte type;
	byte fill1 = 0;
	byte fill2 = 0;
	byte fill3 = 0;
	byte fill4 = 256;
	byte x;
	byte y;
	byte color;
};

//ID = playerID
windowPrinter::windowPrinter(unsigned int id)
{	
	this->id = id;
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (res == 0) {
		std::cout << "WSA Started!" << std::endl;
	}
	else {
		std::cout << "WSA failed to start!" << std::endl;
		exit(0);
	}

	//Defines socket and address, binds socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(44444);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	bind(sock, (sockaddr*)&addr, sizeof(addr));
	
	//Defines field state
	field = new char*[13];
	field[0] =	new char[13]{ 0,0,2,2,2,2,2,2,2,2,2,0,0 };
	field[1] =	new char[13]{ 0,1,2,1,2,1,2,1,2,1,2,1,0 };
	field[2] =	new char[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[3] =	new char[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[4] =	new char[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[5] =	new char[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[6] =	new char[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[7] =	new char[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[8] =	new char[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[9] =	new char[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[10] = new char[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[11] = new char[13]{ 0,1,2,1,2,1,2,1,2,1,2,1,0 };
	field[12] = new char[13]{ 0,0,2,2,2,2,2,2,2,2,2,0,0 };
	playerPos = new Coordinate[4];

	//Sets players start position
	playerPos[0].x = 0;
	playerPos[0].y = 0;

	playerPos[1].x = 12;
	playerPos[1].y = 12;

	playerPos[2].x = 0;
	playerPos[2].y = 12;

	playerPos[3].x = 12;
	playerPos[3].y = 0;

}


windowPrinter::~windowPrinter()
{
}

void windowPrinter::setField(char field[])
{
	for(int y = 0; y < 13; y++)
	{
		for(int x = 0; x < 13; x++)
		{
			this->field[x][y] = field[x+y*13];
			std::cout << field[x+y*13] << std::endl;
		}
	}
}



void windowPrinter::clear() {
	Coordinate c;
	clearRect(c,201,201);
}

void windowPrinter::clearRect(Coordinate pos, int w, int h) {
	//Declares pixel-info message
	message msg;
	msg.type = 2;

	//For every row
	for (size_t i = 0; i < h; i++)
	{
		msg.y = pos.y + i;
		//For every column
		for (size_t j = 0; j < w; j++)
		{
			//Sends message to GameBoard
			msg.x = pos.x + j;
			sendto(sock, (char*)&msg, 7, 0, (sockaddr*)&addr, sizeof(addr));
		}
	}

}

void windowPrinter::fillRect(Coordinate pos, int w, int h, int color, int delay) {
	//Declares pixel-info message
	message msg;
	msg.type = 1;
	msg.color = color;
	//For every row
	for (size_t i = 0; i < h; i++)
	{
		msg.y = pos.y + i;
		//For every column
		for (size_t j = 0; j < w; j++)
		{
			//Sends message to GameBoard
			msg.x = pos.x + j;
			sendto(sock, (char*)&msg, 8, 0, (sockaddr*)&addr, sizeof(addr));
		}
		Sleep(delay);
	}
}

void windowPrinter::fillRect(int x, int y, int w, int h, int color, int delay) {
	//Declares pixel-info message
	message msg;
	msg.type = 1;
	msg.color = color;
	//For every row
	for (size_t i = 0; i < h; i++)
	{
		msg.y = y + i;
		//For every column
		for (size_t j = 0; j < w; j++)
		{
			//Sends message to GameBoard
			msg.x = x + j;
			sendto(sock, (char*)&msg, 8, 0, (sockaddr*)&addr, sizeof(addr));
		}
		Sleep(delay);
	}
}

void windowPrinter::drawRect(Coordinate pos, int w, int h, int color, int delay) {
	message msg;
	msg.type = 1;
	msg.color = color;
	msg.y = pos.y;
	msg.x = pos.x;

	//Top edge
	for (size_t i = 0; i < w; i++)
	{
		msg.x++;
		sendto(sock, (char*)&msg, 8, 0, (sockaddr*)&addr, sizeof(addr));
	}
	//Right edge
	for (size_t i = 0; i < h; i++)
	{
		msg.y++;
		sendto(sock, (char*)&msg, 8, 0, (sockaddr*)&addr, sizeof(addr));
	}
	//Bottom edge
	for (size_t i = 0; i < w; i++)
	{
		msg.x--;
		sendto(sock, (char*)&msg, 8, 0, (sockaddr*)&addr, sizeof(addr));
	}
	//Left edge
	for (size_t i = 0; i < h; i++)
	{
		msg.y--;
		sendto(sock, (char*)&msg, 8, 0, (sockaddr*)&addr, sizeof(addr));
	}


	std::cout << std::endl << "Sent!" << std::endl;
}

void windowPrinter::drawBoard() {
	Coordinate c;
	fillRect(c, 200, 200, 1, 1);

	//Draws field borders
	fillRect(c, 200, 2, 4, 3);
	fillRect(c, 2, 200, 4, 3);
	c.y = 198;
	fillRect(c, 200, 3, 4, 3);
	c.y = 0;
	c.x = 198;
	fillRect(c, 3, 201, 4, 3);

	//Draws field inner walls
	Coordinate pos;
	for (pos.y = 1; pos.y < 12; pos.y += 2)
	{
		for (pos.x = 1; pos.x < 12; pos.x += 2)
		{
			fillRect(grid(pos), 15, 15, 4, 1);
		}
	}

	//Draws boxes
	drawBoxes(field);
}

void windowPrinter::drawBoxes(char** field) {
	Coordinate pos;
	//For every row
	for (pos.x = 0; pos.x < 13; pos.x += 1)
	{
		//For every column
		for (pos.y = 0; pos.y < 13; pos.y += 1)
		{
			//If box is in position pos, draw box
			if (field[pos.x][pos.y] == 2) fillRect(grid(pos), 15, 15, 6, 0);
		}
		Sleep(2);
	}
}

void windowPrinter::drawBomb(Coordinate pos) {
	//Draws black ellipse
	fillEllipse(pos, 0);
}


void windowPrinter::moveplayer(unsigned int id, Coordinate newPos, int direction) {
	//If last player location was on a bomb, draw a bomb
	if (field[playerPos[id].x][playerPos[id].y] == 3) {
		drawBomb(grid(playerPos[id]));
	}
	else { //If not, erase the area (fill with white)
		fillRect(grid(playerPos[id]), 15, 15, 1, 0);
	}
	
	//Saves new position
	playerPos[id] = newPos;
	//Draws player in new location
	drawPlayer(id, newPos, direction);
}

void windowPrinter::placeBomb(Coordinate pos) {
	drawBomb(grid(pos));
	field[pos.x][pos.y] = 3;
}

void windowPrinter::explode(Coordinate pos) {
	Coordinate startH = pos;
	Coordinate startV = pos;
	int lengthH;
	int lengthV;

	int i=-1;
	while(i<3)
	{
		i++;
		if(pos.x-i>=0){
			if (field[pos.x - i][pos.y]==2) {
				std::cout << pos.x - i << ": Box" << std::endl;
				break;
			}
			else if (field[pos.x - i][pos.y]==1) {
				std::cout << pos.x - i << ": Wall" << std::endl;
				i--;
				break;
			}
		}
		else {
			std::cout << pos.x - i << ": Edge" << std::endl;
			i--;
			break;
		}
		std::cout << pos.x - i << ": empty" << std::endl;
	}

	startH.x -= i;
	std::cout << "Start: " << startH.x << std::endl;
	lengthH = i + 1;
	i = 0;
	while(i<3)
	{
		i++;
		if (pos.x + i <= 12) {
			if (field[pos.x + i][pos.y] == 2) {
				break;
			}
			else if (field[pos.x + i][pos.y] == 1) {
				i--;
				break;
			}
		}
		else {
			i--;
			break;
		}
	}

	lengthH += i;

	i = 0;
	while (i < 3)
	{
		i++;
		if (pos.y - i >= 0) {
			if (field[pos.x][pos.y - i] == 2) {
				std::cout << pos.y - i << ": Box" << std::endl;
				break;
			}
			else if (field[pos.x][pos.y - i] == 1) {
				std::cout << pos.y - i << ": Wall" << std::endl;
				i--;
				break;
			}
		}
		else {
			std::cout << pos.y - i << ": Edge" << std::endl;
			i--;
			break;
		}
		std::cout << pos.y - i << ": empty" << std::endl;
	}

	startV.y -= i;
	std::cout << "Start: " << startH.y << std::endl;
	lengthV = i + 1;
	i = 0;
	while (i < 3)
	{
		i++;
		if (pos.y + i <= 12) {
			if (field[pos.x][pos.y + i] == 2) {
				break;
			}
			else if (field[pos.x][pos.y + i] == 1) {
				i--;
				break;
			}
		}
		else {
			i--;
			break;
		}
	}

	lengthV += i;


	for (int i = 0; i < lengthH; i++)
	{
		if (field[startH.x + i][pos.y] != 3) field[startH.x + i][pos.y] = 0;
	}
	for (int i = 0; i < lengthV; i++)
	{
		if (field[pos.x][startV.y + i] != 3) field[pos.x][startV.y + i] = 0;
	}

	field[pos.x][pos.y] = 0;


	fillRect(grid(startH), 15 * lengthH, 15, 7, 0);
	fillRect(grid(startV), 15, 15 * lengthV, 7, 0);
	fillRect(grid(startH), 15 * lengthH, 15, 1, 0);
	fillRect(grid(startV), 15, 15 * lengthV, 1, 0);
}


Coordinate windowPrinter::grid(Coordinate in) {
	Coordinate ret;
	ret.x = in.x * 15 + 2;
	ret.y = in.y * 15 + 2;
	return ret;
}

void windowPrinter::drawPlayer(int player, Coordinate pos, byte direction) {
	int colorA = 0;
	std::cout << "Player: " << player << std::endl;
	switch (player) {
	case 0: 
		colorA = 2;
		break;
	case 1:
		colorA = 3;
		break;
	case 2:
		colorA = 4;
		break;
	case 3:
		colorA = 7;
		break;
	}
	pos = grid(pos);
	fillEllipse(pos, colorA);

	//EYES
	//Offset x, offset y
	int x1 = 0;
	int y1 = 0;
	int x2 = 0;
	int y2 = 0;
	switch (direction) {
	case 0: 
		x1 = 4;
		x2 = 9;
		y1 = 4;
		y2 = 4;
		break;
	case 1:
		x1 = 10;
		x2 = 10;
		y1 = 4;
		y2 = 9;
		break;
	case 2:
		x1 = 4;
		x2 = 9;
		y1 = 9;
		y2 = 9;
		break;
	case 3:
		x1 = 4;
		x2 = 4;
		y1 = 4;
		y2 = 9;
		break;
	}
	fillRect(pos.x + x1, pos.y + y1, 2, 2, 1, 0);
	fillRect(pos.x + x2, pos.y + y2, 2, 2, 1, 0);
}


void windowPrinter::fillEllipse(Coordinate pos,int color) {
	fillRect(pos.x + 5, pos.y + 2, 5, 11, color, 0);
	fillRect(pos.x + 4, pos.y + 3, 7, 9, color, 0);
	fillRect(pos.x + 3, pos.y + 4, 9, 7, color, 0);
	fillRect(pos.x + 2, pos.y + 5, 11, 5, color, 0);
}

