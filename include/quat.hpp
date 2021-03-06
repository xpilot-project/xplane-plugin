#ifndef Quat_h
#define Quat_h

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#define PI (3.14159265358979)
#define RAD_TO_DEG (180.0 / PI)
#define DEG_TO_RAD (1.0 / RAD_TO_DEG)

struct Quaternion
{
    Quaternion() :x(0), y(0), z(0), w(1)
    {
    };

    Quaternion(double x, double y, double z, double w) :x(x), y(y), z(z), w(w)
    {
    };

    void normalize()
    {
        double norm = std::sqrt(x * x + y * y + z * z + w * w);
        x /= norm;
        y /= norm;
        z /= norm;
        w /= norm;
    }

    double norm()
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    double x;
    double y;
    double z;
    double w;
};

const Quaternion identity{ 1,0,0,0 };

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

void threeaxisrot(double r11, double r12, double r21, double r31, double r32, double res[])
{
    res[0] = atan2(r31, r32);
    res[1] = asin(r21);
    res[2] = atan2(r11, r12);
}

float dot(const Quaternion& left, const Quaternion& right)
{
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z) + (left.w * right.w);
}

Quaternion euler2quat(double yaw, double pitch, double roll)
{
    float halfRoll = roll * DEG_TO_RAD * 0.5f;
    float halfPitch = pitch * DEG_TO_RAD * 0.5f;
    float halfYaw = yaw * DEG_TO_RAD * 0.5f;

    float sinRoll = (float)sin(halfRoll);
    float cosRoll = (float)cos(halfRoll);
    float sinPitch = (float)sin(halfPitch);
    float cosPitch = (float)cos(halfPitch);
    float sinYaw = (float)sin(halfYaw);
    float cosYaw = (float)cos(halfYaw);

    Quaternion result{};

    result.x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
    result.y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
    result.z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
    result.w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);

    //double local_Heading = yaw * DEG_TO_RAD;
    //double local_Pitch = pitch * DEG_TO_RAD;
    //double local_Roll = roll * DEG_TO_RAD;

    //double Cosine1 = cos(local_Roll / 2);
    //double Cosine2 = cos(local_Pitch / 2);
    //double Cosine3 = cos(local_Heading / 2);
    //double Sine1 = sin(local_Roll / 2);
    //double Sine2 = sin(local_Pitch / 2);
    //double Sine3 = sin(local_Heading / 2);

    //Quaternion q{};
    //q.w = float(Cosine1 * Cosine2 * Cosine3 + Sine1 * Sine2 * Sine3);
    //q.x = float(Sine1 * Cosine2 * Cosine3 - Cosine1 * Sine2 * Sine3);
    //q.y = float(Cosine1 * Sine2 * Cosine3 + Sine1 * Cosine2 * Sine3);
    //q.z = float(Cosine1 * Cosine2 * Sine3 - Sine1 * Sine2 * Cosine3);

    return result;

    //double cy = cos(yaw * 0.5);
    //double sy = sin(yaw * 0.5);
    //double cp = cos(pitch * 0.5);
    //double sp = sin(pitch * 0.5);
    //double cr = cos(roll * 0.5);
    //double sr = sin(roll * 0.5);

    //Quaternion q;
    //q.w = cr * cp * cy + sr * sp * sy;
    //q.x = sr * cp * cy - cr * sp * sy;
    //q.y = cr * sp * cy + sr * cp * sy;
    //q.z = cr * cp * sy - sr * sp * cy;

    //return q;
}

void quat2euler(const Quaternion& q, double* yaw, double* pitch, double* roll)
{
    double local_w = q.w;
    double local_x = q.x;
    double local_y = q.y;
    double local_z = q.z;

    double sq_w = local_w * local_w;
    double sq_x = local_x * local_x;
    double sq_y = local_y * local_y;
    double sq_z = local_z * local_z;

    *yaw = float(atan2(2.0 * (local_x * local_y + local_z * local_w), (sq_x - sq_y - sq_z + sq_w)) * RAD_TO_DEG);
    *pitch = float(asin(-2.0 * (local_x * local_z - local_y * local_w)) * RAD_TO_DEG);
    *roll = float(atan2(2.0 * (local_y * local_z + local_x * local_w), (-sq_x - sq_y + sq_z + sq_w)) * RAD_TO_DEG);
}

Quaternion Slerp(const Quaternion& start, const Quaternion& end, double amount)
{
    float opposite;
    float inverse;
    float mdot = dot(start, end);

    if (std::abs(mdot) > 1.0f - 0.0000001)
    {
        inverse = 1.0f - amount;
        opposite = amount * sgn(mdot);
    }
    else
    {
        float macos = (float)acos(abs(mdot));
        float invSin = (float)(1.0 / sin(macos));

        inverse = (float)sin((1.0f - amount) * macos) * invSin;
        opposite = (float)sin(amount * macos) * invSin * sgn(dot);
    }

    Quaternion result{};

    result.x = (inverse * start.x) + (opposite * end.x);
    result.y = (inverse * start.y) + (opposite * end.y);
    result.z = (inverse * start.z) + (opposite * end.z);
    result.w = (inverse * start.w) + (opposite * end.w);

    return result;
}

