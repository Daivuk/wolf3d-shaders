#ifndef SHADERS_H_INCLUDED
#define SHADERS_H_INCLUDED

//
//--- Position2, Color4
//
static const char *PC_VERT = 
"uniform mat4 ProjMtx;"

"attribute vec2 Position;"
"attribute vec4 Color;"

"varying vec4 Frag_Color;"

"void main()"
"{"
"   Frag_Color = Color;"
"   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);"
"}"
;

static const char *PC_FRAG = 
"varying vec4 Frag_Color;"

"void main()"
"{"
"   gl_FragColor = Frag_Color;"
"}"
;

//
//--- Position2, TexCoord2, Color4
//
static const char *PTC_FRAG =
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"uniform sampler2D Texture;"

"void main()"
"{"
"   vec4 diffuse = texture2D(Texture, Frag_TexCoord);"
"   gl_FragColor = diffuse * Frag_Color;"
"}"
;

static const char *PTC_VERT =
"uniform mat4 ProjMtx;"

"attribute vec2 Position;"
"attribute vec2 TexCoord;"
"attribute vec4 Color;"

"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"void main()"
"{"
"   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);"
"   Frag_TexCoord = TexCoord;"
"   Frag_Color = Color;"
"}";

//
//--- Position3, Normal3, TexCoord2, Color4
//
static const char *PNTC_VERT =
"uniform mat4 ProjMtx;"

"attribute vec3 Position;"
"attribute vec3 Normal;"
"attribute vec2 TexCoord;"
"attribute vec4 Color;"

"varying vec3 Frag_Normal;"
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"void main()"
"{"
"   gl_Position = ProjMtx * vec4(Position.xyz, 1);"
"   Frag_Normal = Normal;"
"   Frag_TexCoord = TexCoord;"
"   Frag_Color = Color;"
"}"
;

static const char *PNTC_FRAG =
"varying vec3 Frag_Normal;"
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"uniform sampler2D Texture;"

"void main()"
"{"
"   vec4 diffuse = texture2D(Texture, Frag_TexCoord);"
"   if (diffuse.a < .3) discard;"
"   gl_FragColor = diffuse * Frag_Color;"
"}"
;

#endif
