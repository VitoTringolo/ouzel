// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_PLANE_HPP
#define OUZEL_MATH_PLANE_HPP

#include <cmath>
#include <cstddef>
#include "MathUtils.hpp"

namespace ouzel
{
    template<class T> class Vector4;

    template<class T> class Plane final
    {
    public:
        T v[4]{0, 0, 0, 0};

        Plane()
        {
        }

        Plane(T a, T b, T c, T d):
            v{a, b, c, d}
        {
        }

        Plane(const Plane& copy):
            v{copy.v[0], copy.v[1], copy.v[2], copy.v[3]}
        {
        }

        Plane& operator=(const Plane& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = vec.v[2];
            v[3] = vec.v[3];
            return *this;
        }

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& a() { return v[0]; }
        inline T a() const { return v[0]; }

        inline T& b() { return v[1]; }
        inline T b() const { return v[1]; }

        inline T& c() { return v[2]; }
        inline T c() const { return v[2]; }

        inline T& d() { return v[3]; }
        inline T d() const { return v[3]; }

        void flip()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        T dot(const Vector4<T>& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3];
        }

        void normalize()
        {
            float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == 1.0F) // already normalized
                return;

            n = sqrtf(n);
            if (n < std::numeric_limits<float>::min()) // too close to zero
                return;

            n = 1.0F / n;
            v[0] *= n;
            v[1] *= n;
            v[2] *= n;
            v[3] *= n;
        }

        inline bool operator==(const Plane& plane) const
        {
            return v[0] == plane.v[0] && v[1] == plane.v[1] && v[2] == plane.v[2] && v[3] == plane.v[3];
        }

        inline bool operator!=(const Plane& plane) const
        {
            return v[0] != plane.v[0] || v[1] != plane.v[1] || v[2] != plane.v[2] || v[3] != plane.v[3];
        }

        static inline Plane makeFrustumPlane(float a, float b, float c, float d)
        {
            float n = sqrtf(a * a + b * b + c * c);
            if (n < std::numeric_limits<float>::min()) // too close to zero
                return Plane();

            n = 1.0F / n;
            return Plane(a * n, b * n, c * n, d * n);
        }
    };
}

#endif // OUZEL_MATH_PLANE_HPP
