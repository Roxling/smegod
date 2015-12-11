#version 420 core

layout (points) in;
layout (points) out;
layout (max_vertices = 1) out;

uniform vec3 camera_pos;
uniform float g_FrameRate;
uniform vec3 g_TotalVel;
uniform float g_heightRange;
uniform bool moveParticles;

in vec3 vs_pos[];
in vec3 vs_seed[];
in vec3 vs_speed[];
in float vs_random[];
flat in uint vs_type[];

out vec3 gs_pos;
out vec3 gs_seed;
out vec3 gs_speed;
out float gs_random;
flat out uint gs_type;

void main()
{
	gs_pos = vs_pos[0];
	gs_seed = vs_seed[0];
	gs_speed = vs_speed[0];
	gs_random = vs_random[0];
	gs_type = vs_type[0];

	if(moveParticles) {
         //move forward
         gs_pos += gs_speed.xyz/g_FrameRate + g_TotalVel.xyz;

         //if the particle is outside the bounds, move it to random position near the eye         
         if (gs_pos.y <=  camera_pos.y-g_heightRange ) {
            float x = gs_seed.x + camera_pos.x;
            float z = gs_seed.z + camera_pos.z;
            float y = gs_seed.y + camera_pos.y;
            gs_pos = vec3(x,y,z);
         }
    }

	EmitVertex();
	EndPrimitive();
}