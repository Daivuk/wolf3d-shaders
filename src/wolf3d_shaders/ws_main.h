// Wolf3D Shaders specific functions
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <GL/gl3w.h>
#include <SDL.h>
#include <cinttypes>

#include "ID_HEADS.H"

#include "ws_Matrix.h"

extern _boolean sdl_keystates[NumCodes];

GLuint ws_create_texture(uint8_t *data, int16_t w, int16_t h);
void ws_update_sdl();

// Dos function emulations
int16_t inportb(int16_t addr);
void outportb(int16_t addr, char val);

typedef int16_t(*Interrupt)(void);

Interrupt getvect(int16_t r_num);
void setvect(int16_t r_num, Interrupt interrupt);
void ws_draw_screen_from_raw(byte* data, int16_t chunk);
void ws_play_sound(float* data, int len, float x, float y, bool _3d = false);
void ws_draw_wall(float x, float y, int dir, int texture, bool isDoor=false);
void ws_finish_draw_3d();
void ws_update_camera();
void ws_draw_ceiling(int x, int y, int color, bool* neighbors);
void ws_draw_floor(int x, int y, int color, bool* neighbors);
void ws_draw_door_floor(int x, int y, int color, bool* neighbors, float percent);
void ws_draw_door_ceiling(int x, int y, int color, bool* neighbors, float percent);
void ws_draw_sprite(int x, int y, int texture);
void Mouse(int16_t x);

#endif
