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
    char sendbuf[SERVER_SENDBUFLEN] = "I'm server";
	//recvbuf structure: [(player number + received bytes + delimiter) for each player]
	//const int RECV_BUFLEN = NUM_PLAYERS + (DEFAULT_BUFLEN * NUM_PLAYERS) + NUM_PLAYERS; //worst case, player num + default buflen + delim
    //char recvbuf[RECV_BUFLEN];

	SceneManager_Server* manager = new SceneManager_Server();
	Server* server = new Server(manager);
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

		std::vector<PlayerInput> inputs = server->pullData();

		if (firstMessage) {
			manager->resetClocks();
			firstMessage = false;
		}

		// Process data
		for (int p = 0; p < inputs.size(); p++) {
			std::string player_str = std::to_string(p);
			//std::cout << "processing player " << p << "\n";

			//manager->addPlayer(player_str) now handled by Server on accept

			manager->processInput(player_str, inputs[p]);
			//TODO check for ending game?
		}

		//Update Game State
		manager->update();

		//Send updated data back to clients
		float sendbufSize = manager->encodeScene(sendbuf);
		//std::cout << "sendbufSize: " << sendbufSize << std::endl;
		server->pushDataAll(sendbuf, sendbufSize, 0);
    } while (1);

    // shutdown the connection since we're done
    server->end_game();
    iResult = server->cleanup(SD_SEND);
    if (iResult == -1) {
        return 1;
    }

    return 0;
}
