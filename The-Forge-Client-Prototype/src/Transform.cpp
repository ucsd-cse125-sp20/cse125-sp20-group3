#include "Transform.h"

Transform::Transform(mat4 transformation)
{
	this->M = transformation;
}

Transform::~Transform()
{
}

void Transform::unload()
{
	for (auto child : children) {
		child->unload();
	}
}

void Transform::addChild(Node* child)
{
	children.push_back(child);
}

void Transform::removeChild(Node* child)
{
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void Transform::update(float deltaTime)
{
	for (auto child : children) {
		child->update(deltaTime);
	}
}

void Transform::updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform)
{
	for (auto child : children) {
		child->updateTransformBuffer(desc, parentTransform * this->M);
	}
}

void Transform::draw(Cmd* cmd)
{
	for (auto child : children) {
		child->draw(cmd);
	}
}
