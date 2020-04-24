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
};