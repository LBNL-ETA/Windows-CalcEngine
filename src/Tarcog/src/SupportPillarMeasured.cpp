#include <WCECommon.hpp>

#include "SupportPillarMeasured.hpp"

namespace Tarcog::ISO15099
{
    double surface2Temperature(const PillarMeasurement & pm)
    {
        return pm.glass1.thickness / pm.glass1.conductivity * pm.conductivity / pm.totalThickness
                 * (pm.temperatureSurface4 - pm.temperatureSurface1)
               + pm.temperatureSurface1;
    }

    double surface3Temperature(const PillarMeasurement & pm)
    {
        return (pm.temperatureSurface4
                - pm.glass2.thickness / pm.glass2.conductivity * pm.conductivity / pm.totalThickness
                    * (pm.temperatureSurface4 - pm.temperatureSurface1));
    }

    double cRadiative(const PillarMeasurement & pm)
    {
        using ConstantsData::STEFANBOLTZMANN;
        double t2 = surface2Temperature(pm);
        double t3 = surface3Temperature(pm);

        return STEFANBOLTZMANN / (1 / pm.glass1.emissivity + 1 / pm.glass2.emissivity - 1)
               * ((std::pow(t3, 4) - std::pow(t2, 4)) / (t3 - t2));
    }

    double rGap(const PillarMeasurement & pm)
    {
        return pm.totalThickness / pm.conductivity
               - (pm.glass1.thickness / pm.glass1.conductivity
                  + pm.glass2.thickness / pm.glass2.conductivity);
    }

    double cStar(const PillarMeasurement & pm)
    {
        return 1 / rGap(pm) - cRadiative(pm);
    }

    // Measured vacuum pillar includes the measured conductance of the gas and putting this to zero will
    // result in thermal conductivity of the gas to be zero.
    constexpr auto defaultVacuumPressure{0.0};

    MeasuredPillarLayer::MeasuredPillarLayer(const PillarMeasurement & pillar) :
        CIGUGapLayer(pillar.totalThickness - pillar.glass1.thickness - pillar.glass2.thickness, defaultVacuumPressure),
        m_MeasuredConductance(cStar(pillar))
    {}

    void MeasuredPillarLayer::calculateConvectionOrConductionFlow()
    {
        if(!isCalculated())
        {
            m_ConductiveConvectiveCoeff = m_MeasuredConductance;
            setCalculated();
        }
    }
}   // namespace Tarcog::ISO15099