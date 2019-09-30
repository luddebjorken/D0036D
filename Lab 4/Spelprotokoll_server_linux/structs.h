#pragma once
#include <string>

#define MAXNAMELENGTH 32

enum ObjectDesc
{
	EMPTY,
	WALL,
	BOX,
	BOMB
};

enum States {
	DISCONNECTED,
	ALIVE,
	DEAD
};

struct Coordinate
{
	int x = 0;
	int y = 0;
};

enum MsgType {
	JOIN,
	LEAVE,
	START,
	LOADED,
	CHANGE,
	EVENT
};

struct MsgHead {
	unsigned int length = 16; //Size of just head
	unsigned int seq_no;
	unsigned int id;
	MsgType type;
};

//Join/leave

struct JoinMsg {
	MsgHead head;
	char name[MAXNAMELENGTH];
};


//Change (client to server)
enum ChangeType{
	NEWPLAYER,
	PLAYERLEAVE,
	NEWPLAYERPOSITION,
	BOMBPLACE,
	EXPLOSION,
	FIELDUPDATE
};

struct ChangeMsg { //size 20
	MsgHead head;
	ChangeType type;
};

//Types of changeMsg
struct NewPlayerMsg{
	ChangeMsg msg;
	char name[MAXNAMELENGTH]; 
};
struct PlayerLeaveMsg{
	ChangeMsg msg;
};
struct NewPlayerPositionMsg{
	ChangeMsg msg; 
	int id;
	Coordinate pos; 
	int rot;
};
struct BombPlaceMsg {
	ChangeMsg msg;
	Coordinate pos;
};
struct UpdateFieldMsg
{
	ChangeMsg msg;
	char board[169];
};

//Event message (server to client)
enum EventType{
	MOVE,
	PLACEBOMB
};

// Included first in all Event messages
struct EventMsg{ //Size 20
	MsgHead head;
	EventType type;
};


struct MoveEvent{
	EventMsg event;
	Coordinate pos;
};
