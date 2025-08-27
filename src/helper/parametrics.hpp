#pragma once

#include <algorithm>
#include <string>


namespace TestUtil
{
    struct AngleCase
    {
        double angle;
        SpectralAveraging::ScatteringType scatter;
        double Tf, Tb, Rf, Rb, Absf, Absb;
    };

    static std::string scatterName(SpectralAveraging::ScatteringType s)
    {
        switch(s)
        {
            case SpectralAveraging::ScatteringType::Direct:
                return "Direct";
            case SpectralAveraging::ScatteringType::Diffuse:
                return "Diffuse";
            case SpectralAveraging::ScatteringType::Total:
                return "Total";
            default:
                return "Unknown";
        }
    }

    // Pretty param names like Angle_10deg, Angle_82_5deg, etc.
    static auto angleParamNames = [](const ::testing::TestParamInfo<AngleCase> & info) {
        std::ostringstream os;
        std::string a = std::to_string(info.param.angle);
        std::ranges::replace(a, '.', '_');
        // trim trailing zeros and possible trailing '_'
        while(!a.empty() && (a.back() == '0'))
            a.pop_back();
        if(!a.empty() && a.back() == '.')
            a.pop_back();
        if(!a.empty() && a.back() == '_')
            a.pop_back();

        os << scatterName(info.param.scatter) << "_" << (a.empty() ? "0" : a) << "deg";
        return os.str();
    };
}   // namespace Helper