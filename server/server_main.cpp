#undef UNICODE

#include "../common/macros.h"
#include "../common/player.h"
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
	Player* player = new Player(mat4(1));

    // Game State data
    float deltaTime = 0.001f;

    std::cout << "server started" << std::endl;

    // Receive until the peer shuts down the connection
    do {
        //Standard server game loop logic:
        // Receive input from all clients
        // Update game states
        // send updated state to all clients
        // wait until end of time
        ZeroMemory( recvbuf, RECV_BUFLEN );
        iResult = server->recvData(recvbuf, RECV_BUFLEN, 0);
        if (iResult > 0) {
            // printf("Bytes received: %d\n", iResult);
            // printf("Message recieved: %s\n", recvbuf);
            std::cout<<"Bytes received: "<<iResult<<std::endl;
            std::cout<<"Message received: "<<recvbuf<<std::endl;

            // Process data
			int move_x, move_z = 0;
            if (recvbuf[0] == '1') {
				move_z = 1;
            }
            if (recvbuf[1] == '1') {
				move_x = -1;
            }
            if (recvbuf[2] == '1') {
				move_z = -1;
            }
            if (recvbuf[3] == '1') {
				move_x = 1;
            }

			player->setMove(move_x, move_z);

			//Update Game State
			player->update();

			//Send updated data back to clients
			float sendbufSize = 0;
			//sendbuf[0] = player->getPositionAndRotation
			//sendbufSize += number of bytes filled in
			std::cout << "sending " << sendbuf << std::endl;
            iSendResult = server->sendData(sendbuf, sendbufSize, 0);
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
