#include "ws.h"

void ws_draw_sprite(int x, int y, int texture)
{
    if (!ws_sprite_enabled) return;

    auto xi = x / 65536;
    auto yi = y / 65536;
    auto visible = ws_visible_tiles[xi][yi];

    ws_Texture pic;
    {
        auto it = ws_sprite_textures.find(texture);
        if (it == ws_sprite_textures.end())
            pic = ws_load_sprite_texture(texture);
        else
            pic = it->second;
    }

    auto xf = (float)x / 65536.0f;
    auto yf = 64.0f - (float)y / 65536.0f;
    ws_prepare_for_pntc(GL_QUADS, ws_sprite_texture_enabled ? pic.tex : ws_resources.whiteTexture);

    auto pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

    int clip[4] = { 0, 0, 64, 64 };

    // Sprite settings
    {
        auto it = ws_sprite_settings.find(texture);
        if (it != ws_sprite_settings.end())
        {
            if (it->second.emit_light)
            {
                auto light = it->second.light;

                // Make sure the radius of the light is in the current draw rect
                int radiusi = (int)light.radius + 1;
                if (xi + radiusi >= ws_culled_rect[0] &&
                    yi + radiusi >= ws_culled_rect[1] &&
                    xi - radiusi <= ws_culled_rect[2] &&
                    yi - radiusi <= ws_culled_rect[3])
                {
                    // Now lets check if any of the tile in the light rect is visible
                    int lightRect[4] = {
                        std::max(0, xi - radiusi),
                        std::max(0, yi - radiusi),
                        std::min(63, xi + radiusi),
                        std::min(63, yi + radiusi)
                    };

                    bool lightVisible = false;
                    for (int lx = lightRect[0]; lx <= lightRect[2]; ++lx)
                    {
                        for (int ly = lightRect[1]; ly <= lightRect[3]; ++ly)
                        {
                            if (ws_visible_tiles[lx][ly])
                            {
                                lightVisible = true;
                                break;
                            }
                        }
                        if (lightVisible) break;
                    }
                    if (lightVisible)
                    {
                        light.position = ws_Vector3(xf, yf, light.position.z) + ws_cam_right * light.position.x - ws_cam_front_flat * light.position.y;
                        ws_active_lights.push_back(light);
                    }
                }
            }
            memcpy(clip, it->second.clip, sizeof(clip));
        }
    }

    if (!visible) return;

    float clipf[4] = {
        (float)clip[0] / 64.0f,
        (float)clip[1] / 64.0f,
        (float)clip[2] / 64.0f,
        (float)clip[3] / 64.0f
    };

    xf -= ws_cam_right.x * 0.5f;
    yf -= ws_cam_right.y * 0.5f;

    pVertices[0].position.x = xf + ws_cam_right.x * clipf[0];
    pVertices[0].position.y = yf + ws_cam_right.y * clipf[0];
    pVertices[0].position.z = 1.0f - clipf[1];
    pVertices[0].normal = -ws_cam_front - ws_cam_right + ws_Vector3::UnitZ;
    pVertices[0].texCoord = { clipf[0], clipf[1] };
    pVertices[0].color = { 1, 1, 1, 1 };

    pVertices[1].position.x = xf + ws_cam_right.x * clipf[0];
    pVertices[1].position.y = yf + ws_cam_right.y * clipf[0];
    pVertices[1].position.z = 1.0f - clipf[3];
    pVertices[1].normal = -ws_cam_front - ws_cam_right - ws_Vector3::UnitZ * .1f;
    pVertices[1].texCoord = { clipf[0], clipf[3] };
    pVertices[1].color = { 1, 1, 1, 1 };

    pVertices[2].position.x = xf + ws_cam_right.x * clipf[2];
    pVertices[2].position.y = yf + ws_cam_right.y * clipf[2];
    pVertices[2].position.z = 1.0f - clipf[3];
    pVertices[2].normal = -ws_cam_front + ws_cam_right - ws_Vector3::UnitZ * .1f;
    pVertices[2].texCoord = { clipf[2], clipf[3] };
    pVertices[2].color = { 1, 1, 1, 1 };

    pVertices[3].position.x = xf + ws_cam_right.x * clipf[2];
    pVertices[3].position.y = yf + ws_cam_right.y * clipf[2];
    pVertices[3].position.z = 1.0f - clipf[1];
    pVertices[3].normal = -ws_cam_front + ws_cam_right + ws_Vector3::UnitZ;
    pVertices[3].texCoord = { clipf[2], clipf[1] };
    pVertices[3].color = { 1, 1, 1, 1 };

    ws_pntc_count += 4;
}
