#ifndef SHADERS_H_INCLUDED
#define SHADERS_H_INCLUDED

//
//--- Position2, Color4
//
static const char *PC_VERT = 
"uniform mat4 ProjMtx;"

"attribute vec2 ws_Vector2;"
"attribute vec4 ws_Color;"

"varying vec4 Frag_Color;"

"void main()"
"{"
"   Frag_Color = ws_Color;"
"   gl_Position = ProjMtx * vec4(ws_Vector2.xy, 0, 1);"
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

"attribute vec2 ws_Vector2;"
"attribute vec2 ws_TexCoord;"
"attribute vec4 ws_Color;"

"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"void main()"
"{"
"   gl_Position = ProjMtx * vec4(ws_Vector2.xy, 0, 1);"
"   Frag_TexCoord = ws_TexCoord;"
"   Frag_Color = ws_Color;"
"}";

//
//--- ws_Vector3, Normal3, TexCoord2, Color4
//
static const char *PNTC_VERT =
"uniform mat4 ProjMtx;"

"attribute vec3 ws_Vector2;"
"attribute vec3 ws_Vector3;"
"attribute vec2 ws_TexCoord;"
"attribute vec4 ws_Color;"

"varying vec3 Frag_Normal;"
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"void main()"
"{"
"   gl_Position = ProjMtx * vec4(ws_Vector2.xyz, 1);"
"   Frag_Normal = ws_Vector3;"
"   Frag_TexCoord = ws_TexCoord;"
"   Frag_Color = ws_Color;"
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
