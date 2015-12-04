#version 330 core
in vec3 texCoords;
out vec4 color;

uniform samplerCube skybox;

const vec3 ambient = vec3(0.7);
void main() {
    color = texture(skybox, texCoords);
	color.rgb *= ambient;
	color.w = 1;
}