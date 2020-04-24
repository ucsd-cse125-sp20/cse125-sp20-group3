#undef UNICODE

#include "../common/macros.h"
#include "../common/player.h"
#include <string>
#include <map>
#include <iostream>
#include "Server.h"
#include "GameObject.h"
#include "../common/client2server.h"

int __cdecl main(void)
{
    std::cout << "starting..." << std::endl;
    int iResult;
	int iSendResult;
    char sendbuf[DEFAULT_BUFLEN] = "I'm server";
	const int RECV_BUFLEN = DEFAULT_BUFLEN * NUM_PLAYERS;
    char recvbuf[RECV_BUFLEN];

	Server* server = new Server();
	std::map<std::string, GameObject*> idMap;
    int next_id = 0;
	Player* player = new Player(mat4(1));
    idMap[std::to_string(next_id)] = player;
    next_id++;

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

            // Process data
			// read id, handle player input
            PlayerInput in = ((PlayerInput*)recvbuf)[0];
			//populate in with received data

			player->setMoveAndDir(in);

			//Update Game State
			player->update();

			//Send updated data back to clients
			float sendbufSize = 0;
            player->setData(sendbuf, 0);
            sendbufSize += sizeof(GameObject::GameObjectData);
			//std::cout << "sending " << sendbuf << std::endl;
            iSendResult = server->sendData(sendbuf, sendbufSize, 0);
            if (iSendResult == -1) {
                return 1;
            }
            
        }
        else if (iResult == 0) {
                
        }
        else  {
            return 1;
        }

    } while (1);// (iResult > 0);

    // shutdown the connection since we're done
    iResult = server->cleanup(SD_SEND);
    if (iResult == -1) {
        return 1;
    }

    return 0;
}
