////////////////////////////////////////////////////////////////////////////////
// Filename: Light.vs
// GLSL texture vertex shader with texture coordinate input and a texture coordinate output.
// The texture coordinates use the vec2 type since it only contains two floats for the U and V coordinates.
// The texture coordinates are passed straight through to the pixel shader.
//
//There is a new input and output variable which is a 3 float normal vector.
//The normal vector is used for calculating the amount of light by using the angle between the
//direction of the normal and the direction of the light.
////////////////////////////////////////////////////////////////////////////////

#version 330


/////////////////////
// INPUT VARIABLES //
/////////////////////
in vec3 inputPosition;
in vec2 inputTexCoord;
in vec3 inputNormal;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
out vec2 texCoord;
out vec3 normal;


///////////////////////
// UNIFORM VARIABLES //
///////////////////////
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);
	gl_Position = viewMatrix * gl_Position;
	gl_Position = projectionMatrix * gl_Position;

	// Store the texture coordinates for the pixel shader.
	texCoord = inputTexCoord;

	//The normal vector for this vertex is calculated in world space and then normalized before being sent as input
	//into the pixel shader. Sometimes these need to be re-normalized inside the pixel shader due to the interpolation that occurs.

	//Calculate the normal vector against the world matrix only.
	normal = mat3(worldMatrix)* inputNormal;

	//Normalize the normal vector.
	normal = normalize(normal);
}