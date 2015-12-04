#version 330 core

layout (points) in;
layout (points) out;
layout (max_vertices = 1) out;

uniform vec3 camera_pos;
uniform float g_FrameRate;
uniform vec3 g_TotalVel;
uniform float g_heightRange;
uniform bool moveParticles;

in VS_OUT {
	vec3 pos;
	vec3 seed;
	vec3 speed;
	float random;
	uint type;
} gs_in[];

out vec3 pos;
out vec3 seed;
out vec3 speed;
out float random;
out uint type;

void main()
{
	pos = gs_in[0].pos;
	seed = gs_in[0].seed;
	speed = gs_in[0].speed;
	random = gs_in[0].random;
	type = gs_in[0].type;

	if(moveParticles) {
         //move forward
         pos += speed.xyz/g_FrameRate + g_TotalVel.xyz;

         //if the particle is outside the bounds, move it to random position near the eye         
         if (pos.y <=  camera_pos.y-g_heightRange ) {
            float x = seed.x + camera_pos.x;
            float z = seed.z + camera_pos.z;
            float y = seed.y + camera_pos.y;
            pos = vec3(x,y,z);
         }
    }

	EmitVertex();
	EndPrimitive();
}