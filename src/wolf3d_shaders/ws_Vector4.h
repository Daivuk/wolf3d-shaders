#ifndef VECTOR4_H_INCLUDED
#define VECTOR4_H_INCLUDED


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// Onut
// #include <onut/Align.h>

// STL
#include <algorithm>
#include <cmath>

// Forward Declarations
struct ws_Matrix;
struct ws_Vector2;
struct ws_Vector3;
struct ws_Vector4;

using Rect = ws_Vector4;

ws_Vector4 operator+(const ws_Vector4& V1, const ws_Vector4& V2);
ws_Vector4 operator-(const ws_Vector4& V1, const ws_Vector4& V2);
ws_Vector4 operator*(const ws_Vector4& V1, const ws_Vector4& V2);
ws_Vector4 operator*(const ws_Vector4& V, float S);
ws_Vector4 operator/(const ws_Vector4& V1, const ws_Vector4& V2);
ws_Vector4 operator*(float S, const ws_Vector4& V);

struct ws_Vector4
{
    float x, y, z, w;

    ws_Vector4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
    explicit ws_Vector4(float s) : x(s), y(s), z(s), w(s) {}
    ws_Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    ws_Vector4(const ws_Vector2& pos, float _z, float _w);
    ws_Vector4(const ws_Vector2& pos, const ws_Vector2& size);
    ws_Vector4(float _x, float _y, const ws_Vector2& size);
    ws_Vector4(const ws_Vector3& pos, float _w);
    ws_Vector4(const ws_Vector3& pos);
    explicit ws_Vector4(const float *pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]), w(pArray[3]) {}

    // Comparison operators
    bool operator == (const ws_Vector4& V) const
    {
        return x == V.x && y == V.y && z == V.z && w == V.w;
    }
    bool operator != (const ws_Vector4& V) const
    {
        return x != V.x || y != V.y || z != V.z || w != V.w;
    }

    // Assignment operators
    ws_Vector4& operator= (const ws_Vector4& V) { x = V.x; y = V.y; z = V.z; w = V.w; return *this; }
    ws_Vector4& operator+= (const ws_Vector4& V) { x += V.x; y += V.y; z += V.z; w += V.w; return *this; }
    ws_Vector4& operator-= (const ws_Vector4& V) { x -= V.x; y -= V.y; z -= V.z; w -= V.w; return *this; }
    ws_Vector4& operator*= (const ws_Vector4& V) { x *= V.x; y *= V.y; z *= V.z; w *= V.w; return *this; }
    ws_Vector4& operator*= (float S) { x *= S; y *= S; z *= S; w *= S; return *this; }
    ws_Vector4& operator/= (float S)
    {
        auto invS = 1.0f / S;
        x *= invS; y *= invS; z *= invS; w *= invS;
        return *this;
    }

    // Urnary operators
    ws_Vector4 operator+ () const { return *this; }
    ws_Vector4 operator- () const { return ws_Vector4(-x, -y, -z, -w); }

    // Vector operations
    bool InBounds(const ws_Vector4& Bounds) const
    {
        return !(
            (x <= Bounds.x && x >= -Bounds.x) &&
            (y <= Bounds.y && y >= -Bounds.y) &&
            (z <= Bounds.z && z >= -Bounds.z) &&
            (w <= Bounds.w && w >= -Bounds.w));
    }

    float Distance(const ws_Vector2& p) const;

    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
    float LengthSquared() const
    {
        return (x * x + y * y + z * z + w * w);
    }

    float Dot(const ws_Vector4& V) const
    {
        return x * V.x + y * V.z + z * V.z + w * V.w;
    }
    void Cross(const ws_Vector4& v2, const ws_Vector4& v3, ws_Vector4& result) const
    {
        const ws_Vector4& v1 = *this;
        result.x = ((v2.z*v3.w - v2.w*v3.z)*v1.y) - ((v2.y*v3.w - v2.w*v3.y)*v1.z) + ((v2.y*v3.z - v2.z*v3.y)*v1.w);
        result.y = ((v2.w*v3.z - v2.z*v3.w)*v1.x) - ((v2.w*v3.x - v2.x*v3.w)*v1.z) + ((v2.z*v3.x - v2.x*v3.z)*v1.w);
        result.z = ((v2.y*v3.w - v2.w*v3.y)*v1.x) - ((v2.x*v3.w - v2.w*v3.x)*v1.y) + ((v2.x*v3.y - v2.y*v3.x)*v1.w);
        result.w = ((v2.z*v3.y - v2.y*v3.z)*v1.x) - ((v2.z*v3.x - v2.x*v3.z)*v1.y) + ((v2.y*v3.x - v2.x*v3.y)*v1.z);
    }
    ws_Vector4 Cross(const ws_Vector4& v2, const ws_Vector4& v3) const
    {
        ws_Vector4 result;
        const ws_Vector4& v1 = *this;
        result.x = ((v2.z*v3.w - v2.w*v3.z)*v1.y) - ((v2.y*v3.w - v2.w*v3.y)*v1.z) + ((v2.y*v3.z - v2.z*v3.y)*v1.w);
        result.y = ((v2.w*v3.z - v2.z*v3.w)*v1.x) - ((v2.w*v3.x - v2.x*v3.w)*v1.z) + ((v2.z*v3.x - v2.x*v3.z)*v1.w);
        result.z = ((v2.y*v3.w - v2.w*v3.y)*v1.x) - ((v2.x*v3.w - v2.w*v3.x)*v1.y) + ((v2.x*v3.y - v2.y*v3.x)*v1.w);
        result.w = ((v2.z*v3.y - v2.y*v3.z)*v1.x) - ((v2.z*v3.x - v2.x*v3.z)*v1.y) + ((v2.y*v3.x - v2.x*v3.y)*v1.z);
        return std::move(result);
    }

    void Normalize()
    {
        auto len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len > 0.0f)
        {
            len = 1.0f / len;
        }
        x *= len;
        y *= len;
        z *= len;
        w *= len;
    }
    void Normalize(ws_Vector4& result) const
    {
        auto len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len > 0.0f)
        {
            len = 1.0f / len;
        }
        result.x = x * len;
        result.y = y * len;
        result.z = z * len;
        result.w = w * len;
    }

    void Clamp(const ws_Vector4& vmin, const ws_Vector4& vmax)
    {
        x = std::max(vmin.x, x);
        x = std::min(vmax.x, x);
        y = std::max(vmin.y, y);
        y = std::min(vmax.y, y);
        z = std::max(vmin.z, z);
        z = std::min(vmax.z, z);
        w = std::max(vmin.w, w);
        w = std::min(vmax.w, w);
    }
    void Clamp(const ws_Vector4& vmin, const ws_Vector4& vmax, ws_Vector4& result) const
    {
        result.x = std::max(vmin.x, x);
        result.x = std::min(vmax.x, result.x);
        result.y = std::max(vmin.y, y);
        result.y = std::min(vmax.y, result.y);
        result.z = std::max(vmin.z, z);
        result.z = std::min(vmax.z, result.z);
        result.w = std::max(vmin.w, w);
        result.w = std::min(vmax.w, result.w);
    }

    ws_Vector2 TopLeft() const;
    ws_Vector2 Top() const;
    ws_Vector2 TopRight() const;
    ws_Vector2 Left() const;
    ws_Vector2 Center() const;
    ws_Vector2 Right() const;
    ws_Vector2 BottomLeft() const;
    ws_Vector2 Bottom() const;
    ws_Vector2 BottomRight() const;

    inline ws_Vector4 TopLeft(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x, y, rect.z, rect.w});
    }
    inline ws_Vector4 Top(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x + z * .5f - rect.z * .5f, y, rect.z, rect.w});
    }
    inline ws_Vector4 TopRight(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x + z - rect.z, y, rect.z, rect.w});
    }
    inline ws_Vector4 Left(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x, y + w * .5f - rect.w * .5f, rect.z, rect.w});
    }
    inline ws_Vector4 Center(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x + z * .5f - rect.z * .5f, y + w * .5f - rect.w * .5f, rect.z, rect.w});
    }
    inline ws_Vector4 Right(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x + z - rect.z, y + w * .5f - rect.w * .5f, rect.z, rect.w});
    }
    inline ws_Vector4 BottomLeft(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x, y + w - rect.w, rect.z, rect.w});
    }
    inline ws_Vector4 Bottom(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x + z * .5f - rect.z * .5f, y + w - rect.w, rect.z, rect.w});
    }
    inline ws_Vector4 BottomRight(const ws_Vector4& rect) const
    {
        return std::move(ws_Vector4{x + z - rect.z, y + w - rect.w, rect.z, rect.w});
    }

    ws_Vector2 TopLeft(float offset) const;
    ws_Vector2 Top(float offset) const;
    ws_Vector2 TopRight(float offset) const;
    ws_Vector2 Left(float offset) const;
    ws_Vector2 Center(float offset) const;
    ws_Vector2 Right(float offset) const;
    ws_Vector2 BottomLeft(float offset) const;
    ws_Vector2 Bottom(float offset) const;
    ws_Vector2 BottomRight(float offset) const;
    ws_Vector2 TopLeft(const ws_Vector2& offset) const;
    ws_Vector2 TopRight(const ws_Vector2& offset) const;
    ws_Vector2 BottomLeft(const ws_Vector2& offset) const;
    ws_Vector2 BottomRight(const ws_Vector2& offset) const;

    ws_Vector4 Fill(const ws_Vector2& size) const;

    inline ws_Vector4 Fill(const ws_Vector4& rect) const
    {
        float maxScale = std::max<float>(z / rect.z, w / rect.w);
        return std::move(Center({0, 0, rect.z * maxScale, rect.w * maxScale}));
    }

    ws_Vector4 Fit(const ws_Vector2& size) const;

    inline ws_Vector4 Fit(const ws_Vector4& rect) const
    {
        Rect ret;

        float scale = std::min<>(z / rect.z, w / rect.w);

        ret.x = x + z * .5f - rect.z * scale * .5f;
        ret.y = y + w * .5f - rect.w * scale * .5f;
        ret.z = rect.z * scale;
        ret.w = rect.w * scale;

        return std::move(ret);
    }

    inline ws_Vector4 Grow(float by) const
    {
        return std::move(ws_Vector4{x - by, y - by, z + by * 2, w + by * 2});
    }

    bool Contains(const ws_Vector2& point) const;


    // Static functions
    static float Distance(const ws_Vector4& v1, const ws_Vector4& v2)
    {
        auto diff = v2 - v1;
        return diff.Length();
    }
    static float DistanceSquared(const ws_Vector4& v1, const ws_Vector4& v2)
    {
        auto diff = v2 - v1;
        return diff.LengthSquared();
    }

    static void Min(const ws_Vector4& v1, const ws_Vector4& v2, ws_Vector4& result)
    {
        result.x = std::min(v1.x, v2.x);
        result.y = std::min(v1.y, v2.y);
        result.z = std::min(v1.z, v2.z);
        result.w = std::min(v1.w, v2.w);
    }
    static ws_Vector4 Min(const ws_Vector4& v1, const ws_Vector4& v2)
    {
        ws_Vector4 result;
        result.x = std::min(v1.x, v2.x);
        result.y = std::min(v1.y, v2.y);
        result.z = std::min(v1.z, v2.z);
        result.w = std::min(v1.w, v2.w);
        return result;
    }

    static void Max(const ws_Vector4& v1, const ws_Vector4& v2, ws_Vector4& result)
    {
        result.x = std::max(v1.x, v2.x);
        result.y = std::max(v1.y, v2.y);
        result.z = std::max(v1.z, v2.z);
        result.w = std::max(v1.w, v2.w);
    }
    static ws_Vector4 Max(const ws_Vector4& v1, const ws_Vector4& v2)
    {
        ws_Vector4 result;
        result.x = std::max(v1.x, v2.x);
        result.y = std::max(v1.y, v2.y);
        result.z = std::max(v1.z, v2.z);
        result.w = std::max(v1.w, v2.w);
        return result;
    }

    static void Lerp(const ws_Vector4& v1, const ws_Vector4& v2, float t, ws_Vector4& result)
    {
        result = v1 + (v2 - v1) * t;
    }
    static ws_Vector4 Lerp(const ws_Vector4& v1, const ws_Vector4& v2, float t)
    {
        return v1 + (v2 - v1) * t;
    }

    static void SmoothStep(const ws_Vector4& v1, const ws_Vector4& v2, float t, ws_Vector4& result)
    {
        t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t*t*(3.f - 2.f*t);
        result = v1 + (v2 - v1) * t;
    }
    static ws_Vector4 SmoothStep(const ws_Vector4& v1, const ws_Vector4& v2, float t)
    {
        t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t*t*(3.f - 2.f*t);
        return v1 + (v2 - v1) * t;
    }

    static void Barycentric(const ws_Vector4& v1, const ws_Vector4& v2, const ws_Vector4& v3, float f, float g, ws_Vector4& result)
    {
        auto p10 = v2 - v1;
        auto p20 = v3 - v1;
        result = v1 + p10 * f;
        result = result + p20 * g;
    }
    static ws_Vector4 Barycentric(const ws_Vector4& v1, const ws_Vector4& v2, const ws_Vector4& v3, float f, float g)
    {
        ws_Vector4 result;
        auto p10 = v2 - v1;
        auto p20 = v3 - v1;
        result = v1 + p10 * f;
        result = result + p20 * g;
        return result;
    }

    static void CatmullRom(const ws_Vector4& v1, const ws_Vector4& v2, const ws_Vector4& v3, const ws_Vector4& v4, float t, ws_Vector4& result)
    {
        float t2 = t * t;
        float t3 = t * t2;

        auto P0 = (-t3 + 2.0f * t2 - t) * 0.5f;
        auto P1 = (3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;
        auto P2 = (-3.0f * t3 + 4.0f * t2 + t) * 0.5f;
        auto P3 = (t3 - t2) * 0.5f;

        result = P0 * v1;
        result += v2 * P1;
        result += v3 * P2;
        result += v4 * P3;
    }
    static ws_Vector4 CatmullRom(const ws_Vector4& v1, const ws_Vector4& v2, const ws_Vector4& v3, const ws_Vector4& v4, float t)
    {
        ws_Vector4 result;
        float t2 = t * t;
        float t3 = t * t2;

        auto P0 = (-t3 + 2.0f * t2 - t) * 0.5f;
        auto P1 = (3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;
        auto P2 = (-3.0f * t3 + 4.0f * t2 + t) * 0.5f;
        auto P3 = (t3 - t2) * 0.5f;

        result = P0 * v1;
        result += v2 * P1;
        result += v3 * P2;
        result += v4 * P3;

        return result;
    }

    static void Hermite(const ws_Vector4& Position0, const ws_Vector4& Tangent0, const ws_Vector4& Position1, const ws_Vector4& Tangent1, float t, ws_Vector4& result)
    {
        float t2 = t * t;
        float t3 = t * t2;

        auto P0 = (2.0f * t3 - 3.0f * t2 + 1.0f);
        auto T0 = (t3 - 2.0f * t2 + t);
        auto P1 = (-2.0f * t3 + 3.0f * t2);
        auto T1 = (t3 - t2);

        result = (P0 * Position0);
        result = result + Tangent0 * T0;
        result = result + Position1 * P1;
        result = result + Tangent1 * T1;
    }
    static ws_Vector4 Hermite(const ws_Vector4& Position0, const ws_Vector4& Tangent0, const ws_Vector4& Position1, const ws_Vector4& Tangent1, float t)
    {
        ws_Vector4 result;
        float t2 = t * t;
        float t3 = t * t2;

        auto P0 = (2.0f * t3 - 3.0f * t2 + 1.0f);
        auto T0 = (t3 - 2.0f * t2 + t);
        auto P1 = (-2.0f * t3 + 3.0f * t2);
        auto T1 = (t3 - t2);

        result = (P0 * Position0);
        result = result + Tangent0 * T0;
        result = result + Position1 * P1;
        result = result + Tangent1 * T1;
        return result;
    }

    static void Reflect(const ws_Vector4& Incident, const ws_Vector4& Normal, ws_Vector4& result)
    {
        auto dot = Incident.Dot(Normal);
        dot *= 2.0f;
        result = Incident - Normal * dot;
    }
    static ws_Vector4 Reflect(const ws_Vector4& Incident, const ws_Vector4& Normal)
    {
        auto dot = Incident.Dot(Normal);
        dot *= 2.0f;
        return+Incident - Normal * dot;
    }

    static void Refract(const ws_Vector4& Incident, const ws_Vector4& Normal, float RefractionIndex, ws_Vector4& result)
    {
        float IDotN = Incident.Dot(Normal);

        auto R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN);

        if (R <= 0.0f)
        {
            // Total internal reflection
            result = ws_Vector4::Zero;
        }
        else
        {
            R = RefractionIndex * IDotN + std::sqrt(R);
            result = RefractionIndex * Incident - Normal * R;
        }
    }
    static ws_Vector4 Refract(const ws_Vector4& Incident, const ws_Vector4& Normal, float RefractionIndex)
    {
        ws_Vector4 result;
        float IDotN = Incident.Dot(Normal);

        auto R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN);

        if (R <= 0.0f)
        {
            // Total internal reflection
            result = ws_Vector4::Zero;
        }
        else
        {
            R = RefractionIndex * IDotN + std::sqrt(R);
            result = RefractionIndex * Incident - Normal * R;
        }
        return std::move(result);
    }

    static void Transform(const ws_Vector4& v, const ws_Matrix& m, ws_Vector4& result);
    static ws_Vector4 Transform(const ws_Vector4& v, const ws_Matrix& m);

    // Constants
    static const ws_Vector4 Zero;
    static const ws_Vector4 One;
    static const ws_Vector4 UnitX;
    static const ws_Vector4 UnitY;
    static const ws_Vector4 UnitZ;
    static const ws_Vector4 UnitW;
};

