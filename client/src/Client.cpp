//#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT  0x501

#include "Client.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


Client::Client(std::string servername) {
	WSADATA wsaData;
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(servername.c_str(), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	std::cout << "connection established with server\n";

	//UIUtils::editText("waiting", "connected, waiting for other players", "small font", 0xffffffff);
	UIUtils::changeImage("waiting_for_player", "waiting.png", float2(1,1));

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
	}
}

int Client::sendData(char sendbuf[], int buflen, int flags) {
	//std::cout << "sending data\n";
	int iResult = send(ConnectSocket, sendbuf, buflen, flags);
	//std::cout << "sent\n";
	if (iResult == SOCKET_ERROR) {
		//std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}
	return iResult;
}

int Client::recvPlayerID() {
	char idbuf[4];
	int iResult = 0;
	while (iResult == 0) {
		iResult = recv(ConnectSocket, idbuf, 4, 0); //receive id int
		if (iResult == SOCKET_ERROR) {
			std::cout << "recv ID failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return '0';
		}
	}
	int id = ((int*)idbuf)[0];
	return id;
}

Client::UpData Client::recvAndFormatData() {
	//std::cout << "recvAndFormatData\n";
	UpData data;

	char size_int_buf[sizeof(int)];
	int bytes_to_receive;
	std::vector<char> recvbuf;
	std::map<int, Entity::EntityData> idEntMap;
	int iResult;
	u_long blocking = 0, nonblocking = 1;
	
	while (1) { //receive until recv queue is empty
		iResult = ioctlsocket(ConnectSocket, FIONBIO, &nonblocking);
		if (iResult == SOCKET_ERROR) {
			//std::cout << "ioctlsocket before receiving packet size failed with error: " << WSAGetLastError() << "\n";
		}
		//std::cout << "receiving data packet size\n";
		iResult = recv(ConnectSocket, size_int_buf, sizeof(int), 0); //first receive size of data as an int
		if (iResult == SOCKET_ERROR) { //nothing on queue, start processing data to return
			int code = WSAGetLastError();
			if (code == WSAEWOULDBLOCK) {
				//std::cout << "nothing on recv queue, breaking\n";
				break;
			}
			else {
				//std::cout << "recv of size failed with error: " << code << std::endl;
				closesocket(ConnectSocket);
				WSACleanup();
				return data;
			}
		}
		bytes_to_receive = ((int*)size_int_buf)[0]; //save the bytes received as an int
		//std::cout << "found " << bytes_to_receive << " bytes to receive\n";

		recvbuf.resize(bytes_to_receive); //resize vector buffer to be the size of the incoming data

		iResult = ioctlsocket(ConnectSocket, FIONBIO, &blocking);
		if (iResult == SOCKET_ERROR) {
			std::cout << "ioctlsocket before receiving data packet failed with error: " << WSAGetLastError() << "\n";
		}
		iResult = recv(ConnectSocket, &recvbuf[0], (int)recvbuf.size(), 0); //receive data block
		if (iResult == SOCKET_ERROR) {
			std::cout << "recv of data failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return data;
		}
		//std::cout << "received " << iResult << " bytes\n";

		/* process and format data */
		int i = 0;
		char* recvData = &recvbuf[0];

		data.stateUpdate.redTeamData = ((Team::TeamData*)(recvData + i))[0];
		i += sizeof(Team::TeamData);
		if (recvbuf[i] != DELIMITER) std::cout << "delimiter after redTeamData expected but not found\n";
		i++;

		data.stateUpdate.blueTeamData = ((Team::TeamData*)(recvData + i))[0];
		i += sizeof(Team::TeamData);
		if (recvbuf[i] != DELIMITER) std::cout << "delimiter after blueTeamData expected but not found\n";
		i++;

		if (recvbuf[i] != DELIMITER) std::cout << "closing delimiter for state expected but not found\n";
		i++;

		int state = 0;
		int id;
		Entity::EntityData ent_data;
		//std::cout << "processing\n";
		for (; i < recvbuf.size(); i++) {
			if (state == 0) { //read bytes as id int
				if (recvbuf[i] == DELIMITER) { //delimiter found at beginning of state 0 must be closing delimiter, break out
					break;
				}
				else {
					id = ((int*)(recvData + i))[0];
					i += sizeof(int); //advance i to where delimiter should be

					if (recvbuf[i] == DELIMITER) {
						//std::cout << "read id: " << id << " | state 0 delimiter at i: " << i << "\n";
						state++; //end of id bytes found, advance to reading data
					}
					else {
						std::cout << "state 0 delimiter expected but not found, i: " << i << "\n";
					}
				}
			}
			else if (state == 1) { //read bytes as an EntityData
				char* recvData = &recvbuf[0];
				ent_data = ((Entity::EntityData*)(recvData + i))[0];
				i += (sizeof Entity::EntityData); //advance i to where delimiter should be

				if (recvbuf[i] == DELIMITER) {
					//std::cout << "state 1 delimiter at i: " << i << "\n";
					char oldActionState;
					int oldTargetID; //we want to preserve firing states in the case of multiple server ticks within one client update
					if (idEntMap.find(id) != idEntMap.end()) {
						oldActionState = idEntMap[id].actionState;
						oldTargetID = idEntMap[id].targetID;
					}
					else {
						oldActionState = ACTION_STATE_IDLE;
						oldTargetID = -1;
					}

					idEntMap[id] = ent_data; //overwrite everything else
					if (oldActionState == ACTION_STATE_FIRE) { //preserve firing states
						idEntMap[id].actionState = ACTION_STATE_FIRE;
						idEntMap[id].targetID = oldTargetID;
					}
					state = 0; //end of data confirmed, reset to reading id bytes
				}
				else {
					std::cout << "state 1 delimiter expected but not found, i: " << i << "\n";
				}
			}
		}
		if (recvbuf[recvbuf.size() - 2] != DELIMITER || recvbuf[recvbuf.size() - 1] != DELIMITER) { //sanity check
			std::cout << "double delimiter not found at end of recvbuf\n";
			std::cout << "size: " << recvbuf.size() << "\n";
			std::cout << "-2: " << recvbuf[recvbuf.size() - 2] << " 1: " << recvbuf[recvbuf.size() - 1] << "\n";
		}
	}

	for (std::pair<int, Entity::EntityData> idDataPair : idEntMap) {
		IDEntData idData = { idDataPair.first, idDataPair.second }; //{id, data}
		data.sceneUpdate.entUpdates.push_back(idData); //associate data to id strings
	}

	//std::cout << "returning\n";
	return data;
}

int Client::closeConnection(int how) {
	int iResult = shutdown(ConnectSocket, how);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
		return -1;
	}
	closesocket(ConnectSocket);
	WSACleanup();
	return iResult;
}

/***** legacy code *****/
/*int Client::recvData(char recvbuf[], int buflen, int flags) {
	int iResult = recv(ConnectSocket, recvbuf, buflen, flags);
	std::cout << "bytes received: " << iResult << "\n";
	if (iResult == SOCKET_ERROR) {
		// printf("send failed with error: %d\n", WSAGetLastError());
		std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}
	return iResult;
}*/