#include "TimedTransform.h"

TimedTransform::TimedTransform() : TimedTransform(mat4::identity())
{
}

TimedTransform::TimedTransform(mat4 m) : Transform(m)
{
	active = false;
	this->time = 0.0f;
}

void TimedTransform::activate(float timeout)
{
	active = true;
	time = timeout;
}

void TimedTransform::update(float deltaTime)
{
	if (active) {
		time -= deltaTime;
		if (time <= 0.f) {
			active = false;
		}
	}

	Transform::update(deltaTime);
}