// Binary operators
inline ws_Vector4 operator+(const ws_Vector4& V1, const ws_Vector4& V2)
{
    return ws_Vector4(
        V1.x + V2.x,
        V1.y + V2.y,
        V1.z + V2.z,
        V1.w + V2.w);
}
inline ws_Vector4 operator-(const ws_Vector4& V1, const ws_Vector4& V2)
{
    return ws_Vector4(
        V1.x - V2.x,
        V1.y - V2.y,
        V1.z - V2.z,
        V1.w - V2.w);
}
inline ws_Vector4 operator*(const ws_Vector4& V1, const ws_Vector4& V2)
{
    return ws_Vector4(
        V1.x * V2.x,
        V1.y * V2.y,
        V1.z * V2.z,
        V1.w * V2.w);
}
inline ws_Vector4 operator*(const ws_Vector4& V, float S)
{
    return ws_Vector4(
        V.x * S,
        V.y * S,
        V.z * S,
        V.w * S);
}
inline ws_Vector4 operator/(const ws_Vector4& V1, const ws_Vector4& V2)
{
    return ws_Vector4(
        V1.x / V2.x,
        V1.y / V2.y,
        V1.z / V2.z,
        V1.w / V2.w);
}
inline ws_Vector4 operator*(float S, const ws_Vector4& V)
{
    return ws_Vector4(
        V.x * S,
        V.y * S,
        V.z * S,
        V.w * S);
}

