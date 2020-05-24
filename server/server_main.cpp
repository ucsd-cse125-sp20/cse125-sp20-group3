#undef UNICODE

//#include <Windows.h>
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

	SceneManager_Server* manager = new SceneManager_Server();
	Server* server = new Server(manager);

    // Game State data
	std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
	float deltaTime;

    std::cout << "server started" << std::endl;

    // Receive until the peer shuts down the connection
    do {
        //Standard server game loop logic:
        // Receive input from all clients
        // Update game states
        // send updated state to all clients
        // wait until end of time

		std::chrono::steady_clock::time_point beginTick = std::chrono::steady_clock::now();

		std::vector<PlayerInput> inputs = server->pullData();

		/* Process player input */
		for (int p = 0; p < inputs.size(); p++) {
			//std::cout << "processing player " << p << "\n";

			manager->processInput(p, inputs[p]);
			//TODO check for ending game?
		}

		/* Update Game State */
		auto currTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> deltaDuration = currTime - lastTime;
		deltaTime = deltaDuration.count();
		//std::cout << "deltaTime: " << deltaTime << "\n";
		manager->update(deltaTime);
		lastTime = std::chrono::steady_clock::now();

		/* Send updated data back to clients */
		int statebufSize = manager->encodeState(sendbuf, 0);
		int sendbufSize = manager->encodeScene(sendbuf, statebufSize) + statebufSize;
		std::cout << "sendbufSize: " << sendbufSize << std::endl;
		char sizebuf[4];
		((int*)sizebuf)[0] = sendbufSize; //push size of data packet to players
		server->pushDataAll(sizebuf, sizeof(int), 0); //and then push data packet
		server->pushDataAll(sendbuf, sendbufSize, 0);

		/* wait until end of tick */
		std::chrono::steady_clock::time_point endTick = std::chrono::steady_clock::now();
		std::chrono::duration<float> tickDuration = endTick - beginTick;
		float tickTime = tickDuration.count();
		//std::cout << "tickTime: " << tickTime << "\n";
		float sleepSeconds = max((1.0f / SERVER_TICKRATE) - tickTime, 0);
		//std::cout << "sleeping for " << sleepSeconds << " seconds\n";
		Sleep(sleepSeconds * 1000); //Sleep only accepts milliseconds
    } while (server->gameInProgress());

	std::cout << "all players disconnected, game over\n";

    // shutdown the connection since we're done
    server->end_game();
    iResult = server->cleanup(SD_SEND);
    if (iResult == -1) {
        return 1;
    }

	while (1) {
		//hold before returning so that the window stays open for debugging
	}

    return 0;
}
