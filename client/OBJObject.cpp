#include "OBJObject.h"
#include "Window.h"

OBJObject::OBJObject(std::string objFilename)	: pointSize(2) {
	parse(objFilename.c_str());

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo_v);
	glGenBuffers(1, &vbo_n);
	glGenBuffers(1, &vbo_t);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, vbo_v);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
		normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo_t);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * normals.size(),
		uvs.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);


	// Generate and bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

OBJObject::OBJObject() {
	//server-side objects don't need to parse files for models, so this constructor exists to allow objects
	//to skip the model parsing process
}

OBJObject::~OBJObject()
{
	// Delete the VBO and the VAO.
	// Failure to delete your VAOs, VBOs and other data given to OpenGL
	// is dangerous and may slow your program and cause memory leaks
	glDeleteBuffers(1, &vbo_v);
	glDeleteBuffers(1, &vbo_n);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void OBJObject::loadTexure(std::string filename)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void OBJObject::parse(const char* objFilename) {
	FILE* fp;
	float x, y, z;
	float r, g, b;
	GLuint iv0, iv1, iv2, in0, in1, in2, it0, it1, it2;
	char c1, c2;
	std::vector<GLuint> vInds, tInds, nInds;
	std::vector<glm::vec3> inPoints, inNormals;
	std::vector<glm::vec2> inUVs;

	fopen_s(&fp, objFilename, "rb");
	if (fp == NULL) { std::cerr << "error loading file " << objFilename << std::endl; exit(-1); }

	while ((c1 = fgetc(fp)) != EOF) {
		if ((c1 == 'v' || c1 == 'f') && (c2 = fgetc(fp)) != EOF) {
			if (c1 == 'v' && c2 == ' ') {
				fscanf_s(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				inPoints.push_back(glm::vec3(x, y, z));

			}
			else if (c1 == 'v' && c2 == 'n') {
				fscanf_s(fp, "%f %f %f", &x, &y, &z);
				inNormals.push_back(glm::vec3(x, y, z));
			}
			else if (c1 == 'v' && c2 == 't') {
				fscanf_s(fp, "%f %f", &x, &y);
				inUVs.push_back(glm::vec2(x, y));
			}
			else if (c1 == 'f' && c2 == ' ') {
				fscanf_s(fp, "%u/%u/%u %u/%u/%u %u/%u/%u", &iv0, &it0, &in0, &iv1, &it1, &in1, &iv2, &it2, &in2);
				vInds.push_back(iv0 - 1);
				vInds.push_back(iv1 - 1);
				vInds.push_back(iv2 - 1);
				tInds.push_back(it0 - 1);
				tInds.push_back(it1 - 1);
				tInds.push_back(it2 - 1);
				nInds.push_back(in0 - 1);
				nInds.push_back(in1 - 1);
				nInds.push_back(in2 - 1);
				//printf("%u/%u/%u %u/%u/%u %u/%u/%u\n", iv0, it0, in0, iv1, it1, in1, iv2, it2, in2);
			}
		}
	}

	for (unsigned i = 0; i < vInds.size(); i++) {
		points.push_back(inPoints[vInds[i]]);
		normals.push_back(inNormals[nInds[i]]);
		uvs.push_back(inUVs[tInds[i]]);
		indices.push_back(i);
	}

	//printf("vertices: %d\n", points.size());
	//printf("normals: %d\n", normals.size());
	//printf("faces: %d\n", indices.size());
}

void OBJObject::setTranslate(glm::vec3 position)
{
	model[3] = glm::vec4(position, 1.0f);
}

void OBJObject::setScaleRot(glm::vec3 scale, float deg, glm::vec3 axis)
{
	glm::mat4 scaleRot = glm::scale(glm::mat4(1), scale) * glm::rotate(glm::mat4(1), deg, axis);
	model[0] = scaleRot[0];
	model[1] = scaleRot[1];
	model[2] = scaleRot[2];
}

void OBJObject::setPositionDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
	glm::vec3 forward = glm::normalize(direction);
	glm::vec3 right = glm::cross(forward, up);

	model[0] = glm::vec4(-forward, 0);
	model[1] = glm::vec4(-right, 0);
	model[2] = glm::vec4(up, 0);
	model[3] = glm::vec4(position, 1);
}

void OBJObject::setPositionDirection(glm::vec3 position, glm::vec3 direction)
{
	setPositionDirection(position, direction, glm::vec3(0, 0, 1));
}

void OBJObject::draw(GLuint program)
{
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &Window::view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &Window::projection[0][0]);
	glUniform3fv(glGetUniformLocation(program, "color"), 1, &color[0]);

	glBindVertexArray(vao);
	
	if (texture) glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, (GLuint) (indices.size()), GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
}

void OBJObject::update()
{
	
}

void OBJObject::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = glm::rotate(model, glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}