#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

in VS_OUT {
	vec3 pos;
	vec3 seed;
	vec3 speed;
	float random;
	uint type;
} gs_in[]; 

out vec4 pos;
out vec3 lightDir;
out vec3 pointLightDir;
out vec3 eyeVec;
out vec2 tex;
flat out uint type;
out float random;

uniform vec3 camera_pos;
uniform float g_Near; 
uniform float g_Far; 

uniform mat4 g_mWorldView;
uniform mat4 g_mWorldViewProj;
uniform mat4 g_mProjection;

uniform float g_FrameRate;
uniform float g_SpriteSize = 1.0;
uniform vec3 g_lightPos = vec3(10,10,0); //the directional light in world space 

uniform vec3 g_TotalVel = vec3(0, -0.25, 0);

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
const vec3 g_PointLightPos2 = vec3(-3.7,5.8,3.15);

bool cullSprite(vec3 position, float SpriteSize)
{
    vec4 vpos = g_mWorldView * vec4(position, 1.0);
    
    if( (vpos.z < (g_Near - SpriteSize )) || ( vpos.z > (g_Far + SpriteSize)) ) 
    {
        return true;
    }
    else 
    {
        vec4 ppos = g_mProjection * vpos;
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
    if(!cullSprite(gs_in[0].pos, 2*g_SpriteSize) && totalIntensity > 0 || true)
    {
        type = gs_in[0].type;
        random = gs_in[0].random;
       
        vec3 bpos[4]; //billboard position
        GenRainSpriteVertices(gs_in[0].pos.xyz, gs_in[0].speed.xyz/g_FrameRate + g_TotalVel, camera_pos, bpos);
        
        vec3 closestPointLight = g_PointLightPos;
        float closestDistance = length(g_PointLightPos - bpos[0]);
        if( length(g_PointLightPos2 - bpos[0]) < closestDistance )
           closestPointLight = g_PointLightPos2;
        
        pos = g_mWorldViewProj * vec4(bpos[0], 1.0);
        lightDir = g_lightPos - bpos[0];
        pointLightDir = closestPointLight - bpos[0];
        eyeVec = camera_pos - bpos[0];
        tex = g_texcoords[0];
		EmitVertex();

        pos = g_mWorldViewProj * vec4(bpos[1], 1.0);
        lightDir = g_lightPos - bpos[1];
        pointLightDir = closestPointLight - bpos[1];
        eyeVec = camera_pos - bpos[1];
        tex = g_texcoords[1];
		EmitVertex();

        pos = g_mWorldViewProj * vec4(bpos[2], 1.0);
        lightDir = g_lightPos - bpos[2];
        pointLightDir = closestPointLight - bpos[2];
        eyeVec = camera_pos - bpos[2];
        tex = g_texcoords[2];
		EmitVertex();

        pos = g_mWorldViewProj * vec4(bpos[3], 1.0);
        lightDir = g_lightPos - bpos[3];
        pointLightDir = closestPointLight - bpos[3];
        eyeVec = camera_pos - bpos[3];
        tex = g_texcoords[3];
		EmitVertex();

		EndPrimitive();
    }   
}