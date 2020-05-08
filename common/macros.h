#ifndef _MACROS_H_
#define _MACROS_H_

#define DEFAULT_BUFLEN 512
#define SERVER_SENDBUFLEN 1024
#define SERVER_TICKRATE 20 // times per second
#define DEFAULT_PORT "27015"
#define NUM_PLAYERS 2 //change to 4 eventually //upper bound, no more than 15 (1 byte) because of Server recv
#define DELIMITER ','

#define ID_PLAYER_MIN 0
#define ID_PLAYER_MAX 999
#define ID_BASE_MIN 1000
#define ID_BASE_MAX 1999
#define ID_MINION_MIN 2000
#define ID_MINION_MAX 2999
#define ID_TOWER_MIN 3000
#define ID_TOWER_MAX 3999
#define ID_RESOURCE_MIN 4000
#define ID_RESOURCE_MAX 4999

#define BASE_TYPE 'b'
#define NO_BUILD_TYPE 'n'
#define MINION_TYPE 'm'
#define SUPER_MINION_TYPE 's'
#define TOWER_TYPE 't'
#define LASER_TYPE 'L'
#define CLAW_TYPE 'c'
//#define RESOURCE_TYPE 'r'
#define DUMPSTER_TYPE 'd'
#define RECYCLING_BIN_TYPE 'r'

#define PLAYER_HEALTH 200
#define BASE_HEALTH 2000
#define MINION_HEALTH 50
#define TOWER_HEALTH 500
#define RESOURCE_HEALTH 1

#define PLAYER_ATTACK 25
#define BASE_ATTACK 0
#define MINION_ATTACK 10
#define TOWER_ATTACK 20
#define RESOURCE_ATTACK 0

#endif