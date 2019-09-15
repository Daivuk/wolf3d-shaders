// Wolf3D Shaders specific functions
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <cinttypes>
#include <vector>
#include <string>
#include <cassert>
#include <map>

#include <GL/gl3w.h>
#include <SDL.h>

#include "ID_HEADS.H"

#include "ws_Vector2.h"
#include "ws_Vector3.h"
#include "ws_Matrix.h"

typedef int16_t(*Interrupt)(void);

struct ws_RenderTarget
{
    GLuint handle;
    GLuint frameBuffer;
    GLuint depth;
};

struct ws_Color
{
    float r, g, b, a;
};

struct ws_TexCoord
{
    float u, v;
};

struct ws_VertexPC
{
    ws_Vector2 position;
    ws_Color color;
};

struct ws_VertexPTC
{
    ws_Vector2 position;
    ws_TexCoord texCoord;
    ws_Color color;
};

struct ws_VertexPNTC
{
    ws_Vector3 position;
    ws_Vector3 normal;
    ws_TexCoord texCoord;
    ws_Color color;
};

struct ws_Resources
{
    GLuint programPC;          /* Position2, Color4 */
    GLuint programPTC;         /* Position2, TexCoord2, Color4 */
    GLuint programPNTC;        /* ws_Vector3, Normal3, TexCoord2, Color4 */
    GLuint vertexBuffer;       /* Dynamic version buffer used by batches */
    ws_VertexPC *pPCVertices;     /* Used by dynamic rendering of ws_Vector2/ws_Color */
    ws_VertexPTC *pPTCVertices;   /* Used by dynamic rendering of ws_Vector2/ws_TexCoord/ws_Color */
    ws_VertexPNTC *pPNTCVertices; /* Used by dynamic rendering of ws_Vector2/ws_TexCoord/ws_Color */
    GLuint mapVB;              /* ws_Vector3art of the map */
    GLuint checkerTexture;     /* Test texture to replace non-existing or corrupted data */
    GLuint whiteTexture;       /* ... */
    ws_RenderTarget mainRT;       /* Main scree render ws_cam_target (Final image) */
    GLuint imguiFontTexture;
};

struct ws_Texture
{
    int w, h;
    GLuint tex;
};

struct ws_Font
{
    GLuint tex;
    float ws[256];
    float os[256];
};

extern _boolean sdl_keystates[NumCodes];
extern ws_Color ws_palette[];
extern ws_Color ws_dynamic_palette[];
extern int16_t ws_argc;
extern char **ws_argv;
extern int ws_screen_w;
extern int ws_screen_h;

// Camera
extern ws_Vector3 ws_cam_eye;
extern ws_Vector3 ws_cam_right;
extern ws_Vector3 ws_cam_front;
extern ws_Vector3 ws_cam_target;
extern bool ws_debug_view_enabled;

// Draw stuff
extern int ws_pc_count;
extern int ws_ptc_count;
extern int ws_pntc_count;
extern int ws_draw_mode;
extern float ws_ui_scale;
extern GLenum ws_draw_mode_prim;
extern GLuint ws_current_3d_texture;

// ws_Resources
extern ws_Resources ws_resources;
extern std::map<int16_t, ws_Texture> ws_ui_textures;
extern std::map<int16_t, GLuint> ws_screen_textures;
extern std::map<int, ws_Font> ws_font_textures;
extern std::map<int16_t, ws_Texture> ws_sprite_textures;
extern std::map<int16_t, ws_Texture> ws_wall_textures;

// Settings
extern bool ws_ao_enabled;
extern bool ws_texture_enabled;
extern bool ws_sprite_texture_enabled;
extern bool ws_sprite_enabled;
extern bool ws_wireframe_enabled;
extern float ws_ao_amount;
extern float ws_ao_size;

// Dos function emulations
int16_t inportb(int16_t addr);
void outportb(int16_t addr, char val);

Interrupt getvect(int16_t r_num);
void setvect(int16_t r_num, Interrupt interrupt);
void Mouse(int16_t x);

GLuint ws_create_texture(uint8_t *data, int w, int h);
ws_RenderTarget ws_create_rt(int w, int h);
void ws_resize_rt(ws_RenderTarget &rt, int w, int h);
GLuint ws_create_program(const GLchar *vs, const GLchar *ps, const std::vector<const char *> &attribs);
ws_Texture ws_load_ui_texture(int16_t chunknum);
void ws_preload_ui_textures();
ws_Texture ws_load_sprite_texture(int16_t shapenum);
ws_Texture ws_load_wall_texture(int wallpic);
ws_Font &ws_get_font(int id);

void ws_update_sdl();
void ws_play_sound(float* data, int len, float x, float y, bool _3d = false);

void ws_update_camera();
void ws_finish_draw_3d();
void ws_draw_wall(float x, float y, int dir, int texture, bool isDoor=false, int wallLeft = 0, int wallRight = 0);
void ws_draw_ceiling(int x, int y, int color, bool* neighbors);
void ws_draw_floor(int x, int y, int color, bool* neighbors);
void ws_draw_door_floor(int x, int y, int color, bool* neighbors, float percent);
void ws_draw_door_ceiling(int x, int y, int color, bool* neighbors, float percent);
void ws_draw_sprite(int x, int y, int texture);
void ws_flush();

void ws_draw_screen_from_raw(byte* data, int16_t chunk);
void ws_draw_pc(const ws_VertexPC *pVertices, int count, GLenum mode);
void ws_draw_ptc(const ws_VertexPTC *pVertices, int count, GLenum mode);
void ws_draw_pntc(const ws_VertexPNTC *pVertices, int count, GLenum mode);
void ws_draw_lines(const ws_VertexPC *pVertices, int count);
void ws_draw_points(const ws_VertexPC *pVertices, int count);
void ws_draw_quads(const ws_VertexPC *pVertices, int count);
void ws_prepare_for_pc(int prim);
void ws_prepare_for_ptc(int prim);
void ws_prepare_for_pntc(int prim, GLuint texture);
void ws_draw_lines(const ws_VertexPTC *pVertices, int count);
void ws_draw_points(const ws_VertexPTC *pVertices, int count);
void ws_draw_quads(const ws_VertexPTC *pVertices, int count);
int ws_draw_rect(ws_VertexPTC *pVertices, float x, float y, float w, float h, float u1, float v1, float u2, float v2, const ws_Color &color);
int ws_draw_rect(ws_VertexPC *pVertices, float x, float y, float w, float h, const ws_Color &color);
int ws_draw_line(ws_VertexPC *pVertices, const ws_Vector2 &from, const ws_Vector2 &to, const ws_Color &color);

#endif
