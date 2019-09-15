#include "ws.h"

void ws_draw_wall(float x, float y, int dir, int wallpic, bool isDoor)
{
    GLuint tex = ws_resources.checkerTexture;
    if (wallpic != -1)
    {
        auto it = ws_wall_textures.find(wallpic);
        ws_Texture pic;
        if (it == ws_wall_textures.end())
            pic = ws_load_wall_texture(wallpic);
        else
            pic = it->second;
        tex = pic.tex;
    }
    if (!ws_texture_enabled) tex = ws_resources.whiteTexture;

    y = 64.0f - y;

    ws_prepare_for_pntc(GL_QUADS, tex);
    const ws_Vector2 DIROFS[4] = {
        {-1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, -1.0f} };
    const ws_Vector2 DIRNS[4] = {
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, -1.0f},
        {-1.0f, 0.0f} };
    const ws_Vector2 DIRUVS[4] = {
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 1.0f} };

    auto &ofs = DIROFS[dir];
    auto &n = DIRNS[dir];
    auto &uvs = DIRUVS[isDoor ? dir : 2];
    auto pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

    if (ws_ao_enabled)
    {
        // top
        pVertices[0].position.x = x;
        pVertices[0].position.y = y;
        pVertices[0].position.z = 1.0f;
        pVertices[0].normal.x = n.x;
        pVertices[0].normal.y = n.y;
        pVertices[0].normal.z = 0.0f;
        pVertices[0].texCoord = { uvs.x, 0 };
        pVertices[0].color = { ws_ao_amount, ws_ao_amount, ws_ao_amount, 1 };

        pVertices[1].position.x = x;
        pVertices[1].position.y = y;
        pVertices[1].position.z = 1.0f - ws_ao_size;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uvs.x, ws_ao_size };
        pVertices[1].color = { 1, 1, 1, 1 };

        pVertices[2].position.x = x + ofs.x;
        pVertices[2].position.y = y + ofs.y;
        pVertices[2].position.z = 1.0f - ws_ao_size;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uvs.y, ws_ao_size };
        pVertices[2].color = { 1, 1, 1, 1 };

        pVertices[3].position.x = x + ofs.x;
        pVertices[3].position.y = y + ofs.y;
        pVertices[3].position.z = 1.0f;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uvs.y, 0 };
        pVertices[3].color = { ws_ao_amount, ws_ao_amount, ws_ao_amount, 1 };

        ws_pntc_count += 4;
        pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

        // middle
        pVertices[0].position.x = x;
        pVertices[0].position.y = y;
        pVertices[0].position.z = 1.0f - ws_ao_size;
        pVertices[0].normal.x = n.x;
        pVertices[0].normal.y = n.y;
        pVertices[0].normal.z = 0.0f;
        pVertices[0].texCoord = { uvs.x, ws_ao_size };
        pVertices[0].color = { 1, 1, 1, 1 };

        pVertices[1].position.x = x;
        pVertices[1].position.y = y;
        pVertices[1].position.z = ws_ao_size;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uvs.x, 1.0f - ws_ao_size };
        pVertices[1].color = { 1, 1, 1, 1 };

        pVertices[2].position.x = x + ofs.x;
        pVertices[2].position.y = y + ofs.y;
        pVertices[2].position.z = ws_ao_size;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uvs.y, 1.0f - ws_ao_size };
        pVertices[2].color = { 1, 1, 1, 1 };

        pVertices[3].position.x = x + ofs.x;
        pVertices[3].position.y = y + ofs.y;
        pVertices[3].position.z = 1.0f - ws_ao_size;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uvs.y, ws_ao_size };
        pVertices[3].color = { 1, 1, 1, 1 };

        ws_pntc_count += 4;
        pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

        // bottom
        pVertices[0].position.x = x;
        pVertices[0].position.y = y;
        pVertices[0].position.z = ws_ao_size;
        pVertices[0].normal.x = n.x;
        pVertices[0].normal.y = n.y;
        pVertices[0].normal.z = 0.0f;
        pVertices[0].texCoord = { uvs.x, 1.0f - ws_ao_size };
        pVertices[0].color = { 1, 1, 1, 1 };

        pVertices[1].position.x = x;
        pVertices[1].position.y = y;
        pVertices[1].position.z = 0.0f;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uvs.x, 1 };
        pVertices[1].color = { ws_ao_amount, ws_ao_amount, ws_ao_amount, 1 };

        pVertices[2].position.x = x + ofs.x;
        pVertices[2].position.y = y + ofs.y;
        pVertices[2].position.z = 0.0f;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uvs.y, 1 };
        pVertices[2].color = { ws_ao_amount, ws_ao_amount, ws_ao_amount, 1 };

        pVertices[3].position.x = x + ofs.x;
        pVertices[3].position.y = y + ofs.y;
        pVertices[3].position.z = ws_ao_size;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uvs.y, 1.0f - ws_ao_size };
        pVertices[3].color = { 1, 1, 1, 1 };

        ws_pntc_count += 4;
    }
    else
    {
        pVertices[0].position.x = x;
        pVertices[0].position.y = y;
        pVertices[0].position.z = 1.0f;
        pVertices[0].normal.x = n.x;
        pVertices[0].normal.y = n.y;
        pVertices[0].normal.z = 0.0f;
        pVertices[0].texCoord = { uvs.x, 0 };
        pVertices[0].color = { 1, 1, 1, 1 };

        pVertices[1].position.x = x;
        pVertices[1].position.y = y;
        pVertices[1].position.z = 0.0f;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uvs.x, 1 };
        pVertices[1].color = { 1, 1, 1, 1 };

        pVertices[2].position.x = x + ofs.x;
        pVertices[2].position.y = y + ofs.y;
        pVertices[2].position.z = 0.0f;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uvs.y, 1 };
        pVertices[2].color = { 1, 1, 1, 1 };

        pVertices[3].position.x = x + ofs.x;
        pVertices[3].position.y = y + ofs.y;
        pVertices[3].position.z = 1.0f;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uvs.y, 0 };
        pVertices[3].color = { 1, 1, 1, 1 };

        ws_pntc_count += 4;
    }
}
