#include "header.h"

/*
CLASS: GAME
Class for running the main gamethread. Handles request from playerInstance threads.
Members:
	void requestMove(int id, Coordinate delta): Attempts to move player[id] in the direction of 'delta'. Sends new location results to all connected clients
	void placeBomb(int id): Places a bomb at position of player[id]
	void sendStart(): Sends start signal to all active clients
	void markLoaded(int id): Marks player[id] as loaded. If all active players are loaded, send 'loaded' message to active players

	int** field: Stores the current state of the playing field using enum 'ObjectDesc' (13x13 int array)
	int clientSocket[4]: Stores client TCP sockets
	int playerState[4]: Stores the state of players using enum 'States'
	Coordinate playerPos[4]: Stores the players coordinates
	bool loaded[4]: Stores the players loaded-state

Author: Ludvig Björk Förare
Version: 1.0
Date: 181016
*/

game::game()
{
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

	playerPos[0].x = 0;
	playerPos[0].y = 0;

	playerPos[1].x = 12;
	playerPos[1].y = 12;

	playerPos[2].x = 0;
	playerPos[2].y = 12;

	playerPos[3].x = 12;
	playerPos[3].y = 0;
	
	playerState[0] = DISCONNECTED;
	playerState[1] = DISCONNECTED;
	playerState[2] = DISCONNECTED;
	playerState[3] = DISCONNECTED;

}


game::~game()
{
	delete[](field);
	delete[](playerPos);
}

//Method for sending start message to all active players
//Pre: none
//Post: none
void game::sendStart() {
	//Sends start signal to all players
	MsgHead startmsg;
	startmsg.type = START;
	startmsg.length = 16;
	for (size_t i = 0; i < 4; i++)
	{
		startmsg.id = i;
		if (send(clientSocket[i], (char*)&startmsg, 16, 0) == 0) {//If send fails, terminates player (No mercy)
			playerState[i] = DISCONNECTED;
		}
		
	}
}
//Method for marking players as loaded. When all active players are loaded, sends loaded message to active players
//Pre: int id: player ID to mark as loaded
//Post: none
void game::markLoaded(int id) {
	//Sends start signal to all players
	loaded[id] = true;

	for (size_t i = 0; i < 4; i++)
	{
		if (playerState[i] != DISCONNECTED && !loaded[i]) {
			return;
		}
	}

	MsgHead loadmsg;
	loadmsg.type = LOADED;
	for (size_t i = 0; i < 4; i++)
	{
		loadmsg.id = i;
		if (send(clientSocket[i], (char*)&loadmsg, 16, 0) == 0) {//If send fails, terminates player (No mercy)
			playerState[i] = DISCONNECTED;
		}
	}
}
//Method for moving a player in direction of input. Takes collision into accord. Sends new position to active players
//Pre: int id: ID of player to move. Coordinate delta: direction to move player
//Post: none
//Manipulates playerPos[id] to represent new location
void game::requestMove(int id, Coordinate delta) {
	//Wished position
	Coordinate newPos;
	newPos.x = playerPos[id].x + delta.x;
	newPos.y = playerPos[id].y + delta.y;

	//Limits to within game area
	if (newPos.x > 12 || newPos.x < 0) newPos.x = playerPos[id].x;
	if (newPos.y > 12 || newPos.y < 0) newPos.y = playerPos[id].y;

	//If space is unavalible
	if (field[newPos.x][newPos.y] != EMPTY) {
		return;
	}

	//Determins direction 0 = up, 1 = right, 2 = down, 4 = left
	int direction = 0;
	switch (delta.x) {
	case 1: direction = 1; break;
	case -1: direction = 3; break;
	}
	switch (delta.y) {
	case 1: direction = 2; break;
	case -1: direction = 0; break;
	}

	//Saves new position
	playerPos[id] = newPos;

	//Constructs positon-change-message
	NewPlayerPositionMsg newPosMsg;
	newPosMsg.msg.head.type = CHANGE;
	newPosMsg.msg.head.length = 36;
	newPosMsg.msg.type = NEWPLAYERPOSITION;

	newPosMsg.id = id;
	newPosMsg.pos.x = newPos.x;
	newPosMsg.pos.y = newPos.y;
	newPosMsg.rot = direction;

	//Sends message to active players
	for (size_t i = 0; i < 4; i++)
	{
		if (playerState[i] != DISCONNECTED) {
			newPosMsg.msg.head.id = i;
			if(send(clientSocket[i], (char*)&newPosMsg, 36, 0)==0){//If send fails, terminates player (No mercy)
				playerState[i] = DISCONNECTED;
			}
		}
	}

}
//Method for placing bomb on player[ID]. Sends bomb placement message to active players. Starts bomb timer
//Pre: int id: ID of player to place bomb on
//Post: none
//Marks bomb in field[][]
void game::placeBomb(int id) {
	//Tests if place is avalible. If not, aborts
	if (field[playerPos[id].x][playerPos[id].y == BOMB]) return;

	//Places bomb on field
	field[playerPos[id].x][playerPos[id].y] = BOMB;

	//Constructs bomb-change-message
	BombPlaceMsg bombMsg;
	bombMsg.msg.head.length = 28;
	bombMsg.msg.head.type = CHANGE;
	bombMsg.msg.type = BOMBPLACE;
	bombMsg.pos = playerPos[id];

	//Starts bomb timer thread
	bombTimer b;
	std::thread t = std::thread(&bombTimer::start, b, playerPos[id], clientSocket, playerState, playerPos, field);
	t.detach();

	//Sends message to all players
	for (size_t i = 0; i < 4; i++)
	{
		if (playerState[i] != DISCONNECTED) {
			bombMsg.msg.head.id = i;
			if (send(clientSocket[i], (char*)&bombMsg, 28, 0) == 0) {//If send fails, terminates player (No mercy)
				playerState[i] = DISCONNECTED;
			}
		}
	}
}

