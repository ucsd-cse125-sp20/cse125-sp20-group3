#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT  0x501

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Server.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

SOCKET Server::ClientSockets[NUM_PLAYERS];
std::mutex Server::player_states_mtx[NUM_PLAYERS];

//kind of bad design to have the server receive a reference to the manager, but it cuts down on a lot of checks
Server::Server(SceneManager_Server* manager) { 
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;
	
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
	}

	
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// Create a SOCKET for connecting to server
		ListenSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
		}
	}

	freeaddrinfo(result);

	
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}

	// Accept a client socket
	int number_of_clients = 0;
	while (number_of_clients < NUM_PLAYERS) // let MAX_CLIENTS connect
	{
		ClientSockets[number_of_clients] = accept (ListenSocket, NULL, NULL);
		if (ClientSockets[number_of_clients] == INVALID_SOCKET)
		{ // error accepting connection
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		else
		{ // client connected successfully
			// start a thread that will communicate with client
			char id_buf[4];
			((int*)id_buf)[0] = number_of_clients;
			iResult = send(ClientSockets[number_of_clients], id_buf, 4, 0); //tell client what their player id is
			if (!manager->addPlayer(number_of_clients)) { //spawn a player object for the player
				//if somehow the player object already exists
				std::cout << "error spawning player object, player " << number_of_clients << " already exists\n";
			}

			Player_States[number_of_clients].socket_fd = ClientSockets[number_of_clients];
			Player_States[number_of_clients].player_id = number_of_clients;
			Player_States[number_of_clients].disconnected = false;
			//start thread to handle client recvs and sends
			player_threads[number_of_clients] = std::thread(&Server::handle_player_inputs, &Player_States[number_of_clients], 0);

			number_of_clients++;
			std::cout << "Accepted " << number_of_clients << " of " << NUM_PLAYERS << " players" << std::endl;
		}
	}

	// No longer need listening socket
	closesocket(ListenSocket);
}

bool Server::gameInProgress() {
	for (int p = 0; p < NUM_PLAYERS; p++) {
		if (!Player_States[p].disconnected) {
			return true;
		}
	}
	return false;
}

void Server::pushDataAll(char sendbuf[], int buflen, int flags) {
	unsigned char check = NetUtil::checksum(sendbuf, buflen);
	//std::cout << "pushing data, checksum: " << (int)check << "\n";
	
	for (int p = 0; p < NUM_PLAYERS; p++) { //append each byte in sendbuf to all the player outbound buffers
		player_states_mtx[p].lock();

		unsigned char sum = NetUtil::checksum(sendbuf, buflen);
		if (sum != check) std::cout << "checksum changed at p: " << p << " buflen: " << buflen << " check: " << (int)check << " sum: " << (int)sum << "\n";

		for (int i = 0; i < buflen; i++) {
			Player_States[p].out.push_back(sendbuf[i]);
		}
		player_states_mtx[p].unlock();
	}
}

//void Server::pushDataPlayer(int conn_socket, char sendbuf[], int buflen, int flags) {
/*	for (int i = 0; i < buflen; i++) { //append bytes of sendbuf to only designated player's outbound buffer
		Player_States[conn_socket].out.push_back(sendbuf[i]);
	}
}*/

std::vector<PlayerInput> Server::pullData() {
	std::vector<PlayerInput> inputs;
	for (int p = 0; p < NUM_PLAYERS; p++) {
		player_states_mtx[p].lock();
		inputs.push_back(Player_States[p].in);
		player_states_mtx[p].unlock();
	}
	return inputs;
}

int Server::cleanup(int how) {
	int err = 0;
	int iResult;

	for (SOCKET clientSock : ClientSockets) {
		iResult = shutdown(clientSock, how);
		if (iResult == SOCKET_ERROR) {
			std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
			err = -1;
		}
		closesocket(clientSock);
	}
	WSACleanup();
	if (err) return err;
	return iResult;
}

