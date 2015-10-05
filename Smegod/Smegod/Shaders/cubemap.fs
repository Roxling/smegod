#version 330 core
in vec3 texCoords;
out vec4 color;

uniform samplerCube skybox;

void main() {
    //color = vec4(normalize(texCoords), 1.0);
    
    color = texture(skybox, normalize(texCoords));
}