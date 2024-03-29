#pragma once

#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>

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

    //! Test if two values are withing float tolerance defined in FenestrationCommon constants
    inline bool isEqual(const double val1, const double val2)
    {
        return std::abs(val1 - val2) < ConstantsData::floatErrorTolerance;
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
            // Brackets around std::maximum are necessary because this fails when included in MFC files
            // that uses Windows.h
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

    template <typename T>
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

        std::transform(
          v1.begin(), v1.end(), v2.begin(), result.begin(), std::multiplies<T>());

        return result;
    }
}   // namespace FenestrationCommon
