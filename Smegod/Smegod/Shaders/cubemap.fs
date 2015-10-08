#version 330 core
in vec3 texCoords;
out vec4 color;

uniform samplerCube skybox;

void main() {
    color = vec4(1,0,0,1);
    color = texture(skybox, texCoords);
}