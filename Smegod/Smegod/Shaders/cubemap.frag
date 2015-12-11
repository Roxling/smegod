#version 420 core
in vec3 texCoords;
layout (location = 0) out vec4 geometry_diffuse;


uniform samplerCube skybox;

const float ambient = 0.5;
void main() {
    geometry_diffuse = texture(skybox, texCoords);
	geometry_diffuse.a = ambient;
}