#ifndef SHADERS_H_INCLUDED
#define SHADERS_H_INCLUDED

static const char *PC_FRAG = "varying vec4 Frag_Color;void main(){    gl_FragColor = Frag_Color;}";
static const char *PC_VERT = "uniform mat4 ProjMtx;attribute vec2 Position;attribute vec4 Color;varying vec4 Frag_Color;void main(){    Frag_Color = Color;    gl_Position = ProjMtx * vec4(Position.xy, 0, 1);}";
static const char *PTC_FRAG = "varying vec2 Frag_TexCoord;varying vec4 Frag_Color;uniform sampler2D Texture;void main(){    vec4 diffuse = texture2D(Texture, Frag_TexCoord);    gl_FragColor = diffuse * Frag_Color;}";
static const char *PTC_VERT = "uniform mat4 ProjMtx;attribute vec2 Position;attribute vec2 TexCoord;attribute vec4 Color;varying vec2 Frag_TexCoord;varying vec4 Frag_Color;void main(){    gl_Position = ProjMtx * vec4(Position.xy, 0, 1);    Frag_TexCoord = TexCoord;    Frag_Color = Color;}";

#endif
