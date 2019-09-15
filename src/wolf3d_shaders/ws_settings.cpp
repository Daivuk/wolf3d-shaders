#include "ws.h"

bool ws_ao_enabled = true;
bool ws_texture_enabled = true;
bool ws_sprite_texture_enabled = true;
bool ws_sprite_enabled = true;
bool ws_wireframe_enabled = false;
bool ws_deferred_enabled = true;
float ws_ao_amount = 0.65f;
float ws_ao_size = 0.25f;
ws_Color ws_ambient_color = { .1f, .1f, .1f, 1.0f };
ws_Color ws_player_light_color = { 1.0f, 1.0f, 1.0f, 1.0f };
float ws_player_light_radius = 5.0f;
float ws_player_light_intensity = 1.3f;
