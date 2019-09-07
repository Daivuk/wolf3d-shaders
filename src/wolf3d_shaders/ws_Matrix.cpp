#include <ws_Matrix.h>
#include <ws_Vector2.h>
#include <ws_Vector3.h>
#include <ws_Vector4.h>

const ws_Matrix ws_Matrix::Identity(1, 0, 0, 0, 
                              0, 1, 0, 0, 
                              0, 0, 1, 0, 
                              0, 0, 0, 1);

ws_Matrix::ws_Matrix(const ws_Vector3& r0, const ws_Vector3& r1, const ws_Vector3& r2)
    : _11(r0.x), _12(r0.y), _13(r0.z), _14(0.0f)
    , _21(r1.x), _22(r1.y), _23(r1.z), _24(0.0f)
    , _31(r2.x), _32(r2.y), _33(r2.z), _34(0.0f)
    , _41(0.0f), _42(0.0f), _43(0.0f), _44(1.0f)
{
}

ws_Matrix::ws_Matrix(const ws_Vector4& r0, const ws_Vector4& r1, const ws_Vector4& r2, const ws_Vector4& r3)
    : _11(r0.x), _12(r0.y), _13(r0.z), _14(r0.w)
    , _21(r1.x), _22(r1.y), _23(r1.z), _24(r1.w)
    , _31(r2.x), _32(r2.y), _33(r2.z), _34(r2.w)
    , _41(r3.x), _42(r3.y), _43(r3.z), _44(r3.w)
{
}

ws_Vector3 ws_Matrix::Up() const { return ws_Vector3(_21, _22, _23); }
void ws_Matrix::Up(const ws_Vector3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }

ws_Vector3 ws_Matrix::Down() const { return ws_Vector3(-_21, -_22, -_23); }
void ws_Matrix::Down(const ws_Vector3& v) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

ws_Vector3 ws_Matrix::Right() const { return ws_Vector3(_11, _12, _13); }
void ws_Matrix::Right(const ws_Vector3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }

ws_Vector3 ws_Matrix::Left() const { return ws_Vector3(-_11, -_12, -_13); }
void ws_Matrix::Left(const ws_Vector3& v) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

ws_Vector3 ws_Matrix::Forward() const { return ws_Vector3(-_31, -_32, -_33); }
void ws_Matrix::Forward(const ws_Vector3& v) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

ws_Vector3 ws_Matrix::Backward() const { return ws_Vector3(_31, _32, _33); }
void ws_Matrix::Backward(const ws_Vector3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }

ws_Vector3 ws_Matrix::AxisY() const { return ws_Vector3(_21, _22, _23); }
void ws_Matrix::AxisY(const ws_Vector3& v) { _21 = v.x; _22 = v.y; _23 = v.z; }

ws_Vector3 ws_Matrix::AxisX() const { return ws_Vector3(_11, _12, _13); }
void ws_Matrix::AxisX(const ws_Vector3& v) { _11 = v.x; _12 = v.y; _13 = v.z; }

ws_Vector3 ws_Matrix::AxisZ() const { return ws_Vector3(_31, _32, _33); }
void ws_Matrix::AxisZ(const ws_Vector3& v) { _31 = v.x; _32 = v.y; _33 = v.z; }

ws_Vector3 ws_Matrix::Translation() const { return ws_Vector3(_41, _42, _43); }
void ws_Matrix::Translation(const ws_Vector3& v) { _41 = v.x; _42 = v.y; _43 = v.z; }

ws_Matrix ws_Matrix::CreateBillboard(const ws_Vector3& object, const ws_Vector3& cameraPosition, const ws_Vector3& cameraUp, const ws_Vector3* cameraForward)
{
    auto Z = cameraPosition - object;
    static const float EPSILON = 1.192092896e-7f;

    auto N = Z.LengthSquared();
    if (N < EPSILON)
    {
        if (cameraForward)
        {
            Z = -(*cameraForward);
        }
        else
            Z = ws_Vector3(0, 0, -1);
    }
    else
    {
        Z.Normalize();
    }

    auto X = cameraUp.Cross(Z);
    X.Normalize();

    auto Y = Z.Cross(X);

    ws_Matrix M(X, Y, Z);
    M._41 = object.x;
    M._42 = object.y;
    M._43 = object.z;
    M._44 = 1.f;
    return M;
}

