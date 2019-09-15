#include "ws.h"

void ws_draw_ceiling(int x, int y, int color, bool* neighbors)
{
    auto col = ws_dynamic_palette[color];
    if (!ws_texture_enabled) col = { 1, 1, 1, 1 };

    ws_prepare_for_pntc(GL_QUADS, ws_resources.whiteTexture);

    auto xf = (float)x;
    auto yf = 63.0f - (float)y;

    auto pVertices = ws_resources.pPNTCVertices + ws_pntc_count;
    if (!ws_ao_enabled || !neighbors || (!neighbors[0] && !neighbors[1] && !neighbors[2] && !neighbors[3] && !neighbors[4] && !neighbors[5] && !neighbors[6] && !neighbors[7]) || !ws_ao_size)
    {
        pVertices[0].position.x = xf;
        pVertices[0].position.y = yf;
        pVertices[0].position.z = 1.0f;
        pVertices[0].normal.x = 0;
        pVertices[0].normal.y = 0;
        pVertices[0].normal.z = -1;
        pVertices[0].texCoord = { 0, 0 };
        pVertices[0].color = col;

        pVertices[1].position.x = xf;
        pVertices[1].position.y = yf + 1.0f;
        pVertices[1].position.z = 1.0f;
        pVertices[1].normal.x = 0;
        pVertices[1].normal.y = 0;
        pVertices[1].normal.z = -1;
        pVertices[1].texCoord = { 0, 1 };
        pVertices[1].color = col;

        pVertices[2].position.x = xf + 1.0f;
        pVertices[2].position.y = yf + 1.0f;
        pVertices[2].position.z = 1.0f;
        pVertices[2].normal.x = 0;
        pVertices[2].normal.y = 0;
        pVertices[2].normal.z = -1;
        pVertices[2].texCoord = { 1, 1 };
        pVertices[2].color = col;

        pVertices[3].position.x = xf + 1.0f;
        pVertices[3].position.y = yf;
        pVertices[3].position.z = 1.0f;
        pVertices[3].normal.x = 0;
        pVertices[3].normal.y = 0;
        pVertices[3].normal.z = -1;
        pVertices[3].texCoord = { 1, 0 };
        pVertices[3].color = col;

        ws_pntc_count += 4;
    }
    else
    {
        ws_Color aoCol = { col.r * ws_ao_amount, col.g * ws_ao_amount, col.b * ws_ao_amount, 1 };
        auto midP = (1.0f + ws_ao_amount) * 0.5f;
        ws_Color aoMidCol = { col.r * midP, col.g * midP, col.b * midP, 1 };
        ws_Color nCol[] = {
            neighbors[0] ? aoCol : col,
            neighbors[1] ? ((neighbors[0] || neighbors[2]) ? aoCol : aoMidCol) : ((neighbors[0] || neighbors[2]) ? aoMidCol : col),
            neighbors[2] ? aoCol : col,
            neighbors[3] ? ((neighbors[2] || neighbors[4]) ? aoCol : aoMidCol) : ((neighbors[2] || neighbors[4]) ? aoMidCol : col),
            neighbors[4] ? aoCol : col,
            neighbors[5] ? ((neighbors[4] || neighbors[6]) ? aoCol : aoMidCol) : ((neighbors[4] || neighbors[6]) ? aoMidCol : col),
            neighbors[6] ? aoCol : col,
            neighbors[7] ? ((neighbors[6] || neighbors[0]) ? aoCol : aoMidCol) : ((neighbors[6] || neighbors[0]) ? aoMidCol : col),
        };
        auto invSize = 1.0f - ws_ao_size;
        for (int i = 0; i < 4 * 9; ++i)
        {
            pVertices[i].normal = { 0, 0, -1 };
            pVertices[i].texCoord = { 0, 0 }; // no textures on ceiling
        }

        pVertices[0].position = { xf + ws_ao_size, yf, 1.0f };
        pVertices[1].position = { xf, yf, 1.0f };
        pVertices[2].position = { xf, yf + ws_ao_size, 1.0f };
        pVertices[3].position = { xf + ws_ao_size, yf + ws_ao_size, 1.0f };
        pVertices[0].color = nCol[0];
        pVertices[1].color = nCol[7];
        pVertices[2].color = nCol[6];
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + ws_ao_size, yf, 1.0f };
        pVertices[1].position = { xf + ws_ao_size, yf + ws_ao_size, 1.0f };
        pVertices[2].position = { xf + invSize, yf + ws_ao_size, 1.0f };
        pVertices[3].position = { xf + invSize, yf, 1.0f };
        pVertices[0].color = nCol[0];
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = nCol[0];

        pVertices += 4;
        pVertices[0].position = { xf + invSize, yf, 1.0f };
        pVertices[1].position = { xf + invSize, yf + ws_ao_size, 1.0f };
        pVertices[2].position = { xf + 1, yf + ws_ao_size, 1.0f };
        pVertices[3].position = { xf + 1, yf, 1.0f };
        pVertices[0].color = nCol[0];
        pVertices[1].color = col;
        pVertices[2].color = nCol[2];
        pVertices[3].color = nCol[1];

        pVertices += 4;
        pVertices[0].position = { xf, yf + ws_ao_size, 1.0f };
        pVertices[1].position = { xf, yf + invSize, 1.0f };
        pVertices[2].position = { xf + ws_ao_size, yf + invSize, 1.0f };
        pVertices[3].position = { xf + ws_ao_size, yf + ws_ao_size, 1.0f };
        pVertices[0].color = nCol[6];
        pVertices[1].color = nCol[6];
        pVertices[2].color = col;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + ws_ao_size, yf + ws_ao_size, 1.0f };
        pVertices[1].position = { xf + ws_ao_size, yf + invSize, 1.0f };
        pVertices[2].position = { xf + invSize, yf + invSize, 1.0f };
        pVertices[3].position = { xf + invSize, yf + ws_ao_size, 1.0f };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + invSize, yf + ws_ao_size, 1.0f };
        pVertices[1].position = { xf + invSize, yf + invSize, 1.0f };
        pVertices[2].position = { xf + 1, yf + invSize, 1.0f };
        pVertices[3].position = { xf + 1, yf + ws_ao_size, 1.0f };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = nCol[2];
        pVertices[3].color = nCol[2];

        pVertices += 4;
        pVertices[0].position = { xf, yf + invSize, 1.0f };
        pVertices[1].position = { xf, yf + 1, 1.0f };
        pVertices[2].position = { xf + ws_ao_size, yf + 1, 1.0f };
        pVertices[3].position = { xf + ws_ao_size, yf + invSize, 1.0f };
        pVertices[0].color = nCol[6];
        pVertices[1].color = nCol[5];
        pVertices[2].color = nCol[4];
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + ws_ao_size, yf + invSize, 1.0f };
        pVertices[1].position = { xf + ws_ao_size, yf + 1, 1.0f };
        pVertices[2].position = { xf + invSize, yf + 1, 1.0f };
        pVertices[3].position = { xf + invSize, yf + invSize, 1.0f };
        pVertices[0].color = col;
        pVertices[1].color = nCol[4];
        pVertices[2].color = nCol[4];
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + 1, yf + invSize, 1.0f };
        pVertices[1].position = { xf + invSize, yf + invSize, 1.0f };
        pVertices[2].position = { xf + invSize, yf + 1, 1.0f };
        pVertices[3].position = { xf + 1, yf + 1, 1.0f };
        pVertices[0].color = nCol[2];
        pVertices[1].color = col;
        pVertices[2].color = nCol[4];
        pVertices[3].color = nCol[3];

        ws_pntc_count += 4 * 9;
    }
}

