#undef UNICODE

#include "network_connection.cpp"
#include <string>
#include <iostream>

int __cdecl main(void) 
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char sendbuf[DEFAULT_BUFLEN] = "I'm server";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int connection_result = set_up_server_socket(&wsaData, &ListenSocket, &ClientSocket, &iResult, result, &hints);
    if(connection_result == 1)
    {
        return 1;
    }

    // Game State data
    float currPosX = 0;
    float currPosY = 0;
    float currVelX = 0;
    float currVelY = 0;
    float acceleration = 1;
    float drag = 0.1;
    float deltaTime = 0.001f;


    // Receive until the peer shuts down the connection
    do {
        ZeroMemory( recvbuf, sizeof(recvbuf) );
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            // printf("Bytes received: %d\n", iResult);
            // printf("Message recieved: %s\n", recvbuf);
            std::cout<<"Bytes received: "<<iResult<<std::endl;
            std::cout<<"Message received: "<<recvbuf<<std::endl;


            // Process data
            currVelX -= drag * currVelX;
            currVelY -= drag * currVelY;
            if (recvbuf[0] == '1') {
                currVelY += acceleration * deltaTime;
            }
            if (recvbuf[1] == '1') {
                currVelX -= acceleration * deltaTime;
            }
            if (recvbuf[2] == '1') {
                currVelY -= acceleration * deltaTime;
            }
            if (recvbuf[3] == '1') {
                currVelX += acceleration * deltaTime;
            }

            if (currVelX * currVelX + currVelY * currVelY > 0.01) {
                currPosX += currVelX;
                currPosY += currVelY;
            }

            float* f_buf = (float*)sendbuf;
            f_buf[0] = currPosX;
            f_buf[1] = currPosY;


            iSendResult = send( ClientSocket, sendbuf, iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
                // printf("send failed with error: %d\n", WSAGetLastError());
                std::cout<<"send failed with error: "<<WSAGetLastError()<<std::endl;
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            // printf("Bytes sent: %d\n", iSendResult);
            std::cout<<"Bytes sent: "<<iSendResult<<std::endl;
        }
        else if (iResult == 0)
            // printf("Connection closing...\n");
            std::cout<<"Connection closing.."<<std::endl;
        else  {
            // printf("recv failed with error: %d\n", WSAGetLastError());
            std::cout<<"recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        // printf("shutdown failed with error: %d\n", WSAGetLastError());
        std::cout<<"shutdown failed with error: "<<WSAGetLastError()<<std::endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}