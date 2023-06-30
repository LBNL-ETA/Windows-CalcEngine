#include "SurfaceProperties.hpp"

namespace Tarcog::ISO15099
{
    Tarcog::ISO15099::ConstantSurfaceProperties::ConstantSurfaceProperties(double t_Emissivity,
                                                                           double t_Transmittance) :
        m_Emissivity(t_Emissivity), m_Transmittance(t_Transmittance)
    {}

    double ConstantSurfaceProperties::emissivity(double) const
    {
        return m_Emissivity;
    }

    double ConstantSurfaceProperties::transmittance(double) const
    {
        return m_Transmittance;
    }

    std::unique_ptr<ISurfaceProperties> ConstantSurfaceProperties::clone() const
    {
        return std::make_unique<ConstantSurfaceProperties>(*this);
    }

    ThermochromicSurfaceProperties::ThermochromicSurfaceProperties(
      const std::vector<FenestrationCommon::TableValue> & t_Emissivity,
      const std::vector<FenestrationCommon::TableValue> & t_Transmittance) :
        m_EmissivityInterpolator(t_Emissivity), m_TransmittanceInterpolator(t_Transmittance)
    {}

    double ThermochromicSurfaceProperties::transmittance(double t_Temperature) const
    {
        return m_TransmittanceInterpolator.value(t_Temperature);
    }

    double ThermochromicSurfaceProperties::emissivity(double t_Temperature) const
    {
        return m_EmissivityInterpolator.value(t_Temperature);
    }

    std::unique_ptr<ISurfaceProperties> ThermochromicSurfaceProperties::clone() const
    {
        return std::make_unique<ThermochromicSurfaceProperties>(*this);
    }
}   // namespace Tarcog::ISO15099
