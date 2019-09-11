#include "game.h"



game::game()
{
	std::cout << "Gameinstance created!" << std::endl;
	field = new int*[13];

	field[0] = new int[13]{ 0,0,2,2,2,2,2,2,2,2,2,0,0 };
	field[1] = new int[13]{ 0,1,2,1,2,1,2,1,2,1,2,1,0 };
	field[2] = new int[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[3] = new int[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[4] = new int[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[5] = new int[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[6] = new int[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[7] = new int[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[8] = new int[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[9] = new int[13]{ 2,1,2,1,2,1,2,1,2,1,2,1,2 };
	field[10] = new int[13]{ 2,2,2,2,2,2,2,2,2,2,2,2,2 };
	field[11] = new int[13]{ 0,1,2,1,2,1,2,1,2,1,2,1,0 };
	field[12] = new int[13]{ 0,0,2,2,2,2,2,2,2,2,2,0,0 };

	test.x = 1;
	test.y = 1;

	playerPos[0].x = 0;
	playerPos[0].y = 0;

	playerPos[1].x = 12;
	playerPos[1].y = 12;

	playerPos[2].x = 0;
	playerPos[2].y = 12;

	playerPos[3].x = 12;
	playerPos[3].y = 0;
	
}


game::~game()
{
	delete[](field);
	delete[](playerPos);
}


void game::requestMove(int id, Coordinate delta) {
	//HIT CHECK
	Coordinate newPos;
	newPos.x = playerPos[id].x + delta.x;
	newPos.y = playerPos[id].y + delta.y;
	byte direction = 0;
	switch (delta.x) {
	case 1: direction = 1; break;
	case -1: direction = 3; break;
	}
	switch (delta.y) {
	case 1: direction = 2; break;
	case -1: direction = 0; break;
	}

	//Limits to within borders
	if (newPos.x > 12 || newPos.x < 0) newPos.x = playerPos[id].x;
	if (newPos.y > 12 || newPos.y < 0) newPos.y = playerPos[id].y;

	//If space is unavalible
	if (field[newPos.x][newPos.y] != 0) {
		std::cout << "Invalid move!" << std::endl; //DEBUG
		return;
	}

	std::cout << "Move player " << id << " to " << newPos.x << ":" << newPos.y << std::endl;
	playerPos[id] = newPos;

	NewPlayerPositionMsg newPosMsg;
	newPosMsg.msg.head.type = CHANGE;
	newPosMsg.msg.head.length = 36;
	newPosMsg.msg.type = NEWPLAYERPOSITION;
	newPosMsg.id = id;
	newPosMsg.pos.x = newPos.x;
	newPosMsg.pos.y = newPos.y;
	newPosMsg.rot = direction;
	for (size_t i = 0; i < 4; i++)
	{
		if (playerState[i] != 0) {
			newPosMsg.msg.head.id = i;
			send(clientSocket[i], (char*)&newPosMsg, 36, 0);
			std::cout << "Position sent!" << std::endl;
		}
	}

}

void game::placeBomb(int id) {
	if (field[playerPos[id].x][playerPos[id].y == 3]) return;
	field[playerPos[id].x][playerPos[id].y] = 3;

	BombPlaceMsg bombMsg;
	bombMsg.msg.head.length = 28;
	bombMsg.msg.head.type = CHANGE;
	bombMsg.msg.type = BOMBPLACE;
	bombMsg.pos = playerPos[id];

	bombTimer b;
	std::thread t = std::thread(&bombTimer::start, b, playerPos[id], clientSocket, playerState, playerPos, field);
	t.detach();

	for (size_t i = 0; i < 4; i++)
	{
		if (playerState[i] != 0) {
			bombMsg.msg.head.id = i;
			send(clientSocket[i], (char*)&bombMsg, 28, 0);
		}
	}
}

bombTimer::bombTimer() {
}
bombTimer::~bombTimer() {
}
void bombTimer::start(Coordinate pos, SOCKET* clientSocket, int* playerState, Coordinate* playerPos, int** field) {
	std::this_thread::sleep_for(std::chrono::seconds(4));
	BombPlaceMsg explodeMsg;
	explodeMsg.msg.head.length = 28;
	explodeMsg.msg.head.type = CHANGE;
	explodeMsg.msg.type = EXPLOSION;
	explodeMsg.pos = pos;

	Coordinate startH = pos;
	Coordinate startV = pos;
	int lengthH = 0;
	int lengthV = 0;

	int i = 0;
	while (i < 3)
	{
		i++;
		if (pos.x - i >= 0) {
			if (field[pos.x - i][pos.y] == 2) {
				std::cout << pos.x - i << ": Box" << std::endl;
				break;
			}
			else if (field[pos.x - i][pos.y] == 1) {
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
	while (i < 3)
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
		else std::cout << "AAA" << std::endl;
	}

	for (int i = 0; i < lengthV; i++)
	{
		if (field[pos.x][startV.y + i] != 3) field[pos.x][startV.y + i] = 0;
	}

	field[pos.x][pos.y] = 0;

	for (size_t i = 0; i < 13; i++)
	{
		for (size_t j = 0; j < 13; j++)
		{
			std::cout << field[j][i] << "|";
		}
		std::cout << std::endl << "____________________________________" << std::endl;
	}


	for (size_t i = 0; i < 4; i++)
	{
		if (playerState[i] != 0) {
			if ((playerPos[i].x == pos.x && playerPos[i].y >= startV.y && playerPos[i].y < startV.y + lengthV) ||
				(playerPos[i].y == pos.y && playerPos[i].x >= startH.x && playerPos[i].x < startV.x + lengthH)) {
				
				
			}
			explodeMsg.msg.head.id = i;
			send(clientSocket[i], (char*)&explodeMsg, 28, 0);
		}
	}
}
