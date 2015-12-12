#version 420 core
layout (location = 0) out vec4 geometry_diffuse;
in vec3 texCoords;


uniform samplerCube skybox;

const float ambient = 0.5;
void main() {
    geometry_diffuse = texture(skybox, texCoords);
	geometry_diffuse.a = ambient;
}