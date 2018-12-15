
precision mediump float;
in vec3 TexCoord;

out vec4 fragColor;
precision lowp sampler3D;
uniform samplerCube Skybox;


void main(void)
{
	fragColor = texture(Skybox, TexCoord);
}