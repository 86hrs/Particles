#version 410 core
layout (location = 0) in vec2 aPos;       // Single point vertex
layout (location = 1) in vec2 aOffset;    // Per-instance offset

void main()
{
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    gl_PointSize = 4.0; // Size of the point
}