void ws_draw_floor(int x, int y, int color, bool* neighbors)
{
    auto col = ws_dynamic_palette[color];
    if (!ws_texture_enabled) col = { 1, 1, 1, 1 };

    ws_prepare_for_pntc(GL_QUADS, ws_resources.whiteTexture);

    auto xf = (float)x;
    auto yf = 63.0f - (float)y;

    auto pVertices = ws_resources.pPNTCVertices + ws_pntc_count;
    if (!ws_ao_enabled || !neighbors || (!neighbors[0] && !neighbors[1] && !neighbors[2] && !neighbors[3] && !neighbors[4] && !neighbors[5] && !neighbors[6] && !neighbors[7]) || !ws_ao_size)
    {
        pVertices[0].position.x = xf;
        pVertices[0].position.y = yf + 1.0f;
        pVertices[0].position.z = 0.0f;
        pVertices[0].normal.x = 0;
        pVertices[0].normal.y = 0;
        pVertices[0].normal.z = 1;
        pVertices[0].texCoord = { 0, 0 };
        pVertices[0].color = col;

        pVertices[1].position.x = xf;
        pVertices[1].position.y = yf;
        pVertices[1].position.z = 0.0f;
        pVertices[1].normal.x = 0;
        pVertices[1].normal.y = 0;
        pVertices[1].normal.z = 1;
        pVertices[1].texCoord = { 0, 1 };
        pVertices[1].color = col;

        pVertices[2].position.x = xf + 1.0f;
        pVertices[2].position.y = yf;
        pVertices[2].position.z = 0.0f;
        pVertices[2].normal.x = 0;
        pVertices[2].normal.y = 0;
        pVertices[2].normal.z = 1;
        pVertices[2].texCoord = { 1, 1 };
        pVertices[2].color = col;

        pVertices[3].position.x = xf + 1.0f;
        pVertices[3].position.y = yf + 1.0f;
        pVertices[3].position.z = 0.0f;
        pVertices[3].normal.x = 0;
        pVertices[3].normal.y = 0;
        pVertices[3].normal.z = 1;
        pVertices[3].texCoord = { 1, 0 };
        pVertices[3].color = col;

        ws_pntc_count += 4;
    }
    else
    {
        ws_Color aoCol = { col.r * ws_ao_amount, col.g * ws_ao_amount, col.b * ws_ao_amount, 1 };
        auto midP = (1.0f + ws_ao_amount) * 0.5f;
        ws_Color aoMidCol = { col.r * midP, col.g * midP, col.b * midP, 1 };
        ws_Color nCol[] = {
            neighbors[0] ? aoCol : col,
            neighbors[1] ? ((neighbors[0] || neighbors[2]) ? aoCol : aoMidCol) : ((neighbors[0] || neighbors[2]) ? aoMidCol : col),
            neighbors[2] ? aoCol : col,
            neighbors[3] ? ((neighbors[2] || neighbors[4]) ? aoCol : aoMidCol) : ((neighbors[2] || neighbors[4]) ? aoMidCol : col),
            neighbors[4] ? aoCol : col,
            neighbors[5] ? ((neighbors[4] || neighbors[6]) ? aoCol : aoMidCol) : ((neighbors[4] || neighbors[6]) ? aoMidCol : col),
            neighbors[6] ? aoCol : col,
            neighbors[7] ? ((neighbors[6] || neighbors[0]) ? aoCol : aoMidCol) : ((neighbors[6] || neighbors[0]) ? aoMidCol : col),
        };
        auto invSize = 1.0f - ws_ao_size;
        for (int i = 0; i < 4 * 9; ++i)
        {
            pVertices[i].normal = { 0, 0, 1 };
            pVertices[i].texCoord = { 0, 0 }; // no textures on floors
        }

        pVertices[0].position = { xf + ws_ao_size, yf, 0 };
        pVertices[1].position = { xf + ws_ao_size, yf + ws_ao_size, 0 };
        pVertices[2].position = { xf, yf + ws_ao_size, 0 };
        pVertices[3].position = { xf, yf, 0 };
        pVertices[0].color = nCol[0];
        pVertices[1].color = col;
        pVertices[2].color = nCol[6];
        pVertices[3].color = nCol[7];

        pVertices += 4;
        pVertices[0].position = { xf + ws_ao_size, yf + ws_ao_size, 0 };
        pVertices[1].position = { xf + ws_ao_size, yf, 0 };
        pVertices[2].position = { xf + invSize, yf, 0 };
        pVertices[3].position = { xf + invSize, yf + ws_ao_size, 0 };
        pVertices[0].color = col;
        pVertices[1].color = nCol[0];
        pVertices[2].color = nCol[0];
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + invSize, yf, 0 };
        pVertices[1].position = { xf + 1, yf, 0 };
        pVertices[2].position = { xf + 1, yf + ws_ao_size, 0 };
        pVertices[3].position = { xf + invSize, yf + ws_ao_size, 0 };
        pVertices[0].color = nCol[0];
        pVertices[1].color = nCol[1];
        pVertices[2].color = nCol[2];
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf, yf + invSize, 0 };
        pVertices[1].position = { xf, yf + ws_ao_size, 0 };
        pVertices[2].position = { xf + ws_ao_size, yf + ws_ao_size, 0 };
        pVertices[3].position = { xf + ws_ao_size, yf + invSize, 0 };
        pVertices[0].color = nCol[6];
        pVertices[1].color = nCol[6];
        pVertices[2].color = col;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + ws_ao_size, yf + invSize, 0 };
        pVertices[1].position = { xf + ws_ao_size, yf + ws_ao_size, 0 };
        pVertices[2].position = { xf + invSize, yf + ws_ao_size, 0 };
        pVertices[3].position = { xf + invSize, yf + invSize, 0 };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + invSize, yf + invSize, 0 };
        pVertices[1].position = { xf + invSize, yf + ws_ao_size, 0 };
        pVertices[2].position = { xf + 1, yf + ws_ao_size, 0 };
        pVertices[3].position = { xf + 1, yf + invSize, 0 };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = nCol[2];
        pVertices[3].color = nCol[2];

        pVertices += 4;
        pVertices[0].position = { xf, yf + invSize, 0 };
        pVertices[1].position = { xf + ws_ao_size, yf + invSize, 0 };
        pVertices[2].position = { xf + ws_ao_size, yf + 1, 0 };
        pVertices[3].position = { xf, yf + 1, 0 };
        pVertices[0].color = nCol[6];
        pVertices[1].color = col;
        pVertices[2].color = nCol[4];
        pVertices[3].color = nCol[5];

        pVertices += 4;
        pVertices[0].position = { xf + ws_ao_size, yf + 1, 0 };
        pVertices[1].position = { xf + ws_ao_size, yf + invSize, 0 };
        pVertices[2].position = { xf + invSize, yf + invSize, 0 };
        pVertices[3].position = { xf + invSize, yf + 1, 0 };
        pVertices[0].color = nCol[4];
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = nCol[4];

        pVertices += 4;
        pVertices[0].position = { xf + 1, yf + invSize, 0 };
        pVertices[1].position = { xf + 1, yf + 1, 0 };
        pVertices[2].position = { xf + invSize, yf + 1, 0 };
        pVertices[3].position = { xf + invSize, yf + invSize, 0 };
        pVertices[0].color = nCol[2];
        pVertices[1].color = nCol[3];
        pVertices[2].color = nCol[4];
        pVertices[3].color = col;

        ws_pntc_count += 4 * 9;
    }
}

