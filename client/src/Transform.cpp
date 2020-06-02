#include "Transform.h"

Transform::Transform()
{
	this->M = mat4::identity();
}

Transform::Transform(mat4 transformation)
{
	this->M = transformation;
}

Transform::~Transform()
{
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
	if (active) {
		for (auto child : children) {
			child->update(deltaTime);
		}
	}
}

void Transform::updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform, vec4 color)
{
	if (active) {
		for (auto child : children) {
			vec4 newColor = vec4(this->color[0] * color[0], this->color[1] * color[1], this->color[2] * color[2], this->color[3] * color[3]);
			child->updateTransformBuffer(desc, parentTransform * this->M, newColor);
		}
	}
}

void Transform::updateBoneBuffer(BufferUpdateDesc& desc, OzzObject::UniformDataBones* boneData)
{
	if (active) {
		for (auto child : children) {
			child->updateBoneBuffer(desc, boneData);
		}
	}
}

void Transform::updateParticleBuffer(BufferUpdateDesc& desc)
{
	if (active) {
		for (auto child : children) {
			child->updateParticleBuffer(desc);
		}
	}
}

void Transform::cull(const vec4 planes[6], bool doCull)
{
	if (active) {
		for (auto child : children) {
			child->cull(planes, doCull);
		}
	}
}

void Transform::draw(Cmd* cmd)
{
	if (active) {
		for (auto child : children) {
			child->draw(cmd);
		}
	}
}

void Transform::setMatrix(mat4 m)
{
	this->M = m;
}

void Transform::setPositionDirection(vec3 position, vec3 direction, vec3 up)
{
	vec3 forward = normalize(direction);
	vec3 right = cross(forward, up);

	this->M[0] = vec4(-right, 0);
	this->M[1] = vec4(cross(forward, right), 0);
	this->M[2] = vec4(forward, 0);
	this->M[3] = vec4(position, 1);
}

void Transform::setPositionDirection(vec3 position, vec3 direction)
{
	setPositionDirection(position, direction, vec3(0, 1, 0));
}

void Transform::setPositionDirection(vec3 position, float angle)
{
	vec3 direction = vec3(cos(angle), 0, sin(angle));
	setPositionDirection(position, direction);
}

void Transform::setColor(vec4 c)
{
	color = c;
}
