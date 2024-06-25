#version 330 core
out vec4 FragColor;


in vec2 TexCoord;
uniform sampler2D texture1;
uniform bool showTexture;


void main()
{
    if (showTexture)
        FragColor = texture(texture1, TexCoord);
    else
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    
} 