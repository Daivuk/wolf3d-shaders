// This file was taken from the open source engine onut: https://github.com/Daivuk/onut

#include "ws_Matrix.h"
#include "ws_Vector2.h"
#include "ws_Vector3.h"

const ws_Vector2 ws_Vector2::Zero(0.f, 0.f);
const ws_Vector2 ws_Vector2::One(1.f, 1.f);
const ws_Vector2 ws_Vector2::UnitX(1.f, 0.f);
const ws_Vector2 ws_Vector2::UnitY(0.f, 1.f);

ws_Vector2::ws_Vector2(const ws_Vector3& v3) : x(v3.x), y(v3.y)
{
}

void ws_Vector2::Cross(const ws_Vector2& V, ws_Vector3& result) const
{
    result = ws_Vector3(0, 0, (x * V.y) - (y * V.x));
}

ws_Vector3 ws_Vector2::Cross(const ws_Vector2& V) const
{
    return ws_Vector3(0, 0, (x * V.y) - (y * V.x));
}

void ws_Vector2::Transform(const ws_Vector2& v, const ws_Matrix& m, ws_Vector2& result)
{
    float w = 0.0f;
    result.x = (m._11 * v.x) + (m._21 * v.y) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + m._42;
    w = (m._14 * v.x) + (m._24 * v.y) + m._44;
    result /= w;
}

ws_Vector2 ws_Vector2::Transform(const ws_Vector2& v, const ws_Matrix& m)
{
    ws_Vector2 result;
    float w = 0.0f;
    result.x = (m._11 * v.x) + (m._21 * v.y) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + m._42;
    w = (m._14 * v.x) + (m._24 * v.y) + m._44;
    result /= w;
    return result / w;
}

void ws_Vector2::TransformNormal(const ws_Vector2& v, const ws_Matrix& m, ws_Vector2& result)
{
    result.x = (m._11 * v.x) + (m._21 * v.y) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + m._42;
}

ws_Vector2 ws_Vector2::TransformNormal(const ws_Vector2& v, const ws_Matrix& m)
{
    ws_Vector2 result;
    result.x = (m._11 * v.x) + (m._21 * v.y) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + m._42;
    return result;
}
namespace onut
{
    // static bool walkableAt(const ws_Vector2& pos, bool* pPassableTiles, int width, int height, float tileSizef)
    // {
    //     Point mapPos((int)(pos.x / tileSizef), (int)(pos.y / tileSizef));
    //     if (mapPos.x < 0 || mapPos.y < 0 || mapPos.x >= width || mapPos.y >= height) return false;
    //     return pPassableTiles[mapPos.y * width + mapPos.x];
    // }

    // static bool walkableAt(const ws_Vector2& pos, float* pPassableTiles, int width, int height, float tileSizef)
    // {
    //     Point mapPos((int)(pos.x / tileSizef), (int)(pos.y / tileSizef));
    //     if (mapPos.x < 0 || mapPos.y < 0 || mapPos.x >= width || mapPos.y >= height) return false;
    //     return pPassableTiles[mapPos.y * width + mapPos.x] > 0.0f;
    // }

    // ws_Vector2 tilesCollision(const ws_Vector2& from, const ws_Vector2& to, const ws_Vector2& size, bool* pPassableTiles, int width, int height, int tileSize)
    // {
    //     if (to == from) return to;

    //     float tileSizef = (float)tileSize;
    //     Point lastMapPos((int)(from.x / tileSizef), (int)(from.y / tileSizef));
    //     ws_Vector2 dir = to - from;
    //     ws_Vector2 result = to;
    //     ws_Vector2 hSize = size / 2.0f;

    //     if (dir.x < 0)
    //     {
    //         if (!walkableAt(ws_Vector2(to.x - hSize.x, from.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x) * tileSizef + hSize.x;
    //         }
    //         else if (!walkableAt(ws_Vector2(to.x - hSize.x, from.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x) * tileSizef + hSize.x;
    //         }
    //     }
    //     else if (dir.x > 0)
    //     {
    //         if (!walkableAt(ws_Vector2(to.x + hSize.x, from.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x + 1) * tileSizef - hSize.x - (tileSizef / 100.0f);
    //         }
    //         else if (!walkableAt(ws_Vector2(to.x + hSize.x, from.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x + 1) * tileSizef - hSize.x - (tileSizef / 100.0f);
    //         }
    //     }
    //     if (dir.y < 0)
    //     {
    //         if (!walkableAt(ws_Vector2(from.x - hSize.x, to.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y) * tileSizef + hSize.y;
    //         }
    //         else if (!walkableAt(ws_Vector2(from.x + hSize.x, to.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y) * tileSizef + hSize.y;
    //         }
    //     }
    //     else if (dir.y > 0)
    //     {
    //         if (!walkableAt(ws_Vector2(from.x - hSize.x, to.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y + 1) * tileSizef - hSize.y - (tileSizef / 100.0f);
    //         }
    //         else if (!walkableAt(ws_Vector2(from.x + hSize.x, to.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y + 1) * tileSizef - hSize.y - (tileSizef / 100.0f);
    //         }
    //     }

    //     return result;
    // }

    // ws_Vector2 tilesCollision(const ws_Vector2& from, const ws_Vector2& to, const ws_Vector2& size, float* pPassableTiles, int width, int height, int tileSize)
    // {
    //     if (to == from) return to;

    //     float tileSizef = (float)tileSize;
    //     Point lastMapPos((int)(from.x / tileSizef), (int)(from.y / tileSizef));
    //     ws_Vector2 dir = to - from;
    //     ws_Vector2 result = to;
    //     ws_Vector2 hSize = size / 2.0f;

    //     if (dir.x < 0)
    //     {
    //         if (!walkableAt(ws_Vector2(to.x - hSize.x, from.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x) * tileSizef + hSize.x;
    //         }
    //         else if (!walkableAt(ws_Vector2(to.x - hSize.x, from.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x) * tileSizef + hSize.x;
    //         }
    //     }
    //     else if (dir.x > 0)
    //     {
    //         if (!walkableAt(ws_Vector2(to.x + hSize.x, from.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x + 1) * tileSizef - hSize.x - (tileSizef / 100.0f);
    //         }
    //         else if (!walkableAt(ws_Vector2(to.x + hSize.x, from.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.x = (float)(lastMapPos.x + 1) * tileSizef - hSize.x - (tileSizef / 100.0f);
    //         }
    //     }
    //     if (dir.y < 0)
    //     {
    //         if (!walkableAt(ws_Vector2(from.x - hSize.x, to.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y) * tileSizef + hSize.y;
    //         }
    //         else if (!walkableAt(ws_Vector2(from.x + hSize.x, to.y - hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y) * tileSizef + hSize.y;
    //         }
    //     }
    //     else if (dir.y > 0)
    //     {
    //         if (!walkableAt(ws_Vector2(from.x - hSize.x, to.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y + 1) * tileSizef - hSize.y - (tileSizef / 100.0f);
    //         }
    //         else if (!walkableAt(ws_Vector2(from.x + hSize.x, to.y + hSize.y), pPassableTiles, width, height, tileSizef))
    //         {
    //             result.y = (float)(lastMapPos.y + 1) * tileSizef - hSize.y - (tileSizef / 100.0f);
    //         }
    //     }

    //     return result;
    // }
};
