#pragma once

#include "WCECommon.hpp"
#include <vector>

namespace SpectralAveraging
{
    /// Input wavelengths are in micrometers
    FenestrationCommon::CSeries
      UVAction(const std::vector<double> & t_data, double a = 3.6, double b = 12.0);

    /// Input wavelengths are in micrometers
    FenestrationCommon::CSeries Krochmann(const std::vector<double> & t_data);

    /// Input wavelengths are in micrometers
    FenestrationCommon::CSeries BlackBodySpectrum(const std::vector<double> & t_data,
                                                  double t_temperature);
}   // namespace SpectralAveraging