#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform float movementTransform;

void main()
{
   gl_Position = vec4(aPos.x, movementTransform, aPos.z, 1.0f);
   ourColor = ourColor;
   TexCoord = aTexCoord;
}