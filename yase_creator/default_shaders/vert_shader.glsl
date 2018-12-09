
precision mediump float;

in lowp vec3 in_sommet;
in lowp vec3 in_color;
in lowp vec2 texCoord;

uniform mat4 gVue;
uniform mat4 gModele;
uniform mat4 gProjection;

out lowp vec4 Color;
out lowp vec2 TexCoord;

void main()
{
	gl_Position = gProjection * gVue * gModele * vec4(in_sommet,1.0);
	TexCoord = vec2(0.0, 0.0);//vec2(texCoord.x, 1.0 - texCoord.y);
	Color = vec4(in_color, 1.0);
}