//--- Rect helpers for drawing
// Centered origin sprite
// cx,cy = Sprite center position
#define ORectCenteredOrigin(cx, cy, w, h)       Rect{(cx) - (w) * .5f, (cy) - (h) * .5f, w, h}

// Rectangle helpers
#define ORectFullScreen                         Rect{0, 0, OScreenWf, OScreenHf}
#define ORectGrid(cols, rows, ix, iy, px, py)   Rect{OScreenWf/(cols)*(ix)+((ix>0)?px*.5f:px),OScreenHf/(rows)*(iy)+((iy>0)?py*.5f:py),OScreenWf/(cols)-((ix<=0||ix>=cols-1)?px*1.5f:px),OScreenHf/(rows)-((iy<=0||iy>=rows-1)?py*1.5f:py)}
#define ORectLayout(left, top, right, bottom)   Rect{(left), (top), (right) - (left), (bottom) - (top)}
#define ORectAlign                              onut::alignedRect
template<typename TparentRect, typename Tsize>
inline Rect ORectFit(const TparentRect& parentRect, const Tsize& size)
{
    Rect ret;

    float scale = std::min<>((float)parentRect.z / (float)size.x, (float)parentRect.w / (float)size.y);

    ret.x = (float)parentRect.x + (float)parentRect.z * .5f - (float)size.x * scale * .5f;
    ret.y = (float)parentRect.y + (float)parentRect.w * .5f - (float)size.y * scale * .5f;
    ret.z = (float)size.x * scale;
    ret.w = (float)size.y * scale;

    return std::move(ret);
}