void ws_draw_door_floor(int x, int y, int color, bool* neighbors, float percent)
{
    if (!ws_ao_enabled)
    {
        ws_draw_floor(x, y, color, nullptr);
        return;
    }

    ws_prepare_for_pntc(GL_QUADS, ws_resources.whiteTexture);

    auto xf = (float)x;
    auto yf = 63.0f - (float)y;
    auto zf = 0.01f;

    auto pVertices = ws_resources.pPNTCVertices + ws_pntc_count;
    ws_Color col = { 1, 1, 1, 1 };
    ws_Color aoCol = { ws_ao_amount, ws_ao_amount, ws_ao_amount, 1 };
    for (int i = 0; i < 4 * 4; ++i)
    {
        pVertices[i].normal = { 0, 0, 1 };
        pVertices[i].texCoord = { 0, 0 }; // no textures on floors
    }

    if (neighbors[2])
    {
        pVertices[0].position = { xf + percent, yf + 0.5f, zf };
        pVertices[1].position = { xf + percent, yf + 0.5f - ws_ao_size, zf };
        pVertices[2].position = { xf + 1.0f + percent, yf + 0.5f - ws_ao_size, zf };
        pVertices[3].position = { xf + 1.0f + percent, yf + 0.5f, zf };
        pVertices[0].color = aoCol;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + percent - ws_ao_size, yf + 0.5f, zf };
        pVertices[1].position = { xf + percent - ws_ao_size, yf + 0.5f - ws_ao_size, zf };
        pVertices[2].position = { xf + percent, yf + 0.5f - ws_ao_size, zf };
        pVertices[3].position = { xf + percent, yf + 0.5f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + percent, yf + 0.5f + ws_ao_size, zf };
        pVertices[1].position = { xf + percent, yf + 0.5f, zf };
        pVertices[2].position = { xf + 1.0f + percent, yf + 0.5f, zf };
        pVertices[3].position = { xf + 1.0f + percent, yf + 0.5f + ws_ao_size, zf };
        pVertices[0].color = col;
        pVertices[1].color = aoCol;
        pVertices[2].color = aoCol;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + percent, yf + 0.5f + ws_ao_size, zf };
        pVertices[1].position = { xf + percent - ws_ao_size, yf + 0.5f + ws_ao_size, zf };
        pVertices[2].position = { xf + percent - ws_ao_size, yf + 0.5f, zf };
        pVertices[3].position = { xf + percent, yf + 0.5f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        ws_pntc_count += 4 * 4;
    }
    else
    {
        pVertices[0].position = { xf + 0.5f - ws_ao_size, yf - percent, zf };
        pVertices[1].position = { xf + 0.5f, yf - percent, zf };
        pVertices[2].position = { xf + 0.5f, yf + 1.0f - percent, zf };
        pVertices[3].position = { xf + 0.5f - ws_ao_size, yf + 1.0f - percent, zf };
        pVertices[0].color = col;
        pVertices[1].color = aoCol;
        pVertices[2].color = aoCol;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + 0.5f, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[1].position = { xf + 0.5f - ws_ao_size, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[2].position = { xf + 0.5f - ws_ao_size, yf - percent + 1.0f, zf };
        pVertices[3].position = { xf + 0.5f, yf - percent + 1.0f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + 0.5f, yf - percent, zf };
        pVertices[1].position = { xf + 0.5f + ws_ao_size, yf - percent, zf };
        pVertices[2].position = { xf + 0.5f + ws_ao_size, yf + 1.0f - percent, zf };
        pVertices[3].position = { xf + 0.5f, yf + 1.0f - percent, zf };
        pVertices[0].color = aoCol;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + 0.5f + ws_ao_size, yf - percent + 1.0f, zf };
        pVertices[1].position = { xf + 0.5f + ws_ao_size, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[2].position = { xf + 0.5f, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[3].position = { xf + 0.5f, yf - percent + 1.0f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        ws_pntc_count += 4 * 4;
    }
}

void ws_draw_door_ceiling(int x, int y, int color, bool* neighbors, float percent)
{
    if (!ws_ao_enabled)
    {
        ws_draw_floor(x, y, color, nullptr);
        return;
    }

    ws_prepare_for_pntc(GL_QUADS, ws_resources.whiteTexture);

    auto xf = (float)x;
    auto yf = 63.0f - (float)y;
    auto zf = 0.99f;

    auto pVertices = ws_resources.pPNTCVertices + ws_pntc_count;
    ws_Color col = { 1, 1, 1, 1 };
    ws_Color aoCol = { ws_ao_amount, ws_ao_amount, ws_ao_amount, 1 };
    for (int i = 0; i < 4 * 4; ++i)
    {
        pVertices[i].normal = { 0, 0, 1 };
        pVertices[i].texCoord = { 0, 0 }; // no textures on floors
    }

    if (neighbors[2])
    {
        pVertices[0].position = { xf + percent, yf + 0.5f - ws_ao_size, zf };
        pVertices[1].position = { xf + percent, yf + 0.5f, zf };
        pVertices[2].position = { xf + 1.0f + percent, yf + 0.5f, zf };
        pVertices[3].position = { xf + 1.0f + percent, yf + 0.5f - ws_ao_size, zf };
        pVertices[0].color = col;
        pVertices[1].color = aoCol;
        pVertices[2].color = aoCol;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + percent, yf + 0.5f - ws_ao_size, zf };
        pVertices[1].position = { xf + percent - ws_ao_size, yf + 0.5f - ws_ao_size, zf };
        pVertices[2].position = { xf + percent - ws_ao_size, yf + 0.5f, zf };
        pVertices[3].position = { xf + percent, yf + 0.5f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + percent, yf + 0.5f, zf };
        pVertices[1].position = { xf + percent, yf + 0.5f + ws_ao_size, zf };
        pVertices[2].position = { xf + 1.0f + percent, yf + 0.5f + ws_ao_size, zf };
        pVertices[3].position = { xf + 1.0f + percent, yf + 0.5f, zf };
        pVertices[0].color = aoCol;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + percent - ws_ao_size, yf + 0.5f, zf };
        pVertices[1].position = { xf + percent - ws_ao_size, yf + 0.5f + ws_ao_size, zf };
        pVertices[2].position = { xf + percent, yf + 0.5f + ws_ao_size, zf };
        pVertices[3].position = { xf + percent, yf + 0.5f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        ws_pntc_count += 4 * 4;
    }
    else
    {
        pVertices[0].position = { xf + 0.5f, yf - percent, zf };
        pVertices[1].position = { xf + 0.5f - ws_ao_size, yf - percent, zf };
        pVertices[2].position = { xf + 0.5f - ws_ao_size, yf + 1.0f - percent, zf };
        pVertices[3].position = { xf + 0.5f, yf + 1.0f - percent, zf };
        pVertices[0].color = aoCol;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + 0.5f - ws_ao_size, yf - percent + 1.0f, zf };
        pVertices[1].position = { xf + 0.5f - ws_ao_size, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[2].position = { xf + 0.5f, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[3].position = { xf + 0.5f, yf - percent + 1.0f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        pVertices += 4;
        pVertices[0].position = { xf + 0.5f + ws_ao_size, yf - percent, zf };
        pVertices[1].position = { xf + 0.5f, yf - percent, zf };
        pVertices[2].position = { xf + 0.5f, yf + 1.0f - percent, zf };
        pVertices[3].position = { xf + 0.5f + ws_ao_size, yf + 1.0f - percent, zf };
        pVertices[0].color = col;
        pVertices[1].color = aoCol;
        pVertices[2].color = aoCol;
        pVertices[3].color = col;

        pVertices += 4;
        pVertices[0].position = { xf + 0.5f, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[1].position = { xf + 0.5f + ws_ao_size, yf - percent + ws_ao_size + 1.0f, zf };
        pVertices[2].position = { xf + 0.5f + ws_ao_size, yf - percent + 1.0f, zf };
        pVertices[3].position = { xf + 0.5f, yf - percent + 1.0f, zf };
        pVertices[0].color = col;
        pVertices[1].color = col;
        pVertices[2].color = col;
        pVertices[3].color = aoCol;

        ws_pntc_count += 4 * 4;
    }
}
