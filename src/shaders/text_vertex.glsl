#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vec2(vertex.x, vertex.y), 0.0, 1.0);
   // gl_Position = projection * vec4(200, 200, 0.0, 0.0);
    TexCoords = vec2(vertex.z, vertex.w);
}