// Smart fit is like OFitRect but avoids distortion
template<typename TparentRect, typename Tsize>
inline Rect ORectSmartFit(const TparentRect& parentRect, const Tsize& size)
{
    Rect ret;

    float scale = std::min<>((float)parentRect.z / (float)size.x, (float)parentRect.w / (float)size.y);
    scale = std::floor(scale);

    ret.x = (float)parentRect.x + (float)parentRect.z * .5f - (float)std::ceil(size.x * scale * .5f);
    ret.y = (float)parentRect.y + (float)parentRect.w * .5f - (float)std::ceil(size.y * scale * .5f);
    ret.z = (float)size.x * scale;
    ret.w = (float)size.y * scale;

    return std::move(ret);
};

namespace onut
{
    /**
    Rect alignement helper
    */
    // Rect alignedRect(float xOffset, float yOffset, float width, float height, float padding = 0, Align align = Align::TopLeft);

    // ws_Vector2 alignedRect(const Rect& rect, Align align = Align::TopLeft);

    // template<Align Talign = Align::TopLeft>
    // Rect alignedRect(const Rect& rect, float padding = 0, Align align = Talign)
    // {
    //     return alignedRect(rect.x, rect.y, rect.z, rect.w, padding, align);
    // }

    // template<Align Talign = Align::TopLeft>
    // Rect alignedRect(float width, float height, float padding = 0, Align align = Talign)
    // {
    //     return alignedRect(0, 0, width, height, padding, align);
    // }

    // template<Align Talign = Align::TopLeft>
    // Rect alignedRect(float size, float padding = 0, Align align = Talign)
    // {
    //     return alignedRect(0, 0, size, size, padding, align);
    // }
}

#endif
