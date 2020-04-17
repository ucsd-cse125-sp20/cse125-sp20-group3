#undef UNICODE

//#include "network_connection.cpp"
#include "macros.h"
#include <string>
#include <iostream>
#include "Server.h"

int __cdecl main(void) 
{
    std::cout << "starting..." << std::endl;
    int iResult;
	int iSendResult;
    char sendbuf[DEFAULT_BUFLEN] = "I'm server";
	const int RECV_BUFLEN = DEFAULT_BUFLEN * NUM_PLAYERS;
    char recvbuf[RECV_BUFLEN];

	Server* server = new Server();

    // Game State data
    float currPosX = 0;
    float currPosY = 0;
    float currVelX = 0;
    float currVelY = 0;
    float acceleration = 1;
    float drag = 0.1;
    float deltaTime = 0.001f;

    std::cout << "server started" << std::endl;

    // Receive until the peer shuts down the connection
    do {
        ZeroMemory( recvbuf, RECV_BUFLEN );
        iResult = server->recvData(recvbuf, RECV_BUFLEN, 0);
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
            //f_buf[0] = currPosX;
            //f_buf[1] = currPosY;

			std::string s(sendbuf);
			std::cout << "sending " << s << std::endl;
            iSendResult = server->sendData(sendbuf, (int)strlen(sendbuf), 0);
            if (iSendResult == -1) {
                return 1;
            }
            // printf("Bytes sent: %d\n", iSendResult);
            std::cout << "Bytes sent: "<< iSendResult << std::endl;
        }
		else if (iResult == 0) {
			// printf("Connection closing...\n");
			std::cout << "Nothing received" << std::endl;
		}
        else  {
            return 1;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = server->cleanup(SD_SEND);
    if (iResult == -1) {
        return 1;
    }

    return 0;
}