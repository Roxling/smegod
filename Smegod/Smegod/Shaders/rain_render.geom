#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

in vec3 vs_pos[];
in vec3 vs_seed[];
in vec3 vs_speed[];
in float vs_random[];
flat in uint vs_type[];

out vec3 gs_lightDir;
out vec3 gs_pointLightDir;
out vec3 gs_camVec;
out vec2 gs_tex;
flat out uint gs_type;
out float gs_random;

uniform vec3 g_TotalVel = vec3(0, -0.25, 0);
uniform mat4 view_projection;
uniform vec3 camera_pos;

uniform float g_SpriteSize = 1.0;

uniform vec3 dirLightPos = vec3(50,100,30);

void main()
{
	gs_type = vs_type[0];
    gs_random = vs_random[0];
       
	vec3 velVec = vs_speed[0]/30 + g_TotalVel;

	float height = g_SpriteSize/2.0;
    float width = height/10.0;

    velVec = normalize(velVec);
    vec3 camVec = camera_pos - vs_pos[0];
    vec3 camOnVelVecPlane = camera_pos - ((dot(camVec, velVec)) * velVec);
    vec3 projectedCamVec = camOnVelVecPlane - vs_pos[0];
    vec3 sideVec = normalize(cross(projectedCamVec, velVec));
    
    vec3 pos[4];
    pos[0] = vs_pos[0] - (sideVec * 0.5*width);
    pos[1] = pos[0] + (velVec * height);
    pos[2] = pos[0] + (sideVec * width);
    pos[3] = pos[1] + (sideVec * width );

    gl_Position = view_projection * vec4(pos[0], 1.0);
    gs_tex = vec2(0.0, 0.0);
	gs_camVec =  camera_pos - pos[0];
	gs_lightDir = dirLightPos - pos[0];
    EmitVertex();

    gl_Position = view_projection * vec4(pos[1], 1.0);
    gs_tex = vec2(0.0, 1.0);
	gs_camVec =  camera_pos - pos[1];
	gs_lightDir = dirLightPos - pos[1];
    EmitVertex();

    gl_Position = view_projection * vec4(pos[2], 1.0);
    gs_tex = vec2(1.0, 0.0);
	gs_camVec =  camera_pos - pos[2];
	gs_lightDir = dirLightPos - pos[2];
    EmitVertex();

    gl_Position = view_projection * vec4(pos[3], 1.0);
    gs_tex = vec2(1.0, 1.0);
	gs_camVec =  camera_pos - pos[3];
	gs_lightDir = dirLightPos - pos[3];
    EmitVertex();

    EndPrimitive();
}