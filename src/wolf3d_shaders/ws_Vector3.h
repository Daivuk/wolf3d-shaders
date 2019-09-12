#ifndef VECTOR3_H_INCLUDED
#define VECTOR3_H_INCLUDED


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// STL
#include <algorithm>
#include <cmath>

// Forward Declarations
struct ws_Matrix;
struct ws_Vector2;
struct ws_Vector3;

ws_Vector3 operator+(const ws_Vector3& V1, const ws_Vector3& V2);
ws_Vector3 operator-(const ws_Vector3& V1, const ws_Vector3& V2);
ws_Vector3 operator*(const ws_Vector3& V1, const ws_Vector3& V2);
ws_Vector3 operator*(const ws_Vector3& V, float S);
ws_Vector3 operator/(const ws_Vector3& V1, const ws_Vector3& V2);
ws_Vector3 operator*(float S, const ws_Vector3& V);

struct ws_Vector3
{
    float x, y, z;

    ws_Vector3() : x(0.f), y(0.f), z(0.f) {}
    explicit ws_Vector3(float _s) : x(_s), y(_s), z(_s) {}
    ws_Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    ws_Vector3(const ws_Vector2& v2, float _z);
    ws_Vector3(const ws_Vector2& v2);
    explicit ws_Vector3(const float *pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]) {}

    // Comparison operators
    bool operator == (const ws_Vector3& V) const
    {
        return x == V.x && y == V.y && z == V.z;
    }
    bool operator != (const ws_Vector3& V) const
    {
        return x != V.x || y != V.y || z != V.z;
    }

    // Assignment operators
    ws_Vector3& operator= (const ws_Vector3& V) { x = V.x; y = V.y; z = V.z; return *this; }
    ws_Vector3& operator+= (const ws_Vector3& V) { x += V.x; y += V.y; z += V.z; return *this; }
    ws_Vector3& operator-= (const ws_Vector3& V) { x -= V.x; y -= V.y; z -= V.z; return *this; }
    ws_Vector3& operator*= (const ws_Vector3& V) { x *= V.x; y *= V.y; z *= V.z; return *this; }
    ws_Vector3& operator*= (float S) { x *= S; y *= S; z *= S; return *this; }
    ws_Vector3& operator/= (float S) 
    {
        auto invS = 1.0f / S;
        x *= invS; y *= invS; z *= invS;
        return *this;
    }

    // Urnary operators
    ws_Vector3 operator+ () const { return *this; }
    ws_Vector3 operator- () const
    {
        return ws_Vector3(-x, -y, -z);
    }

    // Vector operations
    bool InBounds(const ws_Vector3& Bounds) const
    {
        return !(
            (x <= Bounds.x && x >= -Bounds.x) &&
            (y <= Bounds.y && y >= -Bounds.y) &&
            (z <= Bounds.z && z >= -Bounds.z));
    }

    float Length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    float LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    float Dot(const ws_Vector3& V) const
    {
        return x * V.x + y * V.y + z * V.z;
    }
    void Cross(const ws_Vector3& V, ws_Vector3& result) const
    {
        result.x = y * V.z - z * V.y;
        result.y = z * V.x - x * V.z;
        result.z = x * V.y - y * V.x;
    }
    ws_Vector3 Cross(const ws_Vector3& V) const
    {
        ws_Vector3 result;
        result.x = y * V.z - z * V.y;
        result.y = z * V.x - x * V.z;
        result.z = x * V.y - y * V.x;
        return std::move(result);
    }

    void Normalize()
    {
        auto len = std::sqrt(x * x + y * y + z * z);
        if (len > 0.0f)
        {
            len = 1.0f / len;
        }
        x *= len;
        y *= len;
        z *= len;
    }
    void Normalize(ws_Vector3& result) const
    {
        auto len = std::sqrt(x * x + y * y + z * z);
        if (len > 0.0f)
        {
            len = 1.0f / len;
        }
        result.x = x * len;
        result.y = y * len;
        result.z = z * len;
    }

    void Clamp(const ws_Vector3& vmin, const ws_Vector3& vmax)
    {
        x = std::max(vmin.x, x);
        x = std::min(vmax.x, x);
        y = std::max(vmin.y, y);
        y = std::min(vmax.y, y);
        z = std::max(vmin.z, z);
        z = std::min(vmax.z, z);
    }
    void Clamp(const ws_Vector3& vmin, const ws_Vector3& vmax, ws_Vector3& result) const
    {
        result.x = std::max(vmin.x, x);
        result.x = std::min(vmax.x, result.x);
        result.y = std::max(vmin.y, y);
        result.y = std::min(vmax.y, result.y);
        result.z = std::max(vmin.z, z);
        result.z = std::min(vmax.z, result.z);
    }

    // Static functions
    static float Distance(const ws_Vector3& v1, const ws_Vector3& v2)
    {
        auto diff = v2 - v1;
        return diff.Length();
    }

    static float DistanceSquared(const ws_Vector3& v1, const ws_Vector3& v2)
    {
        auto diff = v2 - v1;
        return diff.LengthSquared();
    }

    static void Min(const ws_Vector3& v1, const ws_Vector3& v2, ws_Vector3& result)
    {
        result.x = std::min(v1.x, v2.x);
        result.y = std::min(v1.y, v2.y);
        result.z = std::min(v1.z, v2.z);
    }

    static ws_Vector3 Min(const ws_Vector3& v1, const ws_Vector3& v2)
    {
        ws_Vector3 result;
        result.x = std::min(v1.x, v2.x);
        result.y = std::min(v1.y, v2.y);
        result.z = std::min(v1.z, v2.z);
        return result;
    }

    static void Max(const ws_Vector3& v1, const ws_Vector3& v2, ws_Vector3& result)
    {
        result.x = std::max(v1.x, v2.x);
        result.y = std::max(v1.y, v2.y);
        result.z = std::max(v1.z, v2.z);
    }
    static ws_Vector3 Max(const ws_Vector3& v1, const ws_Vector3& v2)
    {
        ws_Vector3 result;
        result.x = std::max(v1.x, v2.x);
        result.y = std::max(v1.y, v2.y);
        result.z = std::max(v1.z, v2.z);
        return result;
    }

    static void Lerp(const ws_Vector3& v1, const ws_Vector3& v2, float t, ws_Vector3& result)
    {
        result = v1 + (v2 - v1) * t;
    }
    static ws_Vector3 Lerp(const ws_Vector3& v1, const ws_Vector3& v2, float t)
    {
        return v1 + (v2 - v1) * t;
    }

    static void SmoothStep(const ws_Vector3& v1, const ws_Vector3& v2, float t, ws_Vector3& result)
    {
        t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t*t*(3.f - 2.f*t);
        result = v1 + (v2 - v1) * t;
    }
    static ws_Vector3 SmoothStep(const ws_Vector3& v1, const ws_Vector3& v2, float t)
    {
        t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t*t*(3.f - 2.f*t);
        return v1 + (v2 - v1) * t;
    }

    static void Barycentric(const ws_Vector3& v1, const ws_Vector3& v2, const ws_Vector3& v3, float f, float g, ws_Vector3& result)
    {
        auto p10 = v2 - v1;
        auto p20 = v3 - v1;
        result = v1 + p10 * f;
        result = result + p20 * g;
    }
    static ws_Vector3 Barycentric(const ws_Vector3& v1, const ws_Vector3& v2, const ws_Vector3& v3, float f, float g)
    {
        ws_Vector3 result;
        auto p10 = v2 - v1;
        auto p20 = v3 - v1;
        result = v1 + p10 * f;
        result = result + p20 * g;
        return result;
    }

    static void CatmullRom(const ws_Vector3& v1, const ws_Vector3& v2, const ws_Vector3& v3, const ws_Vector3& v4, float t, ws_Vector3& result)
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
    static ws_Vector3 CatmullRom(const ws_Vector3& v1, const ws_Vector3& v2, const ws_Vector3& v3, const ws_Vector3& v4, float t)
    {
        ws_Vector3 result;
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

    static void Hermite(const ws_Vector3& Position0, const ws_Vector3& Tangent0, const ws_Vector3& Position1, const ws_Vector3& Tangent1, float t, ws_Vector3& result)
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
    static ws_Vector3 Hermite(const ws_Vector3& Position0, const ws_Vector3& Tangent0, const ws_Vector3& Position1, const ws_Vector3& Tangent1, float t)
    {
        ws_Vector3 result;
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

    static void Reflect(const ws_Vector3& Incident, const ws_Vector3& Normal, ws_Vector3& result)
    {
        auto dot = Incident.Dot(Normal);
        dot *= 2.0f;
        result = Incident - Normal * dot;
    }
    static ws_Vector3 Reflect(const ws_Vector3& Incident, const ws_Vector3& Normal)
    {
        auto dot = Incident.Dot(Normal);
        dot *= 2.0f;
        return+Incident - Normal * dot;
    }

    static void Refract(const ws_Vector3& Incident, const ws_Vector3& Normal, float RefractionIndex, ws_Vector3& result)
    {
        float IDotN = Incident.Dot(Normal);

        auto R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN);

        if (R <= 0.0f)
        {
            // Total internal reflection
            result = ws_Vector3::Zero;
        }
        else
        {
            R = RefractionIndex * IDotN + std::sqrt(R);
            result = RefractionIndex * Incident - Normal * R;
        }
    }
    static ws_Vector3 Refract(const ws_Vector3& Incident, const ws_Vector3& Normal, float RefractionIndex)
    {
        ws_Vector3 result;
        float IDotN = Incident.Dot(Normal);

        auto R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN);

        if (R <= 0.0f)
        {
            // Total internal reflection
            result = ws_Vector3::Zero;
        }
        else
        {
            R = RefractionIndex * IDotN + std::sqrt(R);
            result = RefractionIndex * Incident - Normal * R;
        }
        return std::move(result);
    }

    static void Transform(const ws_Vector3& v, const ws_Matrix& m, ws_Vector3& result);
    static ws_Vector3 Transform(const ws_Vector3& v, const ws_Matrix& m);
    static void TransformNormal(const ws_Vector3& v, const ws_Matrix& m, ws_Vector3& result);
    static ws_Vector3 TransformNormal(const ws_Vector3& v, const ws_Matrix& m);

    // Constants
    static const ws_Vector3 Zero;
    static const ws_Vector3 One;
    static const ws_Vector3 UnitX;
    static const ws_Vector3 UnitY;
    static const ws_Vector3 UnitZ;
    static const ws_Vector3 Up;
    static const ws_Vector3 Down;
    static const ws_Vector3 Right;
    static const ws_Vector3 Left;
    static const ws_Vector3 Forward;
    static const ws_Vector3 Backward;
};

// Binary operators
inline ws_Vector3 operator+(const ws_Vector3& V1, const ws_Vector3& V2)
{
    return ws_Vector3(
        V1.x + V2.x,
        V1.y + V2.y,
        V1.z + V2.z);
}

inline ws_Vector3 operator-(const ws_Vector3& V1, const ws_Vector3& V2)
{
    return ws_Vector3(
        V1.x - V2.x,
        V1.y - V2.y,
        V1.z - V2.z);
}

inline ws_Vector3 operator*(const ws_Vector3& V1, const ws_Vector3& V2)
{
    return ws_Vector3(
        V1.x * V2.x,
        V1.y * V2.y,
        V1.z * V2.z);
}

inline ws_Vector3 operator*(const ws_Vector3& V, float S)
{
    return ws_Vector3(
        V.x * S,
        V.y * S,
        V.z * S);
}

inline ws_Vector3 operator/(const ws_Vector3& V1, const ws_Vector3& V2)
{
    return ws_Vector3(
        V1.x / V2.x,
        V1.y / V2.y,
        V1.z / V2.z);
}

inline ws_Vector3 operator*(float S, const ws_Vector3& V)
{
    return ws_Vector3(
        V.x * S,
        V.y * S,
        V.z * S);
}

#endif
