#pragma once
#include "playerInstance.h"
#include <thread>
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <chrono>
#include "structs.h"
#include "game.h"


class connectThread {

public:
	connectThread();
	~connectThread();
	void start();
	playerInstance** pi;
	std::thread playerThread[4];
};