int Server::handle_player_inputs(player_state* state, int flags) {
	int err = 0;
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	u_long blocking = 0, nonblocking = 1;

	iResult = ioctlsocket(state->socket_fd, FIONBIO, &nonblocking);
	if (iResult == SOCKET_ERROR) {
		std::cout << "ioctlsocket before receiving packet size failed with error: " << WSAGetLastError() << "\n";
	}

	while (1)
	{
		ZeroMemory(recvbuf, DEFAULT_BUFLEN);
		//std::cout << "recving\n";
		iResult = recv(state->socket_fd, recvbuf, DEFAULT_BUFLEN, flags);
		if (iResult == SOCKET_ERROR) {
			int code = WSAGetLastError();
			if (code == WSAEWOULDBLOCK) { //edge case, client waiting for server to send data, server shouldn't wait for client's input
				//std::cout << "would block\n"; //maintain what the player input was last frame
			}
			else {
				// error
				printf("Player %d recv failed with error %d\n", state->player_id, WSAGetLastError());
				state->disconnected = 1;
				closesocket(state->socket_fd);
				return 1;
			}
		} else if(iResult == 0){
			printf("Player %d disconnected\n", state->player_id);
			state->disconnected = 1;
			closesocket(state->socket_fd);
			return 0;
		}
		else{
			//format and save data to this connection's player state
			//std::cout << "locking to write in\n";
			player_states_mtx[state->player_id].lock();
			state->in = ((PlayerInput*)recvbuf)[0];
			player_states_mtx[state->player_id].unlock();
		}

		if (state->out.size() > 0) { //if there's data in the outbound buffer, send it
			//std::cout << "locking to send data\n";
			player_states_mtx[state->player_id].lock();
			//std::cout << "sending " << state->out.size() << " bytes\n";

			iResult = send(state->socket_fd, state->out.data(), state->out.size(), flags);
			if (iResult == SOCKET_ERROR) {
				// printf("send failed with error: %d\n", WSAGetLastError());
				printf("Player %d send failed with error: %d\n", state->player_id, WSAGetLastError());
				state->disconnected = 1;
				closesocket(state->socket_fd);
				player_states_mtx[state->player_id].unlock();
				return 1;
			}	

			state->out.clear(); //clear out data
			player_states_mtx[state->player_id].unlock();
		}
	}
}

void Server::end_game(){
	for (int i = 0; i < NUM_PLAYERS; i++){
		player_threads[i].join();
	}
}

/***** legacy code *****/
/*
int Server::sendDataAll(char sendbuf[], int buflen, int flags) {
	int err = 0;
	int iResult;

	for (int i = 0; i < NUM_PLAYERS; i++) {
		iResult = send(ClientSockets[i], sendbuf, buflen, flags);
		if (iResult == SOCKET_ERROR) {
			// printf("send failed with error: %d\n", WSAGetLastError());
			std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ClientSockets[i]);
			WSACleanup();
			err = -1;
		}
	}
	if (err) return err;
	else return iResult;
}

int Server::sendDataPlayer(int conn_socket, char sendbuf[], int buflen, int flags) {
	int iResult;

	iResult = send(ClientSockets[conn_socket], sendbuf, buflen, flags);
	if (iResult == SOCKET_ERROR) {
		// printf("send failed with error: %d\n", WSAGetLastError());
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ClientSockets[conn_socket]);
		WSACleanup();
		return -1;
	}

	else return iResult;
}

int Server::recvData(char recvbuf[], int buflen, int flags) {
	int err = 0;
	int iResult;
	int totaliResult = 0;
	char tempbufs[NUM_PLAYERS][DEFAULT_BUFLEN];
	int recvlens[NUM_PLAYERS];
	ZeroMemory(tempbufs, sizeof(tempbufs));

	for (int i = 0; i < NUM_PLAYERS; i++) {
		//ZeroMemory(tempbufs, sizeof(tempbufs));
		//std::cout << "recving from player " << i << "\n";
		iResult = recv(ClientSockets[i], tempbufs[i], DEFAULT_BUFLEN, flags);
		if (iResult == SOCKET_ERROR) {
			std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ClientSockets[i]);
			WSACleanup();
			err = -1;
			recvlens[i] = 0;
			continue;
		}
		recvlens[i] = iResult;
		//std::cout << "recvlens from player " << i << ": " << recvlens[i] << "\n";
		totaliResult += iResult;
	}
	if (err) return err;
	int recvbufind = 0;
	for (int i = 0; i < NUM_PLAYERS; i++) {
		char player_num = std::to_string(i).c_str()[0]; //convert player index in sockets array to char

		//std::cout << "writing recvbuf for player " << player_num << "\n";
		recvbuf[recvbufind] = player_num; //write player number to recvbuf
		recvbufind++;

		int begin_data_ind = recvbufind;

		if (recvlens[i] != sizeof(PlayerInput)) {
			//std::cout << "non PlayerInput received\n";
		}
		for (int j = 0; j < min(recvlens[i], sizeof(PlayerInput)); j++) { //only copy one playerinput in
			recvbuf[recvbufind] = tempbufs[i][j];
			recvbufind++;
		}

		recvbuf[recvbufind] = DELIMITER;
		recvbufind++;
	}
	return totaliResult;
}
*/