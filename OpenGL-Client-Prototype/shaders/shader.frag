#version 330 core

uniform vec3 color;
uniform sampler2D tex;

//in vec3 FragPos;
//in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{    
	FragColor = vec4(color, 1) * texture(tex, TexCoord);
	//FragColor = vec4(TexCoord, 0, 1);
}