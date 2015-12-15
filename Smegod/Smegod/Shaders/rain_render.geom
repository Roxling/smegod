#version 420 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

in vec3 vs_pos[];
in vec3 vs_seed[];
in vec3 vs_speed[];
in float vs_random[];
in float vs_type[];

out vec2 gs_tex;
out vec3 gs_pos;
out float gs_type;
out float gs_random;

uniform vec3 u_total_vel;
uniform mat4 u_view_projection;
uniform vec3 u_camera_pos;
uniform float u_sprite_size;

void main()
{
	gs_type = vs_type[0];
    gs_random = vs_random[0];
    gs_pos = vs_pos[0];
	   
	vec3 vel_vec = normalize(vs_speed[0]/30 + u_total_vel);

	float height = u_sprite_size/2.0;
    float width = height/10.0;

    vec3 cam_vec = u_camera_pos - vs_pos[0];
    vec3 cam_vel_vec_plane = u_camera_pos - ((dot(cam_vec, vel_vec)) * vel_vec);
    vec3 projected_cam_vec = cam_vel_vec_plane - vs_pos[0];
    vec3 side_vec = normalize(cross(projected_cam_vec, vel_vec));
    
    vec3 pos[4];
    pos[0] = vs_pos[0] - (side_vec * 0.5*width);
    pos[1] = pos[0] + (vel_vec * height);
    pos[2] = pos[0] + (side_vec * width);
    pos[3] = pos[1] + (side_vec * width );

    gl_Position = u_view_projection * vec4(pos[0], 1.0);
    gs_tex = vec2(0.0, 0.0);
    EmitVertex();

    gl_Position = u_view_projection * vec4(pos[1], 1.0);
    gs_tex = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = u_view_projection * vec4(pos[2], 1.0);
    gs_tex = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = u_view_projection * vec4(pos[3], 1.0);
    gs_tex = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}