#undef UNICODE

#include "../common/macros.h"
#include "../common/player.h"
#include <string>
#include <map>
#include <iostream>
#include "Server.h"
#include "SceneManager_Server.h"
#include "../common/GameObject.h"
#include "../common/client2server.h"

int __cdecl main(void)
{
    std::cout << "starting..." << std::endl;
    int iResult;
	int iSendResult;
    char sendbuf[DEFAULT_BUFLEN] = "I'm server";
	//recvbuf structure: [(player number + received bytes + delimiter) for each player]
	const int RECV_BUFLEN = NUM_PLAYERS + (DEFAULT_BUFLEN * NUM_PLAYERS) + NUM_PLAYERS; //worst case, player num + default buflen + delim
    char recvbuf[RECV_BUFLEN];

	Server* server = new Server();
	SceneManager_Server* manager = new SceneManager_Server();
    bool firstMessage = true;

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

            if (firstMessage) {
                manager->resetClocks();
                firstMessage = false;
            }

            // Process data
			// read id, handle player input
			std::string s(recvbuf);
			//std::cout << "recvbuf: " << s << "\n";
			int ind = 0;
			for (int b = 0; b < NUM_PLAYERS; b++) {
				char player_num = recvbuf[ind];
				std::string player_str = std::string(1, player_num);
				ind++;

				//std::cout << "processing player " << player_num << "\n";

				if (manager->addPlayer(player_str)) { //if this player's communication is new, create new player
					char id_buf[1] = { player_num };
					server->sendDataPlayer(stoi(player_str), id_buf, 1, 0);
				}

				/*int move_x = ((int*)(recvbuf + ind))[0];
				int move_z = ((int*)(recvbuf + ind))[1];
				float view_y_rot = ((float*)(recvbuf + ind))[2];
				std::cout << "read recvbuf: x: " << move_x << " z: " << move_z << " y: " << view_y_rot << "\n";*/

				PlayerInput input = ((PlayerInput*)(recvbuf + ind))[0]; //TODO modify this to allow for build tower commands
				ind += sizeof PlayerInput;

				if (recvbuf[ind] == DELIMITER) {
					manager->processInput(std::string(1, player_num), input);
				}
				else {
					std::cout << "buf size mismatch in receive, delimiter not found when expected\n";
				}
				ind++;
			}

			//Update Game State
			manager->update();

			//Send updated data back to clients
			float sendbufSize = manager->encodeScene(sendbuf);
			//std::cout << "sendbufSize: " << sendbufSize << std::endl;
            iSendResult = server->sendDataAll(sendbuf, sendbufSize, 0);
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