Quaternion operator*(Quaternion& q1, Quaternion& q2)
{
    Quaternion q;
    q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    q.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    q.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return q;
}

double rad2deg(double rad)
{
    return rad * 180.0 / M_PI;
}

//const double EPS = 0.000001;
//const double degtorad = 0.0174533;
//const double radtodeg = 57.2958;
//
//typedef struct quat_t
//{
//    double w, x, y, z;
//} quat, quaternion;
//
//quat CreateFromYawPitchRoll(double yaw, double pitch, double roll);
//void ExtractEulerAngles(const quat& q, double*pitch, double* heading, double* bank);
//double Dot(const quat& lhs, const quat& rhs);
//quat Slerp(const quat& start, const quat& end, double amount);
//quat QuatToXP(const quat& q);
//quat operator*(const quat& left, const quat& right);
//const quat identity{ 1,0,0,0 };
//
//quat CreateFromYawPitchRoll(double yaw, double pitch, double roll)
//{
//    double psi = yaw * degtorad;
//    double theta = pitch * degtorad;
//    double phi = roll * degtorad;
//
//    double sin_psi = sin(psi / 2);
//    double cos_psi = cos(psi / 2);
//    double sin_theta = sin(theta / 2);
//    double cos_theta = cos(theta / 2);
//    double sin_phi = sin(phi / 2);
//    double cos_phi = cos(phi / 2);
//
//    quat q{};
//    q.w = cos_psi * cos_theta * cos_phi + sin_psi * sin_theta * sin_phi;
//    q.x = cos_psi * cos_theta * sin_phi - sin_psi * sin_theta * cos_phi;
//    q.y = cos_psi * sin_theta * cos_phi + sin_psi * cos_theta * sin_phi;
//    q.z = -cos_psi * sin_theta * sin_phi + sin_psi * cos_theta * cos_phi;
//
//    return q;
//}
//
//double clamp(double n, double min, double max)
//{
//    return std::max(min, std::min(n, max));
//}
//
//double normalize(double v, double lower, double upper)
//{
//    double range = upper - lower;
//    if (v < lower)
//    {
//        return v + range;
//    }
//    else if (v > upper)
//    {
//        return v - range;
//    }
//    return v;
//}
//
//void ExtractEulerAngles(const quat& q, double* yaw, double* pitch, double* roll)
//{
//    double sq_w = q.w * q.w;
//    double sq_x = q.x * q.x;
//    double sq_y = q.y * q.y;
//    double sq_z = q.z * q.z;
//
//    *yaw = atan2(2.0 * (q.x * q.y + q.z * q.w), (sq_x - sq_y - sq_z + sq_w)) * radtodeg;
//    *pitch = asin(-2.0 * (q.x * q.z - q.y * q.w)) * radtodeg;
//    *roll = atan2(2.0 * (q.y * q.z + q.x * q.w), (-sq_x - sq_y + sq_z + sq_w)) * radtodeg;
//}
//
//double Dot(const quat& q1, const quat& q2)
//{
//    return (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);
//}
//
//quat Slerp(const quat& qa, const quat& qb, double t)
//{
//    quat qm{};
//
//    // Calculate angle between them.
//    double cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
//    // if qa=qb or qa=-qb then theta = 0 and we can return qa
//    if (abs(cosHalfTheta) >= 1.0)
//    {
//        qm.w = qa.w; qm.x = qa.x; qm.y = qa.y; qm.z = qa.z;
//        return qm;
//    }
//    // Calculate temporary values.
//    double halfTheta = acos(cosHalfTheta);
//    double sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);
//    // if theta = 180 degrees then result is not fully defined
//    // we could rotate around any axis normal to qa or qb
//    if (fabs(sinHalfTheta) < 0.001)
//    { // fabs is floating point absolute
//        qm.w = (qa.w * 0.5 + qb.w * 0.5);
//        qm.x = (qa.x * 0.5 + qb.x * 0.5);
//        qm.y = (qa.y * 0.5 + qb.y * 0.5);
//        qm.z = (qa.z * 0.5 + qb.z * 0.5);
//        return qm;
//    }
//    double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
//    double ratioB = sin(t * halfTheta) / sinHalfTheta;
//    //calculate Quaternion.
//    qm.w = (qa.w * ratioA + qb.w * ratioB);
//    qm.x = (qa.x * ratioA + qb.x * ratioB);
//    qm.y = (qa.y * ratioA + qb.y * ratioB);
//    qm.z = (qa.z * ratioA + qb.z * ratioB);
//    return qm;
//}
//
//quat operator*(const quat& q1, const quat& q2)
//{
//    quat r{};
//    r.x = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
//    r.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
//    r.z = q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
//    r.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
//    //r.x = q1.x * q2.x - q1.y * q2.y - q1.z * q2.z - q1.w * q2.w;
//    //r.y = q1.x * q2.y + q1.y * q2.x + q1.z * q2.w - q1.w * q2.z;
//    //r.z = q1.x * q2.z - q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
//    //r.w = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
//    return r;
//}
//
//quat QuatToXP(const quat& q)
//{
//    quat r{ q.w,q.x,q.y,q.z };
//    return r;
//}

#endif // !Quat_h

