#ifndef _MACROS_H_
#define _MACROS_H_

#define SERVER_NAME "137.110.111.142"

#define DEFAULT_BUFLEN 512
#define SERVER_SENDBUFLEN 512000
#define USE_SMALL_DATA
#define SERVER_TICKRATE 20 // times per second
#define DEFAULT_PORT "27015"
#define NUM_PLAYERS 4 //change to 4 eventually //upper bound, no more than 15 (1 byte) because of Server recv
#define DELIMITER ','

#define NO_TARGET_ID -1

#define ACTION_STATE_IDLE 0
#define ACTION_STATE_MOVE 1
#define ACTION_STATE_ATTACK 2
#define ACTION_STATE_FIRE 3

#define RED_TEAM 'R'
#define BLUE_TEAM 'B'
#define NO_TEAM 'N'
#define RED_TEAM_SMALL 0
#define BLUE_TEAM_SMALL 1
#define NO_TEAM_SMALL 2

#define ID_PLAYER_MIN 0
#define ID_PLAYER_MAX 95
#define ID_BASE_MIN 96
#define ID_BASE_MAX 99
#define ID_MINION_MIN 100
#define ID_MINION_MAX 599
#define ID_SUPER_MINION_MIN 600
#define ID_SUPER_MINION_MAX 1099
#define ID_LASER_MIN 1100
#define ID_LASER_MAX 1299
#define ID_CLAW_MIN 1300
#define ID_CLAW_MAX 1499
#define ID_DUMPSTER_MIN 1500
#define ID_DUMPSTER_MAX 1699
#define ID_RECYCLING_BIN_MIN 1700
#define ID_RECYCLING_BIN_MAX 1899
#define ID_IRON_MIN 1900
#define ID_IRON_MAX 2199
#define ID_BOTTLE_MIN 2200
#define ID_BOTTLE_MAX 2499

//length is z, width is x
#define PLAYER_LENGTH 0.2f //checked
#define PLAYER_WIDTH 0.5f
#define BASE_LENGTH 0.5f
#define BASE_WIDTH 0.5f
#define MINION_LENGTH 0.5f //checked
#define MINION_WIDTH 0.5f
#define SUPER_MINION_LENGTH 0.9f //checked
#define SUPER_MINION_WIDTH 0.75f
#define LASER_LENGTH 1.5f //checked
#define LASER_WIDTH 1.5f
#define CLAW_LENGTH 1.0f //checked
#define CLAW_WIDTH 1.0f
#define DUMPSTER_LENGTH 1.8f //checked
#define DUMPSTER_WIDTH 1.0f
#define RECYCLING_BIN_LENGTH 0.4f //checked kinda
#define RECYCLING_BIN_WIDTH 0.4f
#define PICKUP_LENGTH 0.5f
#define PICKUP_WIDTH 0.5f

#define BUILD_CONFIRM 1
#define BUILD_CANCEL -1

#define BASE_TYPE 'b'
#define NO_BUILD_TYPE 'n'
#define MINION_TYPE 'm'
#define SUPER_MINION_TYPE 's'
#define TOWER_TYPE 't'
#define LASER_TYPE 'l'
#define CLAW_TYPE 'c'
#define DUMPSTER_TYPE 'd'
#define RECYCLING_BIN_TYPE 'r'
#define IRON_TYPE 'i'
#define BOTTLE_TYPE 'p' //p for plastic to avoid overlap

#define METAL_RES_TYPE 'M'
#define PLASTIC_RES_TYPE 'P'

#define STARTING_METAL 5
#define STARTING_PLASTIC 5

#define PLAYER_HEALTH 200
#define BASE_HEALTH 2000
#define MINION_HEALTH 50
#define SUPER_MINION_HEALTH 1500
#define LASER_TOWER_HEALTH 1000
#define CLAW_TOWER_HEALTH 50
#define RESOURCE_HEALTH 10
#define PICKUP_HEALTH 10

#define PLAYER_ATTACK 25
#define BASE_ATTACK 0
#define MINION_ATTACK 10
#define SUPER_MINION_ATTACK 100
#define LASER_TOWER_ATTACK 20
#define CLAW_TOWER_ATTACK 0
#define RESOURCE_ATTACK 0
#define PICKUP_ATTACK 0

#define MINION_ATK_RANGE 7
#define SUPER_MINION_ATK_RANGE 2
#define LASER_FIRE_RANGE 25

#define MINION_ATK_INTERVAL 1.0f
#define SUPER_MINION_ATK_INTERVAL 3.0f
#define LASER_FIRE_INTERVAL 1.5f
#define CLAW_SPAWN_INTERVAL 5.0f
#define PICKUP_TIMEOUT_INTERVAL 15.0f

#define MINION_VELOCITY 3.0
#define SUPER_MINION_VELOCITY 2.0

#define DUMPSTER_PLASTIC 2
#define RECYCLING_BIN_METAL 4
#define BOTTLE_PLASTIC 1
#define IRON_METAL 1

#define SUPER_MINION_METAL_REQ 6
#define SUPER_MINION_PLASTIC_REQ 10
#define LASER_METAL_REQ 4
#define LASER_PLASTIC_REQ 0
#define CLAW_METAL_REQ 5
#define CLAW_PLASTIC_REQ 5

//drop chance 1/x
#define MINION_IRON_DROP_CHANCE 8
#define MINION_BOTTLE_DROP_CHANCE 10
#define SUPER_MINION_IRON_DROP_CHANCE 2
#define SUPER_MINION_BOTTLE_DROP_CHANCE 2

#define DROP_RANGE 2

#define PLASTIC_UI_ICON "resource_plastic"
#define METAL_UI_ICON "resource_metal"

#define PLASTIC_UI_TEXT "plastic_text"
#define METAL_UI_TEXT "metal_text"

#define MINION_UI_ICON "minion"
#define LASER_TOWER_UI_ICON "laser_tower"
#define CLAW_MACHINE_UI_ICON "claw_machine"
#define SUPER_MINION_UI_ICON "super_minion"

#define TEAM_TEXT "team_text"
#define TEAM_BLUE_HEALTH "health_bar_blue"
#define TEAM_RED_HEALTH "health_bar_red"

#define HEALTH_BAR_BLUE_TEAM_DEDUCTED "health_bar_blue_deducted"
#define HEALTH_BAR_RED_TEAM "health_bar_red"

#define DEFEAT_TEXT "defeat_text"
#define VICTORY_TEXT "victory_text"

#endif
