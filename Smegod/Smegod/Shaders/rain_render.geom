#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

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


out vec4 pos;
out vec3 lightDir;
out vec3 pointLightDir;
out vec3 eyeVec;
out vec2 tex;
out uint type;
out float random;


uniform float g_FrameRate;
uniform float g_SpriteSize = 1.0;
uniform vec3 g_eyePos;   //eye in world space
uniform vec3 g_lightPos = vec3(10,10,0); //the directional light in world space 

uniform float g_PointLightIntensity = 2.0;
uniform float g_ResponsePointLight = 1.0;
uniform float dirLightIntensity = 1.0;
uniform float g_ResponseDirLight = 1.0;

const vec2 g_texcoords[4] = 
{ 
	vec2(0,1), 
	vec2(1,1),
	vec2(0,0),
	vec2(1,0),
};

const vec3 g_PointLightPos = vec3(3.7, 5.8, 3.15);

bool cullSprite( float3 position, float SpriteSize)
{
    vec4 vpos = vec4(position, 1.0) * g_mWorldView;
    
    if( (vpos.z < (g_Near - SpriteSize )) || ( vpos.z > (g_Far + SpriteSize)) ) 
    {
        return true;
    }
    else 
    {
        vec4 ppos = vpos * g_mProjection;
        float wext = ppos.w + SpriteSize;
        if( (ppos.x < -wext) || (ppos.x > wext) ||
            (ppos.y < -wext) || (ppos.y > wext) ) {
            return true;
        }
        else 
        {
            return false;
        }
    }
    
    return false;
}

void GenRainSpriteVertices(vec3 worldPos, vec3 velVec, vec3 eyePos, out vec3 outPos[4])
{
    float height = g_SpriteSize/2.0;
    float width = height/10.0;

    velVec = normalize(velVec);
    vec3 eyeVec = eyePos - worldPos;
    vec3 eyeOnVelVecPlane = eyePos - ((dot(eyeVec, velVec)) * velVec);
    vec3 projectedEyeVec = eyeOnVelVecPlane - worldPos;
    vec3 sideVec = normalize(cross(projectedEyeVec, velVec));
    
    outPos[0] =  worldPos - (sideVec * 0.5*width);
    outPos[1] = outPos[0] + (sideVec * width);
    outPos[2] = outPos[0] + (velVec * height);
    outPos[3] = outPos[2] + (sideVec * width );
}

void main()
{
	float totalIntensity = g_PointLightIntensity*g_ResponsePointLight + dirLightIntensity*g_ResponseDirLight;
    if(!cullSprite(pos_in[0], 2*g_SpriteSize) && totalIntensity > 0)
    {
        type = type_in[0];
        random = random_in[0];
       
        vec3 bpos[4];
        GenRainSpriteVertices(pos_in[0].xyz, speed_in[0].xyz/g_FrameRate + g_TotalVel, g_eyePos, bpos);
        
        vec3 closestPointLight = g_PointLightPos;
        float closestDistance = length(g_PointLightPos - bpos[0]);
        if( length(g_PointLightPos2 - bpos[0]) < closestDistance )
           closestPointLight = g_PointLightPos2;
        
        pos = vec4(bpos[0],1.0) * g_mWorldViewProj;
        lightDir = g_lightPos - bpos[0];
        pointLightDir = closestPointLight - bpos[0];
        eyeVec = g_eyePos - bpos[0];
        tex = g_texcoords[0];
		EmitVertex();

        pos = vec4(bpos[1],1.0) * g_mWorldViewProj;
        lightDir = g_lightPos - bpos[1];
        pointLightDir = closestPointLight - bpos[1];
        eyeVec = g_eyePos - bpos[1];
        tex = g_texcoords[1];
		EmitVertex();

        pos = vec4(bpos[2],1.0) * g_mWorldViewProj;
        lightDir = g_lightPos - bpos[2];
        pointLightDir = closestPointLight - bpos[2];
        eyeVec = g_eyePos - bpos[2];
        tex = g_texcoords[2];
		EmitVertex();

        pos = vec4(bpos[3],1.0) * g_mWorldViewProj;
        lightDir = g_lightPos - bpos[3];
        pointLightDir = closestPointLight - bpos[3];
        eyeVec = g_eyePos - bpos[3];
        tex = g_texcoords[3];
		EmitVertex();

		EndPrimitive();
    }   
}