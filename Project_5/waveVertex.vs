#version 330 core

uniform mat4 pMatrix;    
uniform mat4 mvMatrix;   

uniform float scale;  
uniform float time;
 
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
float height = 1.0f;
float movement = 1.0f; 
void main()
{
    vec4 a = vec4(aPos, 1.0); 
    float wave = time * movement + a.x;
    a.x = a.x * scale;
    a.y = height * sin(1 * sqrt(a.x * a.x + a.z * a.z) + wave);
    a.z = a.z * scale;
    TexCoords = aTexCoords;  
    gl_Position = pMatrix * mvMatrix * a;
}                                                    