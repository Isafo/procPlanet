#version 330
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 MVP;

void main ()  {	
	// Convert position to clip coordinates and pass along to fragment shader
	gl_Position = MVP * vec4(Position, 1.0);
}