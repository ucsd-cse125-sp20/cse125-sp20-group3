#define WIN32_LEAN_AND_MEAN

#pragma once

#define _WIN32_WINNT  0x501

#include "../common/macros.h"
#include "game_state.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class Server {
private:
	SOCKET ClientSockets[NUM_PLAYERS];
	players_state Players_State[NUM_PLAYERS];
	std::thread players_threads[NUM_PLAYERS];
	std::mutex players_state_mtx[NUM_PLAYERS];
public:
	Server(); //set up server listening
	int sendData(char sendbuf[], int buflen, int flags); //send data to clients
	int recvData(char recvbuf[], int buflen, int flags); //recv data from clients
	int cleanup(int how); //close all clients' connections with server
	int handle_player_inputs(players_state* players_state, int flags);
	void end_game();
};