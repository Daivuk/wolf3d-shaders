#include "ws.h"

#include "WL_DEF.H"

#include <set>

// the door is the last picture before the sprites
#define DOORWALL	(PMSpriteStart-8)

int ws_culled_rect[4] = { 0, 0, 63, 63 };
bool ws_visible_tiles[64][64] = { false };

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

inline int wall_at(int x, int y)
{
    if (tilemap[x][y] & 0x80) return 0;
    return tilemap[x][y];
}

inline _boolean is_door_at(int x, int y)
{
    return
        ((tilemap[x][y] & 0x80) && !(tilemap[x][y] & 0x40))
        ? _true : _false;
}

inline int door_percenti(int x, int y)
{
    auto doorNum = tilemap[x][y] & 0x3F;
    return (int)doorposition[doorNum];
}

inline float door_percent(int x, int y)
{
    auto doorNum = tilemap[x][y] & 0x3F;
    return (float)doorposition[doorNum] / (float)0xffff;
}

inline bool has_floor_at(int x, int y)
{
    if (pwallstate && pwallx == x && pwally == y) return true;
    byte num = tilemap[x][y];
    return !num || ((num & 0x80) && !(num & 0x40));
}

bool tile_line_of_sight(float offsetX, float offsetY, int fromX, int fromY, int toX, int toY)
{
    if (fromX == toX && fromY == toY) return true;

    float x = (float)fromX + offsetX;
    float y = (float)fromY + offsetY;

    float dx = 0.0f;
    float dy = 0.0f;

    int iter = 0;

    if (abs(toX - fromX) > abs(toY - fromY))
    {
        iter = abs(toX - fromX);
        dx = (toX > fromX) ? 1.0f : -1.0f;
        dy = ((float)toY - (float)fromY) / (float)iter;
    }
    else
    {
        iter = abs(toY - fromY);
        dx = ((float)toX - (float)fromX) / (float)iter;
        dy = (toY > fromY) ? 1.0f : -1.0f;
    }

    for (int i = 0; i < iter; ++i)
    {
        x += dx;
        y += dy;
        if (wall_at((float)x, (float)y)) return false;
    }

    return true;
}

void ws_update_culling()
{
    memset(ws_visible_tiles, ws_debug_view_enabled, sizeof(ws_visible_tiles));
    if (ws_debug_view_enabled)
    {
        ws_culled_rect[0] = 0;
        ws_culled_rect[1] = 0;
        ws_culled_rect[2] = 63;
        ws_culled_rect[3] = 63;
        return;
    }

    static bool newState[64][64];
    auto visibles = newState;
    memset(newState, 0, sizeof(newState));

    // Use fill algorithm from the player's view
    float pxf = (float)player->x / 65536.f;
    float pyf = (float)player->y / 65536.f;
    int px = (int)player->tilex;
    int py = (int)player->tiley;
    
    ws_culled_rect[0] = px;
    ws_culled_rect[1] = py;
    ws_culled_rect[2] = px;
    ws_culled_rect[3] = py;
    visibles[px][py] = true;

    static std::set<std::pair<int, int>> check_next;
    static std::set<std::pair<int, int>> checked;
    check_next.clear();
    checked.clear();
    checked.insert(std::make_pair(px, py));
    if (has_floor_at(px - 1, py)) check_next.insert(std::make_pair(px - 1, py));
    if (has_floor_at(px, py - 1)) check_next.insert(std::make_pair(px, py - 1));
    if (has_floor_at(px + 1, py)) check_next.insert(std::make_pair(px + 1, py));
    if (has_floor_at(px, py + 1)) check_next.insert(std::make_pair(px, py + 1));
    while (!check_next.empty())
    {
        // Grab next
        auto it = check_next.begin();
        int tx = it->first;
        int ty = it->second;
        checked.insert(std::make_pair(tx, ty));
        check_next.erase(it);

        // Check if not obstructed
        if (!tile_line_of_sight(0.5f, 0.5f, px, py, tx, ty) &&
            !tile_line_of_sight(0.2f, 0.2f, px, py, tx, ty) &&
            !tile_line_of_sight(0.8f, 0.2f, px, py, tx, ty) &&
            !tile_line_of_sight(0.2f, 0.8f, px, py, tx, ty) &&
            !tile_line_of_sight(0.8f, 0.8f, px, py, tx, ty))
            continue;

        // Turn on
        visibles[tx][ty] = true;
        ws_culled_rect[0] = std::min(ws_culled_rect[0], tx);
        ws_culled_rect[1] = std::min(ws_culled_rect[1], ty);
        ws_culled_rect[2] = std::max(ws_culled_rect[2], tx);
        ws_culled_rect[3] = std::max(ws_culled_rect[3], ty);

        // Add neighbors
        auto isDoor = is_door_at(tx, ty);
        if (!isDoor || door_percenti(tx, ty) > 0)
        {
            if (has_floor_at(tx - 1, ty) && !checked.count(std::make_pair(tx - 1, ty)))
            {
                check_next.insert(std::make_pair(tx - 1, ty));
            }
            if (has_floor_at(tx, ty - 1) && !checked.count(std::make_pair(tx, ty - 1)))
            {
                check_next.insert(std::make_pair(tx, ty - 1));
            }
            if (has_floor_at(tx + 1, ty) && !checked.count(std::make_pair(tx + 1, ty)))
            {
                check_next.insert(std::make_pair(tx + 1, ty));
            }
            if (has_floor_at(tx, ty + 1) && !checked.count(std::make_pair(tx, ty + 1)))
            {
                check_next.insert(std::make_pair(tx, ty + 1));
            }
        }
    }

    // Post process, expand 1 tile every direction to avoid glitches
    for (int x = ws_culled_rect[0]; x <= ws_culled_rect[2]; ++x)
    {
        for (int y = ws_culled_rect[1]; y <= ws_culled_rect[3]; ++y)
        {
            if (visibles[x][y])
            {
                ws_visible_tiles[x - 1][y - 1] = true;
                ws_visible_tiles[x][y - 1] = true;
                ws_visible_tiles[x + 1][y - 1] = true;
                ws_visible_tiles[x - 1][y] = true;
                ws_visible_tiles[x][y] = true;
                ws_visible_tiles[x + 1][y] = true;
                ws_visible_tiles[x - 1][y + 1] = true;
                ws_visible_tiles[x][y + 1] = true;
                ws_visible_tiles[x + 1][y + 1] = true;
            }
        }
    }
    
    // Increase our view rect
    ws_culled_rect[0] = std::max(ws_culled_rect[0] - 1, 0);
    ws_culled_rect[1] = std::max(ws_culled_rect[1] - 1, 0);
    ws_culled_rect[2] = std::min(ws_culled_rect[2] + 1, 63);
    ws_culled_rect[3] = std::min(ws_culled_rect[3] + 1, 63);
}

