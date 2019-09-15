#include "ws.h"

void ws_draw_sprite(int x, int y, int texture)
{
    if (!ws_sprite_enabled) return;

    auto xf = (float)x / 65536.0f;
    auto yf = 64.0f - (float)y / 65536.0f;

    auto it = ws_sprite_textures.find(texture);
    ws_Texture pic;
    if (it == ws_sprite_textures.end())
        pic = ws_load_sprite_texture(texture);
    else
        pic = it->second;

    ws_prepare_for_pntc(GL_QUADS, ws_sprite_texture_enabled ? pic.tex : ws_resources.whiteTexture);

    auto pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

    pVertices[0].position.x = xf - ws_cam_right.x * 0.5f;
    pVertices[0].position.y = yf - ws_cam_right.y * 0.5f;
    pVertices[0].position.z = 1.0f;
    pVertices[0].normal.x = -ws_cam_front.x;
    pVertices[0].normal.y = -ws_cam_front.y;
    pVertices[0].normal.z = -ws_cam_front.z;
    pVertices[0].texCoord = { 0, 0 };
    pVertices[0].color = { 1, 1, 1, 1 };

    pVertices[1].position.x = xf - ws_cam_right.x * 0.5f;
    pVertices[1].position.y = yf - ws_cam_right.y * 0.5f;
    pVertices[1].position.z = 0.0f;
    pVertices[1].normal.x = -ws_cam_front.x;
    pVertices[1].normal.y = -ws_cam_front.y;
    pVertices[1].normal.z = -ws_cam_front.z;
    pVertices[1].texCoord = { 0, 1 };
    pVertices[1].color = { 1, 1, 1, 1 };

    pVertices[2].position.x = xf + ws_cam_right.x * 0.5f;
    pVertices[2].position.y = yf + ws_cam_right.y * 0.5f;
    pVertices[2].position.z = 0.0f;
    pVertices[2].normal.x = -ws_cam_front.x;
    pVertices[2].normal.y = -ws_cam_front.y;
    pVertices[2].normal.z = -ws_cam_front.z;
    pVertices[2].texCoord = { 1, 1 };
    pVertices[2].color = { 1, 1, 1, 1 };

    pVertices[3].position.x = xf + ws_cam_right.x * 0.5f;
    pVertices[3].position.y = yf + ws_cam_right.y * 0.5f;
    pVertices[3].position.z = 1.0f;
    pVertices[3].normal.x = -ws_cam_front.x;
    pVertices[3].normal.y = -ws_cam_front.y;
    pVertices[3].normal.z = -ws_cam_front.z;
    pVertices[3].texCoord = { 1, 0 };
    pVertices[3].color = { 1, 1, 1, 1 };

    ws_pntc_count += 4;
}
