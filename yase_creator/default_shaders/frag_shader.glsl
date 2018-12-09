
precision mediump float;

in lowp vec2 TexCoord;

out lowp vec4 fragColor;

//uniform sampler2D ourTexture1;

void main(void) {
	fragColor = vec4(0.5, 0.5, 0.5, 1);// texture(ourTexture1, TexCoord);
}