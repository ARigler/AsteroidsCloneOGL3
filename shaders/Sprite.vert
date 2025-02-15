// Request GLSL 3.3
#version 330 core
#extension GL_ARB_separate_shader_objects : enable

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Attribute 0 is position, 1 is tex coords.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

// Add texture coordinate as output
out vec2 fragTexCoord;

void main()
{
	// Convert position to homogeneous coordinates
	vec4 pos = vec4(inPosition, 1.0);
	// Transform position to world space, then clip space
	gl_Position = pos * uWorldTransform * uViewProj;

	// Transform
	// Pass along the texture coordinate to frag shader
	fragTexCoord = inTexCoord;
}