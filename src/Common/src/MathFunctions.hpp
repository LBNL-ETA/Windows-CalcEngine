#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <cmath>

namespace FenestrationCommon
{
    static const double PI = 4.0 * std::atan(1.0);

    inline double radians(const double d)
    {
        return d * PI / 180;
    }

    inline double degrees(const double r)
    {
        return r * 180 / PI;
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
    		const auto RTMax = std::fmax(T, R);
    		if(RTMax == R)
            {
    		    tr.R = 1 - T;
            }
            if(RTMax == T)
            {
                tr.T = 1 - R;
            }

		}
		return tr;
	}

}   // namespace FenestrationCommon

#endif
