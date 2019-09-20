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

//
//--- Position3, Normal3, TexCoord2, Color4 but for g-buffer
//
static const char *PNTC_GBUFFER_VERT =
"uniform mat4 ProjMtx;"

"attribute vec3 Position;"
"attribute vec3 Normal;"
"attribute vec2 TexCoord;"
"attribute vec4 Color;"

"varying vec3 Frag_Normal;"
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"
"varying vec2 Frag_Depth;"

"void main()"
"{"
"   gl_Position = ProjMtx * vec4(vec3(Position.xy, Position.z*1.2), 1);"
"   Frag_Normal = Normal;"
"   Frag_TexCoord = TexCoord;"
"   Frag_Color = Color;"
"   Frag_Depth = gl_Position.zw;"
"}"
;

static const char *PNTC_GBUFFER_FRAG =
"varying vec3 Frag_Normal;"
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"
"varying vec2 Frag_Depth;"

"uniform sampler2D Texture;"

"void main()"
"{"
"   vec4 diffuse = texture2D(Texture, Frag_TexCoord);"
"   if (diffuse.a < .3) discard;"
"   gl_FragData[0] = diffuse * Frag_Color;"
"   gl_FragData[1] = vec4(normalize(Frag_Normal) * 0.5 + 0.5, 1);"
"   gl_FragData[2] = vec4(Frag_Depth.x / Frag_Depth.y, 0, 0, 1);"
"}"
;

//
//--- Position2, TexCoord2, Color4 - Point Light
//
static const char *PTC_POINTLIGHT_VERT =
"uniform mat4 ProjMtx;"
"uniform vec3 LightPosition;"
"uniform float LightRadius;"

"attribute vec3 Position;"

"varying vec4 Frag_Position;"

"void main()"
"{"
"   gl_Position = ProjMtx * vec4(Position * LightRadius + LightPosition, 1);"
"   Frag_Position = gl_Position;"
"}"
;

static const char *PTC_POINTLIGHT_FRAG =
"uniform sampler2D AlbeoTexture;"
"uniform sampler2D NormalTexture;"
"uniform sampler2D DepthTexture;"

"uniform mat4 InvProjMtx;"
"uniform vec3 LightPosition;"
"uniform float LightRadius;"
"uniform float LightIntensity;"
"uniform vec4 LightColor;"

"varying vec4 Frag_Position;"

"void main()"
"{"
"   vec2 texCoord = Frag_Position.xy / Frag_Position.w * 0.5 + 0.5;"
"   texCoord.y = 1 - ((1 - texCoord.y) * 160 / 200);"
"   gl_FragColor = vec4(texCoord, 0, 1);"
"   vec4 gAlbeo = texture2D(AlbeoTexture, texCoord);"
"   vec4 gNormal = texture2D(NormalTexture, texCoord);"
"   vec4 gDepth = texture2D(DepthTexture, texCoord);"

// Position
"   vec4 position = vec4(Frag_Position.xy / Frag_Position.w, gDepth.r, 1);"
"   position = position * InvProjMtx;"
"   position /= position.w;"

// Normal
"   vec3 normal = gNormal.xyz * 2 - 1;"
"   vec3 dir = LightPosition - position.xyz;"

// Attenuation stuff
"   float dis = length(dir);"
"   float disSqr = dis * dis;"
"   disSqr /= LightRadius * LightRadius;"
"   float dotNormal = dot(normal, dir) / dis;"
"   dotNormal = 1 - (1 - dotNormal) * (1 - dotNormal);"
"   float intensity = clamp(1 - disSqr, 0, 1);"
"   dotNormal = clamp(dotNormal, 0, 1);"
"   intensity *= dotNormal;"

"   gl_FragColor = gAlbeo * intensity * LightIntensity * LightColor;"
"}"
;

//
//--- Position2, TexCoord2, Color4 - HDR
//
static const char *PTC_HDR_VERT =
"uniform mat4 ProjMtx;"

"attribute vec2 Position;"
"attribute vec2 TexCoord;"
"attribute vec4 Color;"

"varying vec2 Frag_Position;"
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"void main()"
"{"
"   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);"
"   Frag_Position = gl_Position.xy;"
"   Frag_TexCoord = TexCoord;"
"   Frag_Color = Color;"
"}"
;

static const char *PTC_HDR_FRAG =
"uniform sampler2D HDRTexture;"

"uniform float LumMultiplier;"

"varying vec2 Frag_Position;"
"varying vec2 Frag_TexCoord;"
"varying vec4 Frag_Color;"

"void main()"
"{"
"   vec4 hdr = texture2D(HDRTexture, Frag_TexCoord);"

"   hdr.rgb = pow(hdr.rgb, vec3(1.1));"
"   hdr.rgb = hdr.rgb * LumMultiplier;"

"   gl_FragColor = hdr;"
"}"
;

#endif
