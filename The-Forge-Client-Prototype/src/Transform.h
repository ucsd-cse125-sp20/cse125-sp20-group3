#pragma once

#include <algorithm>

#include "Node.h"

class Transform : public Node {
public:
	mat4 M;
	std::vector<Node*> children;

	Transform(mat4 transformation);
	~Transform();

	void unload() override;

	void addChild(Node* child);
	void removeChild(Node* child);

	void update(float deltaTime) override;
	void updateTransformBuffer(BufferUpdateDesc& desc, mat4 parentTransform);
	void draw(Cmd* cmd) override;
};