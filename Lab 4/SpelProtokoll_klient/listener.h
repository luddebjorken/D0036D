#pragma once
#include "structs.h"
#include <iostream>
#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <Windows.h>
#include "windowPrinter.h"
struct listenResult{
	int type; //1: pos change, 2: bomb drop, 3: explosion, 4: disconnect 
};


class listener
{
public:
	listener(SOCKET socket, unsigned int id, windowPrinter* wp);
	~listener();
	void listen();
private:
	SOCKET socket;
	listenResult results;
	unsigned int id;
	windowPrinter* wp;
};

