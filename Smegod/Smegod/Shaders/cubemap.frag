#version 330 core
in vec3 texCoords;
out vec4 color;

uniform samplerCube skybox;

const float ambient = 0.7;
void main() {
    color = texture(skybox, texCoords);
	color.a = ambient;
}