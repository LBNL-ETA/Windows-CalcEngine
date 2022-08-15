#include <cmath>

#include "SpectrumFunctions.hpp"
#include "WCECommon.hpp"

FenestrationCommon::CSeries
  SpectralAveraging::UVAction(const std::vector<double> & t_data, double a, double b)
{
    FenestrationCommon::CSeries result;
    for(auto & val : t_data)
    {
        double value = std::exp(a - b * val);
        result.addProperty(val, value);
    }

    return result;
}

FenestrationCommon::CSeries SpectralAveraging::Krochmann(const std::vector<double> & t_data)
{
    return UVAction(t_data, 12.28, 25.56);
}

FenestrationCommon::CSeries SpectralAveraging::BlackBodySpectrum(const std::vector<double> & t_data,
                                                                 double t_temperature)
{
    FenestrationCommon::CSeries result;

    const double ev = 1.602e-19;   // J

    const double k = 8.61739e-5;           // eV/K
    const double h = 4.135669e-15;         // eV s
    const double hc = 1239.842;            // eVnm , in nm because we are dividing with lambda
    const double kT = k * t_temperature;   // eV

    for(auto & val : t_data)
    {
        double lambda = val * 1e3;   // to convert it to nanometers
        double C1 = 8e-9 * ConstantsData::WCE_PI * h * ev * ConstantsData::SPEEDOFLIGHT
                    / std::pow(lambda * 1e-9, 5);
        double q1 = 1 / std::expm1(hc / kT / lambda);
        double energy = C1 * q1;
        result.addProperty(val, energy);
    }

    return result;
}
