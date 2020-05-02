#ifndef _MACROS_H_
#define _MACROS_H_

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define NUM_PLAYERS 1 //change to 4 eventually //upper bound, no more than 15 (1 byte) because of Server recv
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
#define MINION_TYPE 'm'
#define TOWER_TYPE 't'
#define RESOURCE_TYPE 'r'

#endif