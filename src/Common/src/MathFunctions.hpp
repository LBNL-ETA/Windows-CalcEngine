#pragma once

#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "Constants.hpp"

namespace FenestrationCommon
{
    static const double WCE_PI = 4.0 * std::atan(1.0);

    inline double radians(const double d)
    {
        return d * WCE_PI / 180;
    }

    inline double degrees(const double r)
    {
        return r * 180 / WCE_PI;
    }

    template <typename T>
    inline typename std::enable_if<std::is_floating_point<T>::value, bool>::type
    isEqual(const T val1, const T val2, const T tolerance = static_cast<T>(ConstantsData::floatErrorTolerance))
    {
        return std::abs(val1 - val2) < tolerance;
    }

    struct TR
    {
        double T;
        double R;
    };

    inline TR checkRange(const double T, const double R)
    {
        TR tr{T, R};
        if(T + R > 1)
        {
            // Brackets around std::maximum are necessary because this fails when included in MFC
            // files that uses Windows.h
            const auto RTMax = (std::max)(T, R);
            if(isEqual(RTMax, R))
            {
                tr.R = 1 - T;
            }
            if(isEqual(RTMax, T))
            {
                tr.T = 1 - R;
            }
        }
        return tr;
    }

    //! Finds linear interpolation between the points for a given value
    inline double linearInterpolation(double x1, double x2, double y1, double y2, double x)
    {
        double delta{0};
        if(!isEqual(x1, x2))
        {
            delta = ((y2 - y1) / (x2 - x1)) * (x - x1);
        }
        return y1 + delta;
    }


    //! Calculates the area of a polygon with n equal sides.
    //! Here, 'radius' refers to the distance from the center of the polygon
    //! to one of its vertices. This interpretation aligns with the radius of
    //! the circumscribed circle of the polygon.
    //!
    //! Note: The formula used is specialized for polygons defined in this way
    //! and may differ from standard formulas for regular polygons.
    //! @param nSides The number of sides of the polygon.
    //! @param radius The radius of the circumscribed circle, measured from the
    //!               center of the polygon to a vertex.
    //! @return The area of the polygon calculated based on the radius.
    inline double nTagonArea(const size_t nSides, const double radius)
    {
        return 0.5 * nSides * std::pow(radius, 2) * std::sin(2 * WCE_PI / nSides);
    }


    template<typename T>
    std::string to_string_with_precision(const T a_value, const int n = 6)
    {
        std::ostringstream out;
        out.precision(n);
        out << std::fixed << a_value;
        return out.str();
    }

    //! Multiplies two vectors
    template<typename T>
    std::vector<T> mult(const std::vector<T> & v1, const std::vector<T> & v2)
    {
        if(v1.size() != v2.size())
        {
            throw std::runtime_error(
              "Vectors that are multiplied with each other are different size.");
        }

        std::vector<T> result(v1.size());

        std::transform(v1.begin(), v1.end(), v2.begin(), result.begin(), std::multiplies<T>());

        return result;
    }

}   // namespace FenestrationCommon