/*
CLASS: BOMBTIMER
Class for handling bomb explosions, is started as a separate thread to keep track of time
Members:
	start(Coordinate pos, int* clientSocket, int* playerState, Coordinate* playerPos, int** field): After four second, calculates and executes and explosion at position 'pos'. Kills players and destroys boxes withing explosion area
Author: Ludvig Björk Förare
Version: 1.0
Date: 181016
*/

bombTimer::bombTimer() {
}
bombTimer::~bombTimer() {
}

//Method for bomb behaviour. After four seconds explodes into '+' formation, taking collission into accord. Destroys boxes and kills players
//Pre: Coordinate pos: position of bomb. int* clientSocket: array of clients. int* playerState: state of players. Coordinate* playerpos: Positions of players. int** field: Current field
//Post: none
//Manipulates field in explosion area, changes playerState if player is killed or has disconnected
void bombTimer::start(Coordinate pos, int* clientSocket, int* playerState, Coordinate* playerPos, int** field) {
	//Waits for explosion
	std::this_thread::sleep_for(std::chrono::seconds(4));

	//Constructs bomb message
	BombPlaceMsg explodeMsg;
	explodeMsg.msg.head.length = 28;
	explodeMsg.msg.head.type = CHANGE;
	explodeMsg.msg.type = EXPLOSION;
	explodeMsg.pos = pos;

	//Calculates affected area
	Coordinate startH = pos; //Explosion start position (horizontal)
	Coordinate startV = pos; //Explosion start position (vertical)
	int lengthH = 0; //Explosion length (horizontal)
	int lengthV = 0; //Explosion length (vertical)

	//Horizontal start
	int i = 0;
	while (i < 3) //Bomb range = 3
	{
		i++;
		if (pos.x - i >= 0) { //If withing field
			if (field[pos.x - i][pos.y] == BOX) {  //If box, mark start @ box
				break;
			}
			else if (field[pos.x - i][pos.y] == WALL) { //If wall, mark start before wall
				i--;
				break;
			}
		}
		else { //If edge, mark start before edge
			i--;
			break;
		}
	}

	startH.x -= i; //Changes start
	lengthH = i + 1; //Adds to length

	//Horizontal end
	i = 0;
	while (i < 3)
	{
		i++;
		if (pos.x + i <= 12) { //If withing field
			if (field[pos.x + i][pos.y] == BOX) { //If box, mark end @ box
				break;
			}
			else if (field[pos.x + i][pos.y] == WALL) { //If wall, mark end before wall
				i--;
				break;
			}
		}
		else { //If edge, mark end before edge
			i--;
			break;
		}
	}

	lengthH += i; //Extends length

	//Same code, but for vertical
	i = 0;
	while (i < 3)
	{
		i++;
		if (pos.y - i >= 0) {
			if (field[pos.x][pos.y - i] == BOX) {
				break;
			}
			else if (field[pos.x][pos.y - i] == WALL) {
				i--;
				break;
			}
		}
		else {
			i--;
			break;
		}
	}

	startV.y -= i;
	lengthV = i + 1;
	i = 0;
	while (i < 3)
	{
		i++;
		if (pos.y + i <= 12) {
			if (field[pos.x][pos.y + i] == BOX) {
				break;
			}
			else if (field[pos.x][pos.y + i] == WALL) {
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

	//Marks all affected spaces as empty (ignores bombs)
	for (int i = 0; i < lengthH; i++)
	{
		if (field[startH.x + i][pos.y] != BOMB) field[startH.x + i][pos.y] = EMPTY;
		field[startH.x + i][pos.y] = EMPTY;
	}
	for (int i = 0; i < lengthV; i++)
	{
		if (field[pos.x][startV.y + i] != BOMB) field[pos.x][startV.y + i] = EMPTY;
		field[pos.x][startV.y + i] = EMPTY;
	}

	field[pos.x][pos.y] = 0; //Removes bomb

	//For every potential player
	for (size_t i = 0; i < 4; i++)
	{
		if (playerState[i] != DISCONNECTED) { //If player is not disconnected
			if ((playerPos[i].x == pos.x && playerPos[i].y >= startV.y && playerPos[i].y < startV.y + lengthV) || //If player is in affected area
				(playerPos[i].y == pos.y && playerPos[i].x >= startH.x && playerPos[i].x < startV.x + lengthH)) {
				playerState[i] = DEAD;
			}
			//
			explodeMsg.msg.head.id = i;
			if (send(clientSocket[i], (char*)&explodeMsg, 28, 0) == 0) { //If send fails, terminates player (No mercy)
				playerState[i] = DISCONNECTED;
			}
		}
	}
}
