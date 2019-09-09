#include "header.h"

game::game()
{
	
}


game::~game()
{
	delete(listenThread);
}

bool game::findServer() {
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (res == 0) {
		std::cout << "WSA started" << std::endl;
	}
	else {
		std::cout << "An error occurred when starting WSA" << std::endl;
		system("Pause");
		return false;
	}
	std::cout << "Name: ";
	std::string name = "ludde2"; 
	//getline(std::cin, name);

	while (true) {
		std::cout << "Server ip: ";
		std::string addrString = "10.10.10.13";
		//std::string addrString = "127.0.0.1";
		//getline(std::cin, addrString);

		//Defines address
		sock = socket(AF_INET, SOCK_STREAM, NULL);

		sockaddr_in addr;
		addr.sin_family = AF_INET; //IPv4
		addr.sin_port = htons(44444); //Client listen port
		inet_pton(AF_INET, addrString.c_str(), &addr.sin_addr);

		//Connects to socket
		int result = SOCKET_ERROR;

		std::cout << "Connecting to " + addrString + "..." << std::endl;
		for (size_t i = 0; i < 5 && result == SOCKET_ERROR; i++) //5 Connect attempts
		{
			result = connect(sock, (sockaddr*)&addr, sizeof(addr));
		}
		if (result != SOCKET_ERROR) {
			std::cout << "Server found!" << std::endl; //DEBUG
			break;
		}
		else {
			std::cout << "Error: server not found!" << std::endl;
		}
	}
		//Constructs and sends join packet
		JoinMsg joinMessage;
		joinMessage.head = defaultHead;
		joinMessage.head.type = JOIN;
		defaultHead.seq_no++;
		strcpy_s(joinMessage.name, name.c_str());
		joinMessage.head.length = sizeof(joinMessage);

		send(sock, (char*)&joinMessage, sizeof(joinMessage), 0);

		//Receives ID
		char buffer[256] = { '\0*' };
		recv(sock, buffer, sizeof(buffer), 0);
		MsgHead joinResponse;
		memcpy(&joinResponse, buffer, buffer[0]);
		defaultHead.id = joinResponse.id;

		std::cout << "ID: " << defaultHead.id << std::endl;

		wp = new windowPrinter(defaultHead.id);

		listenThread = new listener(sock, defaultHead.id, wp);
		std::thread t = std::thread(&listener::listen, listenThread);
		t.detach();

		while (!listenThread->start) {
			if (GetAsyncKeyState('S') & 1) {
				std::cout << "S" << std::endl;
				MsgHead startMsg = defaultHead;
				startMsg.type = START;
				send(sock, (char*)&startMsg, 16, NULL);
			}
		}; //Waits for start signal
		std::cout << "Game started!" << std::endl;
		runGame(&listenThread->loaded); //Starts game input
		
}

void game::runGame(bool * loaded) {
	wp->clear();
	wp->drawBoard();

	MsgHead loadmsg = defaultHead;
	loadmsg.type = LOADED;
	defaultHead.seq_no++;
	
	send(sock, (char*)&loadmsg, 16, 0);
	while (!loaded); //Waits until all player have loaded
	std::cout << "SWEEEEET VICTORYYYY" << std::endl;
	Coordinate p1Pos;
	Coordinate p1Move;
	int bombCount = 2;
	std::clock_t timer = NULL;

	std::cout << "Listening for input" << std::endl;
	while (true) {

		if (GetAsyncKeyState(' ') & 1 && bombCount>0)
		{
			placeBomb();
			bombCount--;
			timer = std::clock();
			
		}

		if (bombCount < 2) { //Fult, men förhoppningsvis mer optimiserat
			if (((double)std::clock()-timer)/ CLOCKS_PER_SEC > 2)
			{
				timer = std::clock();
				bombCount++;
			}
		}


		if (GetAsyncKeyState('A') & 1)
		{
			p1Move.x = -1;
		}
		else if (GetAsyncKeyState('D') & 1)
		{
			p1Move.x = 1;
		}
		else {
			p1Move.x = 0;
		}

		if (GetAsyncKeyState('W') & 1)
		{
			p1Move.y = -1;
		}
		else if (GetAsyncKeyState('S') & 1)
		{
			p1Move.y = 1;
		}
		else {
			p1Move.y = 0;
		}
		
		if (p1Move.x != 0 || p1Move.y != 0) {
			movePlayer(p1Pos, p1Move);
		}
	}
}

void game::movePlayer(Coordinate pos, Coordinate delta) {
	std::cout << "Sending move!" << std::endl; //DEBUG
	MoveEvent moveMsg;
	moveMsg.event.head = defaultHead;
	moveMsg.event.head.type = EVENT;
	moveMsg.event.head.length = 28;
	moveMsg.event.type = MOVE;
	moveMsg.pos = delta;
	defaultHead.seq_no++;

	std::cout << "Size: " << moveMsg.event.head.length << std::endl;
	if (send(sock, (char*)&moveMsg, 28, 0) == 0) {
		closesocket(sock);
		exit(0);
	}

}

void game::placeBomb() {
	EventMsg bombMsg;
	bombMsg.head = defaultHead;
	bombMsg.head.type = EVENT;
	bombMsg.head.length = 20;
	bombMsg.type = PLACEBOMB;
	defaultHead.seq_no++;

	if (send(sock, (char*)&bombMsg, 20, 0) == 0) {
		closesocket(sock);
		exit(0);
	}
}

int main() {
	Sleep(500);
	game gt;
	gt.findServer();
}


