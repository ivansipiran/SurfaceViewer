#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat3 m_3x3;

out vec2 TexCoords;
out vec4 position;
out vec3 new_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	position = model * vec4(aPos, 1.0);
	new_normal = normalize(m_3x3*aNormal);
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}