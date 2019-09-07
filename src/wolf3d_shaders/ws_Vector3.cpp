#include <ws_Matrix.h>
#include <ws_Vector2.h>
#include <ws_Vector3.h>

const ws_Vector3 ws_Vector3::Zero(0.f, 0.f, 0.f);
const ws_Vector3 ws_Vector3::One(1.f, 1.f, 1.f);
const ws_Vector3 ws_Vector3::UnitX(1.f, 0.f, 0.f);
const ws_Vector3 ws_Vector3::UnitY(0.f, 1.f, 0.f);
const ws_Vector3 ws_Vector3::UnitZ(0.f, 0.f, 1.f);
const ws_Vector3 ws_Vector3::Up(0.f, 0.f, 1.f);
const ws_Vector3 ws_Vector3::Down(0.f, 0.f, -1.f);
const ws_Vector3 ws_Vector3::Right(1.f, 0.f, 0.f);
const ws_Vector3 ws_Vector3::Left(-1.f, 0.f, 0.f);
const ws_Vector3 ws_Vector3::Forward(0.f, 1.f, 0.f);
const ws_Vector3 ws_Vector3::Backward(0.f, -1.f, 0.f);

ws_Vector3::ws_Vector3(const ws_Vector2& v2, float _z) 
    : x(v2.x), y(v2.y), z(_z)
{
}

ws_Vector3::ws_Vector3(const ws_Vector2& v2) 
    : x(v2.x), y(v2.y), z(0.f)
{
}

void ws_Vector3::Transform(const ws_Vector3& v, const ws_Matrix& m, ws_Vector3& result)
{
    float w = 0.0f;
    result.x = (m._11 * v.x) + (m._21 * v.y) + (m._31 * v.z) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + (m._32 * v.z) + m._42;
    result.z = (m._13 * v.x) + (m._23 * v.y) + (m._33 * v.z) + m._43;
    w = (m._14 * v.x) + (m._24 * v.y) + (m._34 * v.z) + m._44;
    result /= w;
}

ws_Vector3 ws_Vector3::Transform(const ws_Vector3& v, const ws_Matrix& m)
{
    ws_Vector3 result;
    float w = 0.0f;
    result.x = (m._11 * v.x) + (m._21 * v.y) + (m._31 * v.z) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + (m._32 * v.z) + m._42;
    result.z = (m._13 * v.x) + (m._23 * v.y) + (m._33 * v.z) + m._43;
    w = (m._14 * v.x) + (m._24 * v.y) + (m._34 * v.z) + m._44;
    result /= w;
    return std::move(result);
}

void ws_Vector3::TransformNormal(const ws_Vector3& v, const ws_Matrix& m, ws_Vector3& result)
{
    result.x = (m._11 * v.x) + (m._21 * v.y) + (m._31 * v.z) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + (m._32 * v.z) + m._42;
    result.z = (m._13 * v.x) + (m._23 * v.y) + (m._33 * v.z) + m._43;
}

ws_Vector3 ws_Vector3::TransformNormal(const ws_Vector3& v, const ws_Matrix& m)
{
    ws_Vector3 result;
    result.x = (m._11 * v.x) + (m._21 * v.y) + (m._31 * v.z) + m._41;
    result.y = (m._12 * v.x) + (m._22 * v.y) + (m._32 * v.z) + m._42;
    result.z = (m._13 * v.x) + (m._23 * v.y) + (m._33 * v.z) + m._43;
    return std::move(result);
}
