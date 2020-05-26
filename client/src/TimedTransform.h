#pragma once

#include "Transform.h"

class TimedTransform : public Transform {
public:
	float time = 0.0f;

	TimedTransform();
	TimedTransform(mat4 m);

	void activate(float timeout);

	void update(float deltaTime) override;
};