#version 330 core

layout (points) in;
layout (points) out;
layout (max_vertices = 1) out;

uniform mat4 g_mProjection;
uniform mat4 view_projection;
uniform vec3 camera_pos;
uniform float g_Near; 
uniform float g_Far; 

in vec3 pos_in[];
in vec3 seed_in[];
in vec3 speed_in[];
in float random_in[];
in uint type_in[];

out vec3 pos;
out vec3 seed;
out vec3 speed;
out float random;
out uint type;

uniform float g_FrameRate;
uniform vec3 g_TotalVel = vec3(0, -0.25, 0);
uniform vec3 g_eyePos;   //eye in world space
uniform float g_heightRange = 30.0;

void main()
{
	pos = pos_in[0];
	seed = seed_in[0];
	speed = speed_in[0];
	random = random_in[0];
	type = type_in[0];

	if(moveParticles) {
         //move forward
         pos += speed.xyz/g_FrameRate + g_TotalVel.xyz;

         //if the particle is outside the bounds, move it to random position near the eye         
         if (pos.y <=  g_eyePos.y-g_heightRange ) {
            float x = seed.x + g_eyePos.x;
            float z = seed.z + g_eyePos.z;
            float y = seed.y + g_eyePos.y;
            pos = vec3(x,y,z);
         }
    }

	EmitVertex();
	EndPrimitive();
}