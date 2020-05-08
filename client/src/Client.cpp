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
		//TODO handle failure
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
		//TODO handle failure
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			//TODO handle failure
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

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		//TODO handle failure
	}
}

int Client::sendData(char sendbuf[], int buflen, int flags) {
	//std::cout << "sending data\n";
	int iResult = send(ConnectSocket, sendbuf, buflen, flags);
	//std::cout << "sent\n";
	if (iResult == SOCKET_ERROR) {
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}
	return iResult;
}

char Client::recvPlayerID() {
	char idbuf[1];
	int iResult = 0;
	while (iResult == 0) {
		iResult = recv(ConnectSocket, idbuf, 1, 0); //receive 1 byte
		if (iResult == SOCKET_ERROR) {
			std::cout << "recv ID failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return '0';
		}
	}
	return idbuf[0];
}

std::vector<Client::UpdateData> Client::recvAndFormatData() {
	//std::cout << "recvAndFormatData\n";
	char size_int_buf[sizeof(int)];
	int bytes_to_receive;
	std::vector<char> recvbuf;
	std::map<std::string, Entity::EntityData> dataMap;
	int iResult;
	u_long blocking = 0, nonblocking = 1;
	
	while (1) { //receive until recv queue is empty
		iResult = ioctlsocket(ConnectSocket, FIONBIO, &nonblocking);
		if (iResult == SOCKET_ERROR) {
			std::cout << "ioctlsocket before receiving packet size failed with error: " << WSAGetLastError() << "\n";
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
				std::cout << "recv of size failed with error: " << code << std::endl;
				closesocket(ConnectSocket);
				WSACleanup();
				std::vector<UpdateData> empty;
				return empty;
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
			std::vector<UpdateData> empty;
			return empty;
		}
		//std::cout << "received " << iResult << " bytes\n";

		/* process and format data */
		std::string id_str = "";
		Entity::EntityData data;
		int state = 0;
		//std::cout << "processing\n";
		for (int i = 0; i < recvbuf.size(); i++) {
			if (state == 0) { //append bytes to id_str until delimiter encountered
				if (recvbuf[i] == DELIMITER) {
					state++; //end of id bytes found, advance to reading data
					//std::cout << "id_str: " + id_str + "\n";
				}
				else { //read id bytes one by one, appending to id_str
					id_str += recvbuf[i];
				}
			}
			else if (state == 1) { //read bytes as an EntityData
				char* recvData = &recvbuf[0];
				data = ((Entity::EntityData*)(recvData + i))[0];
				i += (sizeof Entity::EntityData); //advance i to where delimiter should be

				if (recvbuf[i] == DELIMITER) {
					//std::cout << "state 1 delimiter at i: " << i << "\n";
					dataMap[id_str] = data; //overwrite old data in the case of multiple updates on same object in one tick
					state = 0; //end of data confirmed, reset to reading id bytes
					id_str = "";
				}
				else {
					//std::cout << "state 1 delimiter expected but not found, i: " << i << "\n";
				}
			}
		}
		if (recvbuf[recvbuf.size() - 2] != DELIMITER || recvbuf[recvbuf.size() - 1] != DELIMITER) { //sanity check
			std::cout << "double delimiter not found at end of recvbuf\n";
		}
	}

	std::vector<UpdateData> updateDataVec; //format data into a vector to be returned
	for (std::pair<std::string, Entity::EntityData> idDataPair : dataMap) {
		UpdateData upData = { idDataPair.first, idDataPair.second }; //{id_str, data}
		updateDataVec.push_back(upData); //associate data to id strings
	}
	//std::cout << "returning\n";
	return updateDataVec;
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