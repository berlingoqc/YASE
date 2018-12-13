
precision mediump float;

in lowp vec2 TexCoord;

out lowp vec4 fragColor;

uniform sampler2D ourTexture1;

void main(void) {
	fragColor =  texture(ourTexture1, TexCoord);
}