ws_Matrix ws_Matrix::CreateConstrainedBillboard(const ws_Vector3& object, const ws_Vector3& cameraPosition, const ws_Vector3& rotateAxis, const ws_Vector3* cameraForward, const ws_Vector3* objectForward)
{
    static const float s_minAngle = 0.99825467075f;
    static const float EPSILON = 1.192092896e-7f;

    auto faceDir = object - cameraPosition;

    auto N = faceDir.LengthSquared();
    if (N < EPSILON)
    {
        if (cameraForward)
        {
            faceDir = -(*cameraForward);
        }
        else
            faceDir = ws_Vector3(0, 0, -1);
    }
    else
    {
        faceDir.Normalize();
    }

    auto dot = std::fabs(rotateAxis.Dot(faceDir));
    ws_Vector3 X, Z;
    if (dot > s_minAngle)
    {
        if (objectForward)
        {
            Z = *objectForward;
            dot = std::fabs(rotateAxis.Dot(Z));
            if (dot > s_minAngle)
            {
                dot = std::fabs(rotateAxis.Dot(ws_Vector3(0, 0, -1)));
                Z = (dot > s_minAngle) ? ws_Vector3(1, 0, 0) : ws_Vector3(0, 0, -1);
            }
        }
        else
        {
            dot = std::fabs(rotateAxis.Dot(ws_Vector3(0, 0, -1)));
            Z = (dot > s_minAngle) ? ws_Vector3(1, 0, 0) : ws_Vector3(0, 0, -1);
        }

        X = rotateAxis.Cross(Z);
        X.Normalize();

        Z = X.Cross(rotateAxis);
        Z.Normalize();
    }
    else
    {
        X = rotateAxis.Cross(faceDir);
        X.Normalize();

        Z = X.Cross(rotateAxis);
        Z.Normalize();
    }

    ws_Matrix M(X, rotateAxis, Z);
    M._41 = object.x;
    M._42 = object.y;
    M._43 = object.z;
    M._44 = 1.0f;
    return std::move(M);
}

ws_Matrix ws_Matrix::CreateTranslation(const ws_Vector3& position)
{
    return ws_Matrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        position.x, position.y, position.z, 1);
}

ws_Matrix ws_Matrix::CreateScale(const ws_Vector3& scales)
{
    return ws_Matrix(
        scales.x, 0, 0, 0,
        0, scales.y, 0, 0,
        0, 0, scales.z, 0,
        0, 0, 0, 1);
}

ws_Matrix ws_Matrix::Create2DTranslationZoom(const ws_Vector2& resolution, const ws_Vector2& camera, float zoom)
{
    return ws_Matrix(
        zoom, 0, 0, 0,
        0, zoom, 0, 0,
        0, 0, zoom, 0,
        -camera.x * zoom + resolution.x * 0.5f, -camera.y * zoom + resolution.y * 0.5f, 0, 1);
}

ws_Matrix ws_Matrix::CreateFromAxisAngle(const ws_Vector3& axis, float angle)
{
    float cosTheta = std::cos(angle);
    float sinTheta = std::sin(angle);
    float invCosTheta = 1 - cosTheta;

    return ws_Matrix(
        cosTheta + axis.x * axis.x * invCosTheta, axis.x * axis.y * invCosTheta - axis.z * sinTheta, axis.x * axis.z * invCosTheta + axis.y * sinTheta, 0,
        axis.y * axis.x * invCosTheta + axis.z * sinTheta, cosTheta + axis.y * axis.y * invCosTheta, axis.y * axis.z * invCosTheta - axis.x * sinTheta, 0,
        axis.z * axis.x * invCosTheta - axis.y * sinTheta, axis.z * axis.y * invCosTheta + axis.x * sinTheta, cosTheta + axis.z * axis.z * invCosTheta, 0,
        0, 0, 0, 1);
}

ws_Matrix ws_Matrix::CreateLookAt(const ws_Vector3& position, const ws_Vector3& target, const ws_Vector3& up)
{
    auto EyeDirection = position - target;
    auto R2 = EyeDirection;
    R2.Normalize();

    auto R0 = up.Cross(R2);
    R0.Normalize();

    auto R1 = R2.Cross(R0);

    auto NegEyePosition = -position;

    auto D0 = R0.Dot(NegEyePosition);
    auto D1 = R1.Dot(NegEyePosition);
    auto D2 = R2.Dot(NegEyePosition);

    return ws_Matrix(
        R0.x, R1.x, R2.x, 0,
        R0.y, R1.y, R2.y, 0,
        R0.z, R1.z, R2.z, 0,
        D0, D1, D2, 1);
}

ws_Matrix ws_Matrix::CreateWorld(const ws_Vector3& position, const ws_Vector3& forward, const ws_Vector3& up)
{
    auto yaxis = forward;
    yaxis.Normalize();
    auto zaxis = up;
    auto xaxis = yaxis.Cross(zaxis);
    xaxis.Normalize();
    yaxis = zaxis.Cross(xaxis);

    return ws_Matrix(
        xaxis.x, xaxis.y, xaxis.z, 0,
        yaxis.x, yaxis.y, yaxis.z, 0,
        zaxis.x, zaxis.y, zaxis.z, 0,
        position.x, position.y, position.z, 1);
}
