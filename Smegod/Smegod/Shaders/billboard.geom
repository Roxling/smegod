#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 view_projection;
uniform vec3 camera_pos;

out vec2 texcoords;

void main()
{
	vec3 pos = gl_in[0].gl_Position.xyz;

	vec3 toCamera = normalize(camera_pos - pos);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up);

	pos -= (right * 0.5);
    gl_Position = view_projection * vec4(pos, 1.0);
    texcoords = vec2(0.0, 0.0);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = view_projection * vec4(pos, 1.0);
    texcoords = vec2(0.0, 1.0);
    EmitVertex();

    pos.y -= 1.0;
    pos += right;
    gl_Position = view_projection * vec4(pos, 1.0);
    texcoords = vec2(1.0, 0.0);
    EmitVertex();

    pos.y += 1.0;
    gl_Position = view_projection * vec4(pos, 1.0);
    texcoords = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}