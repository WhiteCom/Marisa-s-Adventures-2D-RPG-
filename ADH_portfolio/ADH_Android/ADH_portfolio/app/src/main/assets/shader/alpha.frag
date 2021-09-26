#ifdef GL_ES// Android
precision mediump float;
#define VARYING varying

#define FRAGCOLOR gl_FragColor

#else// Window
#version 150

#define VARYING in

out vec4 fragColor;
#define FRAGCOLOR fragColor

#endif

uniform sampler2D tex;

VARYING vec4 colorV;
VARYING vec2 texcoordV;

void main()
{
	FRAGCOLOR = texture2D(tex, texcoordV) * colorV;
}
