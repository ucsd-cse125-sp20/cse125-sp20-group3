#define WIN32_LEAN_AND_MEAN

#pragma once

#define _WIN32_WINNT  0x501


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int set_up_server_socket(WSADATA* WSAData_ptr, SOCKET* ListenSocket_ptr, SOCKET* ClientSocket_ptr,
    int* iResult_ptr, struct addrinfo* result, struct addrinfo* hints_ptr);

int set_up_client_socket(char* server_name, WSADATA* wsaData_ptr, SOCKET* ConnectSocket_ptr,
    struct addrinfo* result, struct addrinfo* ptr, struct addrinfo* hints_ptr, int* iResult_ptr);