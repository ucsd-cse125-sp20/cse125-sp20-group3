#define WIN32_LEAN_AND_MEAN

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

int set_up_server_socket(WSADATA *WSAData_ptr, SOCKET *ListenSocket_ptr, SOCKET *ClientSocket_ptr,
    int *iResult_ptr, struct addrinfo *result, struct addrinfo *hints_ptr)
{
    // WSADATA wsaData = *WSAData_ptr;
    // int iResult = *iResult_ptr;

    // SOCKET ListenSocket = *LisentnerSocket_ptr;
    // SOCKET ClientSocket = *ClientSocket_ptr;

    // // struct addrinfo *result = NULL;
    // struct addrinfo hints = *hints_ptr;
    
    // Initialize Winsock
    *iResult_ptr = WSAStartup(MAKEWORD(2,2), WSAData_ptr);
    if (*iResult_ptr != 0) {
        printf("WSAStartup failed with error: %d\n", *iResult_ptr);
        return 1;
    }

    ZeroMemory(hints_ptr, sizeof(*hints_ptr));
    hints_ptr->ai_family = AF_INET;
    hints_ptr->ai_socktype = SOCK_STREAM;
    hints_ptr->ai_protocol = IPPROTO_TCP;
    hints_ptr->ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    *iResult_ptr = getaddrinfo(NULL, DEFAULT_PORT, hints_ptr, &result);
    if ( *iResult_ptr != 0 ) {
        printf("getaddrinfo failed with error: %d\n", *iResult_ptr);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    *ListenSocket_ptr = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (*ListenSocket_ptr == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    *iResult_ptr = bind( *ListenSocket_ptr, result->ai_addr, (int)result->ai_addrlen);
    if (*iResult_ptr == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(*ListenSocket_ptr);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    *iResult_ptr = listen(*ListenSocket_ptr, SOMAXCONN);
    if (*iResult_ptr == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(*ListenSocket_ptr);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    *ClientSocket_ptr = accept(*ListenSocket_ptr, NULL, NULL);
    if (*ClientSocket_ptr == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(*ListenSocket_ptr);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(*ListenSocket_ptr);
}

int set_up_client_socket(char* server_name, WSADATA *wsaData_ptr, SOCKET *ConnectSocket_ptr, 
    struct addrinfo *result, struct addrinfo *ptr, struct addrinfo *hints_ptr, int *iResult_ptr)
{
    // WSADATA wsaData = *wsaData_ptr;
    
    // struct addrinfo hints = *hints_ptr;
    // int iResult = *iResult_ptr;
    // int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    *iResult_ptr = WSAStartup(MAKEWORD(2,2), wsaData_ptr);
    if (*iResult_ptr != 0) {
        printf("WSAStartup failed with error: %d\n", *iResult_ptr);
        return 1;
    }

    ZeroMemory( hints_ptr, sizeof(*hints_ptr) );
    hints_ptr->ai_family = AF_UNSPEC;
    hints_ptr->ai_socktype = SOCK_STREAM;
    hints_ptr->ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    *iResult_ptr = getaddrinfo(server_name, DEFAULT_PORT, hints_ptr, &result);
    if ( *iResult_ptr != 0 ) {
        printf("getaddrinfo failed with error: %d\n", *iResult_ptr);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        *ConnectSocket_ptr = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (*ConnectSocket_ptr == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        *iResult_ptr = connect( *ConnectSocket_ptr, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (*iResult_ptr == SOCKET_ERROR) {
            closesocket(*ConnectSocket_ptr);
            *ConnectSocket_ptr = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (*ConnectSocket_ptr == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    return 0;
}