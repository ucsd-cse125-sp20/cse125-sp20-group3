#ifndef _SERVER_H_
#define _SERVER_H_

#define WIN32_LEAN_AND_MEAN

#pragma once

#define _WIN32_WINNT  0x501

#include "../common/macros.h"
#include "../common/client2server.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class Server {
private:
	SOCKET ClientSockets[NUM_PLAYERS];
public:
	Server(); //set up server listening
	int sendDataAll(char sendbuf[], int buflen, int flags); //send data to clients
	int sendDataPlayer(int conn_socket, char sendbuf[], int buflen, int flags);
	int recvData(char recvbuf[], int buflen, int flags); //recv data from clients
	int cleanup(int how); //close all clients' connections with server
};

#endif