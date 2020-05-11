#ifndef _SERVER_H_
#define _SERVER_H_
#endif
#define WIN32_LEAN_AND_MEAN

#pragma once

#define _WIN32_WINNT  0x501

#include "../common/macros.h"
#include "../common/client2server.h"
#include "SceneManager_Server.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class Server {
private:
	struct player_state
	{
		int socket_fd, player_id;
		bool disconnected;
		PlayerInput in;
		std::vector<char> out;
	};

	static SOCKET ClientSockets[NUM_PLAYERS];
	player_state Player_States[NUM_PLAYERS];
	std::thread player_threads[NUM_PLAYERS];
	static std::mutex player_states_mtx[NUM_PLAYERS];
public:

	Server(SceneManager_Server* manager); //set up server listening
	bool gameInProgress();
	void pushDataAll(char sendbuf[], int buflen, int flags); //send data to clients
	//void pushDataPlayer(int conn_socket, char sendbuf[], int buflen, int flags);
	std::vector<PlayerInput> pullData(); //recv data from clients
	int cleanup(int how); //close all clients' connections with server
	static int handle_player_inputs(player_state* state, int flags);
	void end_game();
};
