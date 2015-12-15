#version 420 core

layout (points) in;
layout (points) out;
layout (max_vertices = 1) out;

layout (location = 0) out vec3 gs_pos;
layout (location = 1) out vec3 gs_seed;
layout (location = 2) out vec3 gs_speed;
layout (location = 3) out float gs_random;
layout (location = 4) out float gs_type;

in vec3 vs_pos[];
in vec3 vs_seed[];
in vec3 vs_speed[];
in float vs_random[];
in float vs_type[];

uniform vec3 u_camera_pos;
uniform float u_framerate;
uniform vec3 u_total_vel;
uniform float u_height_range;
uniform bool u_move_particles;

void main()
{
	gs_pos = vs_pos[0];
	gs_seed = vs_seed[0];
	gs_speed = vs_speed[0];
	gs_random = vs_random[0];
	gs_type = vs_type[0];

	if(u_move_particles) {
         //move forward
         gs_pos += (gs_speed.xyz + u_total_vel.xyz) / u_framerate;

         //if the particle is outside the bounds, move it to random position near the eye         
         if (gs_pos.y <=  u_camera_pos.y-u_height_range ) {
            float x = gs_seed.x + u_camera_pos.x;
            float z = gs_seed.z + u_camera_pos.z;
            float y = gs_seed.y + u_camera_pos.y;
            gs_pos = vec3(x,y,z);
         }
    }

	EmitVertex();
	EndPrimitive();
}