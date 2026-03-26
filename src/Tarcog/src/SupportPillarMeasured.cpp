#include <WCECommon.hpp>

#include "SupportPillarMeasured.hpp"

namespace Tarcog::ISO15099
{
    double surface2Temperature(const PillarMeasurement & pmt)
    {
        return pmt.glass1.thickness / pmt.glass1.conductivity * pmt.conductivity / pmt.totalThickness
                 * (pmt.temperatureSurface4 - pmt.temperatureSurface1)
               + pmt.temperatureSurface1;
    }

    double surface3Temperature(const PillarMeasurement & pmt)
    {
        return (pmt.temperatureSurface4
                - pmt.glass2.thickness / pmt.glass2.conductivity * pmt.conductivity / pmt.totalThickness
                    * (pmt.temperatureSurface4 - pmt.temperatureSurface1));
    }

    double cRadiative(const PillarMeasurement & pmt)
    {
        using ConstantsData::STEFANBOLTZMANN;
        double tmp2 = surface2Temperature(pmt);
        double tmp3 = surface3Temperature(pmt);

        return STEFANBOLTZMANN / (1 / pmt.glass1.emissivity + 1 / pmt.glass2.emissivity - 1)
               * ((std::pow(tmp3, 4) - std::pow(tmp2, 4)) / (tmp3 - tmp2));
    }

    double rGap(const PillarMeasurement & pmt)
    {
        return pmt.totalThickness / pmt.conductivity
               - (pmt.glass1.thickness / pmt.glass1.conductivity
                  + pmt.glass2.thickness / pmt.glass2.conductivity);
    }

    double cStar(const PillarMeasurement & pmt)
    {
        return 1 / rGap(pmt) - cRadiative(pmt);
    }
}   // namespace Tarcog::ISO15099
