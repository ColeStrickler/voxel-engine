#version 330 core
out vec4 FragColor;



uniform float opacity;


void main()
{
    FragColor = vec4(opacity, 0.5f, 0.2f, 1.0f);
} 