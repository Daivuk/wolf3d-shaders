#include "ws.h"

void ws_draw_wall(float x, float y, int dir, int wallpic, bool isDoor, int wallLeft, int wallRight)
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

    ws_Color col = { 1, 1, 1, 1 };
    ws_Color aoCol = { ws_ao_amount, ws_ao_amount, ws_ao_amount, 1 };
    auto midP = (1.0f + ws_ao_amount) * 0.5f;
    ws_Color aoMidCol = { midP, midP, midP, 1 };

    auto leftAOSize = wallLeft == 0 ? 0.0f : ws_ao_size;
    auto rightAOSize = wallRight == 0 ? 1.0f : 1.0f - ws_ao_size;
    float uL = uvs.x + (uvs.y - uvs.x) * leftAOSize;
    float uR = uvs.x + (uvs.y - uvs.x) * rightAOSize;

    if (ws_ao_enabled)
    {
        if (wallLeft)
        {
            auto colMid = wallLeft == 1 ? aoCol : col;
            auto colEdg = wallLeft == -1 ? aoMidCol : aoCol;

            // top
            pVertices[0].position.x = x + ofs.x * leftAOSize;
            pVertices[0].position.y = y + ofs.y * leftAOSize;
            pVertices[0].position.z = 1.0f;
            pVertices[0].normal.x = n.x;
            pVertices[0].normal.y = n.y;
            pVertices[0].normal.z = 0.0f;
            pVertices[0].texCoord = { uL, 0 };
            pVertices[0].color = aoCol;

            pVertices[1].position.x = x;
            pVertices[1].position.y = y;
            pVertices[1].position.z = 1.0f;
            pVertices[1].normal.x = n.x;
            pVertices[1].normal.y = n.y;
            pVertices[1].normal.z = 0.0f;
            pVertices[1].texCoord = { uvs.x, 0 };
            pVertices[1].color = colEdg;

            pVertices[2].position.x = x;
            pVertices[2].position.y = y;
            pVertices[2].position.z = 1.0f - ws_ao_size;
            pVertices[2].normal.x = n.x;
            pVertices[2].normal.y = n.y;
            pVertices[2].normal.z = 0.0f;
            pVertices[2].texCoord = { uvs.x, ws_ao_size };
            pVertices[2].color = colMid;

            pVertices[3].position.x = x + ofs.x * leftAOSize;
            pVertices[3].position.y = y + ofs.y * leftAOSize;
            pVertices[3].position.z = 1.0f - ws_ao_size;
            pVertices[3].normal.x = n.x;
            pVertices[3].normal.y = n.y;
            pVertices[3].normal.z = 0.0f;
            pVertices[3].texCoord = { uL, ws_ao_size };
            pVertices[3].color = col;

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
            pVertices[0].color = colMid;

            pVertices[1].position.x = x;
            pVertices[1].position.y = y;
            pVertices[1].position.z = ws_ao_size;
            pVertices[1].normal.x = n.x;
            pVertices[1].normal.y = n.y;
            pVertices[1].normal.z = 0.0f;
            pVertices[1].texCoord = { uvs.x, 1.0f - ws_ao_size };
            pVertices[1].color = colMid;

            pVertices[2].position.x = x + ofs.x * leftAOSize;
            pVertices[2].position.y = y + ofs.y * leftAOSize;
            pVertices[2].position.z = ws_ao_size;
            pVertices[2].normal.x = n.x;
            pVertices[2].normal.y = n.y;
            pVertices[2].normal.z = 0.0f;
            pVertices[2].texCoord = { uL, 1.0f - ws_ao_size };
            pVertices[2].color = col;

            pVertices[3].position.x = x + ofs.x * leftAOSize;
            pVertices[3].position.y = y + ofs.y * leftAOSize;
            pVertices[3].position.z = 1.0f - ws_ao_size;
            pVertices[3].normal.x = n.x;
            pVertices[3].normal.y = n.y;
            pVertices[3].normal.z = 0.0f;
            pVertices[3].texCoord = { uL, ws_ao_size };
            pVertices[3].color = col;

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
            pVertices[0].color = colMid;

            pVertices[1].position.x = x;
            pVertices[1].position.y = y;
            pVertices[1].position.z = 0.0f;
            pVertices[1].normal.x = n.x;
            pVertices[1].normal.y = n.y;
            pVertices[1].normal.z = 0.0f;
            pVertices[1].texCoord = { uvs.x, 1 };
            pVertices[1].color = colEdg;

            pVertices[2].position.x = x + ofs.x * leftAOSize;
            pVertices[2].position.y = y + ofs.y * leftAOSize;
            pVertices[2].position.z = 0.0f;
            pVertices[2].normal.x = n.x;
            pVertices[2].normal.y = n.y;
            pVertices[2].normal.z = 0.0f;
            pVertices[2].texCoord = { uL, 1 };
            pVertices[2].color = aoCol;

            pVertices[3].position.x = x + ofs.x * leftAOSize;
            pVertices[3].position.y = y + ofs.y * leftAOSize;
            pVertices[3].position.z = ws_ao_size;
            pVertices[3].normal.x = n.x;
            pVertices[3].normal.y = n.y;
            pVertices[3].normal.z = 0.0f;
            pVertices[3].texCoord = { uL, 1.0f - ws_ao_size };
            pVertices[3].color = col;

            ws_pntc_count += 4;
            pVertices = ws_resources.pPNTCVertices + ws_pntc_count;
        }

        // top
        pVertices[0].position.x = x + ofs.x * leftAOSize;
        pVertices[0].position.y = y + ofs.y * leftAOSize;
        pVertices[0].position.z = 1.0f;
        pVertices[0].normal.x = n.x;
        pVertices[0].normal.y = n.y;
        pVertices[0].normal.z = 0.0f;
        pVertices[0].texCoord = { uL, 0 };
        pVertices[0].color = aoCol;

        pVertices[1].position.x = x + ofs.x * leftAOSize;
        pVertices[1].position.y = y + ofs.y * leftAOSize;
        pVertices[1].position.z = 1.0f - ws_ao_size;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uL, ws_ao_size };
        pVertices[1].color = col;

        pVertices[2].position.x = x + ofs.x * rightAOSize;
        pVertices[2].position.y = y + ofs.y * rightAOSize;
        pVertices[2].position.z = 1.0f - ws_ao_size;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uR, ws_ao_size };
        pVertices[2].color = col;

        pVertices[3].position.x = x + ofs.x * rightAOSize;
        pVertices[3].position.y = y + ofs.y * rightAOSize;
        pVertices[3].position.z = 1.0f;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uR, 0 };
        pVertices[3].color = aoCol;

        ws_pntc_count += 4;
        pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

        // middle
        pVertices[0].position.x = x + ofs.x * leftAOSize;
        pVertices[0].position.y = y + ofs.y * leftAOSize;
        pVertices[0].position.z = 1.0f - ws_ao_size;
        pVertices[0].normal.x = n.x;
        pVertices[0].normal.y = n.y;
        pVertices[0].normal.z = 0.0f;
        pVertices[0].texCoord = { uL, ws_ao_size };
        pVertices[0].color = col;

        pVertices[1].position.x = x + ofs.x * leftAOSize;
        pVertices[1].position.y = y + ofs.y * leftAOSize;
        pVertices[1].position.z = ws_ao_size;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uL, 1.0f - ws_ao_size };
        pVertices[1].color = col;

        pVertices[2].position.x = x + ofs.x * rightAOSize;
        pVertices[2].position.y = y + ofs.y * rightAOSize;
        pVertices[2].position.z = ws_ao_size;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uR, 1.0f - ws_ao_size };
        pVertices[2].color = col;

        pVertices[3].position.x = x + ofs.x * rightAOSize;
        pVertices[3].position.y = y + ofs.y * rightAOSize;
        pVertices[3].position.z = 1.0f - ws_ao_size;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uR, ws_ao_size };
        pVertices[3].color = col;

        ws_pntc_count += 4;
        pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

        // bottom
        pVertices[0].position.x = x + ofs.x * leftAOSize;
        pVertices[0].position.y = y + ofs.y * leftAOSize;
        pVertices[0].position.z = ws_ao_size;
        pVertices[0].normal.x = n.x;
        pVertices[0].normal.y = n.y;
        pVertices[0].normal.z = 0.0f;
        pVertices[0].texCoord = { uL, 1.0f - ws_ao_size };
        pVertices[0].color = col;

        pVertices[1].position.x = x + ofs.x * leftAOSize;
        pVertices[1].position.y = y + ofs.y * leftAOSize;
        pVertices[1].position.z = 0.0f;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uL, 1 };
        pVertices[1].color = aoCol;

        pVertices[2].position.x = x + ofs.x * rightAOSize;
        pVertices[2].position.y = y + ofs.y * rightAOSize;
        pVertices[2].position.z = 0.0f;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uR, 1 };
        pVertices[2].color = aoCol;

        pVertices[3].position.x = x + ofs.x * rightAOSize;
        pVertices[3].position.y = y + ofs.y * rightAOSize;
        pVertices[3].position.z = ws_ao_size;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uR, 1.0f - ws_ao_size };
        pVertices[3].color = col;

        ws_pntc_count += 4;
        pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

        if (wallRight != 0)
        {
            auto colMid = wallRight == 1 ? aoCol : col;
            auto colEdg = wallRight == -1 ? aoMidCol : aoCol;

            // top
            pVertices[0].position.x = x + ofs.x * rightAOSize;
            pVertices[0].position.y = y + ofs.y * rightAOSize;
            pVertices[0].position.z = 1.0f;
            pVertices[0].normal.x = n.x;
            pVertices[0].normal.y = n.y;
            pVertices[0].normal.z = 0.0f;
            pVertices[0].texCoord = { uR, 0 };
            pVertices[0].color = aoCol;

            pVertices[1].position.x = x + ofs.x * rightAOSize;
            pVertices[1].position.y = y + ofs.y * rightAOSize;
            pVertices[1].position.z = 1.0f - ws_ao_size;
            pVertices[1].normal.x = n.x;
            pVertices[1].normal.y = n.y;
            pVertices[1].normal.z = 0.0f;
            pVertices[1].texCoord = { uR, ws_ao_size };
            pVertices[1].color = col;

            pVertices[2].position.x = x + ofs.x;
            pVertices[2].position.y = y + ofs.y;
            pVertices[2].position.z = 1.0f - ws_ao_size;
            pVertices[2].normal.x = n.x;
            pVertices[2].normal.y = n.y;
            pVertices[2].normal.z = 0.0f;
            pVertices[2].texCoord = { uvs.y, ws_ao_size };
            pVertices[2].color = colMid;

            pVertices[3].position.x = x + ofs.x;
            pVertices[3].position.y = y + ofs.y;
            pVertices[3].position.z = 1.0f;
            pVertices[3].normal.x = n.x;
            pVertices[3].normal.y = n.y;
            pVertices[3].normal.z = 0.0f;
            pVertices[3].texCoord = { uvs.y, 0 };
            pVertices[3].color = colEdg;

            ws_pntc_count += 4;
            pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

            // middle
            pVertices[0].position.x = x + ofs.x * rightAOSize;
            pVertices[0].position.y = y + ofs.y * rightAOSize;
            pVertices[0].position.z = 1.0f - ws_ao_size;
            pVertices[0].normal.x = n.x;
            pVertices[0].normal.y = n.y;
            pVertices[0].normal.z = 0.0f;
            pVertices[0].texCoord = { uR, ws_ao_size };
            pVertices[0].color = col;

            pVertices[1].position.x = x + ofs.x * rightAOSize;
            pVertices[1].position.y = y + ofs.y * rightAOSize;
            pVertices[1].position.z = ws_ao_size;
            pVertices[1].normal.x = n.x;
            pVertices[1].normal.y = n.y;
            pVertices[1].normal.z = 0.0f;
            pVertices[1].texCoord = { uR, 1.0f - ws_ao_size };
            pVertices[1].color = col;

            pVertices[2].position.x = x + ofs.x;
            pVertices[2].position.y = y + ofs.y;
            pVertices[2].position.z = ws_ao_size;
            pVertices[2].normal.x = n.x;
            pVertices[2].normal.y = n.y;
            pVertices[2].normal.z = 0.0f;
            pVertices[2].texCoord = { uvs.y, 1.0f - ws_ao_size };
            pVertices[2].color = colMid;

            pVertices[3].position.x = x + ofs.x;
            pVertices[3].position.y = y + ofs.y;
            pVertices[3].position.z = 1.0f - ws_ao_size;
            pVertices[3].normal.x = n.x;
            pVertices[3].normal.y = n.y;
            pVertices[3].normal.z = 0.0f;
            pVertices[3].texCoord = { uvs.y, ws_ao_size };
            pVertices[3].color = colMid;

            ws_pntc_count += 4;
            pVertices = ws_resources.pPNTCVertices + ws_pntc_count;

            // bottom
            pVertices[0].position.x = x + ofs.x;
            pVertices[0].position.y = y + ofs.y;
            pVertices[0].position.z = ws_ao_size;
            pVertices[0].normal.x = n.x;
            pVertices[0].normal.y = n.y;
            pVertices[0].normal.z = 0.0f;
            pVertices[0].texCoord = { uvs.y, 1.0f - ws_ao_size };
            pVertices[0].color = colMid;

            pVertices[1].position.x = x + ofs.x * rightAOSize;
            pVertices[1].position.y = y + ofs.y * rightAOSize;
            pVertices[1].position.z = ws_ao_size;
            pVertices[1].normal.x = n.x;
            pVertices[1].normal.y = n.y;
            pVertices[1].normal.z = 0.0f;
            pVertices[1].texCoord = { uR, 1.0f - ws_ao_size };
            pVertices[1].color = col;

            pVertices[2].position.x = x + ofs.x * rightAOSize;
            pVertices[2].position.y = y + ofs.y * rightAOSize;
            pVertices[2].position.z = 0.0f;
            pVertices[2].normal.x = n.x;
            pVertices[2].normal.y = n.y;
            pVertices[2].normal.z = 0.0f;
            pVertices[2].texCoord = { uR, 1 };
            pVertices[2].color = aoCol;

            pVertices[3].position.x = x + ofs.x;
            pVertices[3].position.y = y + ofs.y;
            pVertices[3].position.z = 0.0f;
            pVertices[3].normal.x = n.x;
            pVertices[3].normal.y = n.y;
            pVertices[3].normal.z = 0.0f;
            pVertices[3].texCoord = { uvs.y, 1 };
            pVertices[3].color = colEdg;

            ws_pntc_count += 4;
        }
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
        pVertices[0].color = col;

        pVertices[1].position.x = x;
        pVertices[1].position.y = y;
        pVertices[1].position.z = 0.0f;
        pVertices[1].normal.x = n.x;
        pVertices[1].normal.y = n.y;
        pVertices[1].normal.z = 0.0f;
        pVertices[1].texCoord = { uvs.x, 1 };
        pVertices[1].color = col;

        pVertices[2].position.x = x + ofs.x;
        pVertices[2].position.y = y + ofs.y;
        pVertices[2].position.z = 0.0f;
        pVertices[2].normal.x = n.x;
        pVertices[2].normal.y = n.y;
        pVertices[2].normal.z = 0.0f;
        pVertices[2].texCoord = { uvs.y, 1 };
        pVertices[2].color = col;

        pVertices[3].position.x = x + ofs.x;
        pVertices[3].position.y = y + ofs.y;
        pVertices[3].position.z = 1.0f;
        pVertices[3].normal.x = n.x;
        pVertices[3].normal.y = n.y;
        pVertices[3].normal.z = 0.0f;
        pVertices[3].texCoord = { uvs.y, 0 };
        pVertices[3].color = col;

        ws_pntc_count += 4;
    }
}
