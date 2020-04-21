
#include "network_connection.cpp"
#include <iostream>
#include <string>

int __cdecl main(int argc, char **argv) 
{
    // Validate the parameters
    if (argc != 2) {
        // printf("usage: %s server-name\n", argv[0]);
        std::cout << "usage: " << argv[0] << " server-name" << std::endl;
        return 1;
    }

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char sendbuf[DEFAULT_BUFLEN] = "I'm client";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    int connection_result = set_up_client_socket(argv[1], &wsaData, &ConnectSocket, result, ptr, &hints, &iResult);
    if( connection_result == 1)
    {
        return 1;
    }
    
    if( ConnectSocket == INVALID_SOCKET){
        std::cout<<"invalid socket"<<std::endl;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        // printf("send failed with error: %d\n", WSAGetLastError());
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // printf("Bytes Sent: %ld\n", iResult);
    std::cout << "Bytes Sent " << iResult << std::endl;    

    // Receive until the peer closes the connection
    while(1) {
        // Read user input ----------------------------------------------------
        std::cout << "Enter message to sent: " << std:: endl;
        std::cin >> sendbuf;
        if(strcmp(sendbuf, "quit") == 0) break;
        // End of read user input ---------------------------------------------

        // Send user input to server ------------------------------------------
        iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
        if (iResult == SOCKET_ERROR) {
            // printf("send failed with error: %d\n", WSAGetLastError());
            std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        // End of send user input to server -----------------------------------

        // Receive result from server -----------------------------------------
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 ){
            // printf("Bytes received: %d\n", iResult);
            // printf("Message recived: %s\n", recvbuf);
            std::cout << "Bytes received: " << iResult << std::endl;
            std::cout << "Message received: " << recvbuf << std::endl;
        }
        else if ( iResult == 0 )
        {
            // printf("Connection closed\n");
            std::cout << "Connection closed" << std::endl;
        }
        else
        {
            // printf("recv failed with error: %d\n", WSAGetLastError());
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
        }
        // End of receive result from server ----------------------------------

        //

    }

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );


    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}