void ws_draw_walls()
{
    auto hwalls = horizwall;
    auto vwalls = (ws_ao_enabled || ws_deferred_enabled) ? horizwall : vertwall;

    for (int x = ws_culled_rect[0]; x <= ws_culled_rect[2]; ++x)
    {
        for (int y = ws_culled_rect[1]; y <= ws_culled_rect[3]; ++y)
        {
            if (!ws_visible_tiles[x][y]) continue;
            if (!wall_at(x, y))
            {
                if (int w = wall_at(x, y - 1))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 3;
                    else
                        w = hwalls[w & 63];
                    ws_draw_wall((float)x, (float)y, SOUTH, w, false,
                        wall_at(x - 1, y) ? 1 : (wall_at(x - 1, y - 1) ? 0 : -1),
                        wall_at(x + 1, y) ? 1 : (wall_at(x + 1, y - 1) ? 0 : -1));
                }
                if (int w = wall_at(x - 1, y))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 2;
                    else
                        w = vwalls[w & 63];
                    ws_draw_wall((float)x, (float)y + 1.0f, EAST, w, false,
                        wall_at(x, y + 1) ? 1 : (wall_at(x - 1, y + 1) ? 0 : -1),
                        wall_at(x, y - 1) ? 1 : (wall_at(x - 1, y - 1) ? 0 : -1));
                }
                if (int w = wall_at(x, y + 1))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 3;
                    else
                        w = hwalls[w & 63];
                    ws_draw_wall((float)x + 1.0f, (float)y + 1.0f, NORTH, w, false,
                        wall_at(x + 1, y) ? 1 : (wall_at(x + 1, y + 1) ? 0 : -1),
                        wall_at(x - 1, y) ? 1 : (wall_at(x - 1, y + 1) ? 0 : -1));
                }
                if (int w = wall_at(x + 1, y))
                {
                    if (is_door_at(x, y))
                        w = DOORWALL + 2;
                    else
                        w = vwalls[w & 63];
                    ws_draw_wall((float)x + 1.0f, (float)y, WEST, w, false,
                        wall_at(x, y - 1) ? 1 : (wall_at(x + 1, y - 1) ? 0 : -1),
                        wall_at(x, y + 1) ? 1 : (wall_at(x + 1, y + 1) ? 0 : -1));
                }

                if (is_door_at(x, y))
                {
                    auto percent = door_percent(x, y);
                    auto tilehit = tilemap[x][y];
                    uint16_t wallpic, doornum;
                    doornum = tilehit & 0x7f;

                    switch (doorobjlist[doornum].lock)
                    {
                    case dr_normal:
                        wallpic = (PMSpriteStart - 8);
                        break;
                    case dr_lock1:
                    case dr_lock2:
                    case dr_lock3:
                    case dr_lock4:
                        wallpic = (PMSpriteStart - 8) + 6;
                        break;
                    case dr_elevator:
                        wallpic = (PMSpriteStart - 8) + 4;
                        break;
                    }

                    if (wall_at(x - 1, y))
                    {
                        ws_draw_wall((float)x + percent, (float)y + 0.5f, SOUTH, wallpic, true, 0, 0);
                        ws_draw_wall((float)x + 1.0f + percent, (float)y + 0.5f, NORTH, wallpic, true, 0, 0);
                    }
                    else if (wall_at(x, y - 1))
                    {
                        ws_draw_wall((float)x + 0.5f, (float)y + 1.0f + percent, EAST, wallpic, true, 0, 0);
                        ws_draw_wall((float)x + 0.5f, (float)y + percent, WEST, wallpic, true, 0, 0);
                    }
                }
            }
        }
    }

    extern uint16_t vgaCeiling[];
    int ceiling = vgaCeiling[gamestate.episode * 10 + mapon] & 0xFF;

    // Push walls
    if (pwallstate)
    {
        float percent = (float)pwallpos / 64.0f;
        const float DIROFS[4][2] = {
            {0.0f, -1.0f},
            {1.0f, 0.0f},
            {0.0f, 1.0f},
            {-1.0f, -0.0f} };
        const int DIROFSi[4][2] = {
            {0, -1},
            {1, 0},
            {0, 1},
            {-1, -0} };
        auto &ofs = DIROFS[pwalldir];
        int n = wall_at(pwallx + DIROFSi[pwalldir][0], pwally + DIROFSi[pwalldir][1]);
        int w = hwalls[n & 63];
        ws_draw_wall((float)pwallx + ofs[0] * percent, (float)pwally + 1.0f + ofs[1] * percent, SOUTH, w);
        ws_draw_wall((float)pwallx + 1.0f + ofs[0] * percent, (float)pwally + ofs[1] * percent, NORTH, w);
        w = vwalls[n & 63];
        ws_draw_wall((float)pwallx + 1.0f + ofs[0] * percent, (float)pwally + 1.0f + ofs[1] * percent, EAST, w);
        ws_draw_wall((float)pwallx + ofs[0] * percent, (float)pwally + ofs[1] * percent, WEST, w);
    }

    // Draw ceilling and floor tile by tile (We could have gone with a full map plane, but then we would be depth testing in useless places and its not
    // as nice to look at in freecam. + we need the AO to be done per tile
    bool neighbors[] = { false, false, false, false, false, false, false, false };
    for (int x = ws_culled_rect[0]; x <= ws_culled_rect[2]; ++x)
    {
        for (int y = ws_culled_rect[1]; y <= ws_culled_rect[3]; ++y)
        {
            if (!ws_visible_tiles[x][y]) continue;
            if (has_floor_at(x, y))
            {
                neighbors[0] = !has_floor_at(x, y + 1);
                neighbors[1] = !has_floor_at(x + 1, y + 1);
                neighbors[2] = !has_floor_at(x + 1, y);
                neighbors[3] = !has_floor_at(x + 1, y - 1);
                neighbors[4] = !has_floor_at(x, y - 1);
                neighbors[5] = !has_floor_at(x - 1, y - 1);
                neighbors[6] = !has_floor_at(x - 1, y);
                neighbors[7] = !has_floor_at(x - 1, y + 1);

                ws_draw_ceiling(x, y, ceiling, neighbors);
                ws_draw_floor(x, y, 0x19, neighbors);
            }
        }
    }

    // Door overlays - not now
    ws_flush();
    //glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
    //glDepthMask(GL_FALSE);
    //for (int x = 1; x < 63; ++x)
    //{
    //    for (int y = 1; y < 63; ++y)
    //    {
    //        if (is_door_at(x, y))
    //        {
    //            neighbors[0] = !has_floor_at(x, y + 1);
    //            neighbors[1] = !has_floor_at(x + 1, y + 1);
    //            neighbors[2] = !has_floor_at(x + 1, y);
    //            neighbors[3] = !has_floor_at(x + 1, y - 1);
    //            neighbors[4] = !has_floor_at(x, y - 1);
    //            neighbors[5] = !has_floor_at(x - 1, y - 1);
    //            neighbors[6] = !has_floor_at(x - 1, y);
    //            neighbors[7] = !has_floor_at(x - 1, y + 1);

    //            float percent = door_percent(x, y);
    //            ws_draw_door_ceiling(x, y, ceiling, neighbors, percent);
    //            ws_draw_door_floor(x, y, 0x19, neighbors, percent);
    //        }
    //    }
    //}
    //ws_flush();
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glDepthMask(GL_TRUE);

    glPolygonMode(GL_FRONT, GL_FILL); // Only walls we do in wireframe
}
