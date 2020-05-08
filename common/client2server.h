#pragma once

enum ClientActions {
	MOVE_FORWARD,
	MOVE_BACKWARDS,
	MOVE_LEFT,
	MOVE_RIGHT
};

struct PlayerInput {
	int move_x, move_z;
	float view_y_rot;
	char buildType; //1 = LASER_TYPE, 2 = CLAW_TYPE, 3 = SUPER_MINION_TYPE, no button pressed = NO_BUILD_TYPE
	int buildIntent; //-1 = cancel, 0 = neutral, 1 = confirm
	bool harvestResource; //e to harvest resource player is looking at
};