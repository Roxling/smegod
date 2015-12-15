#version 420 core
layout (location = 0) out vec4 geometry_diffuse;
in vec3 texcoords;

uniform samplerCube skybox;

#define AMBIENT (0.5)
void main() {
    geometry_diffuse = texture(skybox, texcoords);
	geometry_diffuse.a = AMBIENT;
}