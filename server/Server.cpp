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

Server::Server() {
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	std::cout << "1" << std::endl;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		//TODO handle failure
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	std::cout << "2" << std::endl;
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		//TODO handle failure
	}

	std::cout << "3" << std::endl;
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// Create a SOCKET for connecting to server
		ListenSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			//TODO handle failure
		}

		std::cout << "4" << std::endl;
		//TODO should either of these loop like how client loops on a failed connect() ?

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			//TODO handle failure
		}
	}

	freeaddrinfo(result);

	std::cout << "5" << std::endl;
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		//TODO handle failure
	}

	std::cout << "6" << std::endl;

	// Accept a client socket
	ClientSockets[0] = accept(ListenSocket, NULL, NULL);
	//TODO iterate through all the ClientSockets eventually

	std::cout << "6.5" << std::endl;
	if (ClientSockets[0] == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		//TODO handle failure
	}

	// No longer need server socket
	closesocket(ListenSocket);

	std::cout << "7" << std::endl;
}

int Server::sendData(char sendbuf[], int buflen, int flags) {
	int err = 0;
	int iResult;
	std::string s(sendbuf);

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

int Server::recvData(char recvbuf[], int buflen, int flags) {
	int err = 0;
	int iResult;
	int totaliResult = 0;
	char tempbufs[NUM_PLAYERS][DEFAULT_BUFLEN];
	ZeroMemory(tempbufs, sizeof(tempbufs));

	for (int i = 0; i < NUM_PLAYERS; i++) {
		//ZeroMemory(tempbufs, sizeof(tempbufs));
		iResult = recv(ClientSockets[i], tempbufs[i], DEFAULT_BUFLEN, flags);
		if (iResult == SOCKET_ERROR) {
			// printf("send failed with error: %d\n", WSAGetLastError());
			std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ClientSockets[i]);
			WSACleanup();
			err = -1;
			continue;
		}
		totaliResult += iResult;
	}
	if (err) return err;
	int recvbufind = 0;
	for (int i = 0; i < NUM_PLAYERS; i++) {
		for (int j = 0; j < DEFAULT_BUFLEN; j++, recvbufind++) {
			recvbuf[recvbufind] = tempbufs[i][j];
		}
	}
	return totaliResult;
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