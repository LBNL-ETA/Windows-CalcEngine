#pragma once

#include <memory>
#include <vector>

#include "SurfaceProperties.hpp"

namespace Tarcog::ISO15099
{

    class Surface
    {
    public:
        Surface();
        Surface(double t_Emissivity, double t_Transmittance);
        Surface(const std::vector<FenestrationCommon::TableValue> & t_Emissivity,
                const std::vector<FenestrationCommon::TableValue> & t_Transmittance);

        Surface(Surface const & t_Surface);
        Surface & operator=(Surface const & t_Surface);

        [[nodiscard]] std::shared_ptr<Surface> clone() const;

        void setTemperature(double t_Temperature);
        void setJ(double t_J);

        void applyDeflection(double t_MeanDeflection, double t_MaxDeflection);
        [[nodiscard]] double getTemperature() const;
        [[nodiscard]] double getEmissivity() const;
        [[nodiscard]] double getReflectance() const;
        [[nodiscard]] double getTransmittance() const;
        [[nodiscard]] double J() const;
        [[nodiscard]] double emissivePowerTerm() const;
        [[nodiscard]] double getMeanDeflection() const;
        [[nodiscard]] double getMaxDeflection() const;

        void initializeStart(double t_Temperature);
        void initializeStart(double t_Temperature, double t_Radiation);

    protected:
        void calculateReflectance();

        double m_Temperature{273.15};
        double m_J{0.0};

        double m_Emissivity{0.84};
        double m_Reflectance{0.0};
        double m_Transmittance{0.0};

        // Value for deflection. Positive deflection is surface curved towards left side and
        // negative deflection vice-versa.
        double m_MeanDeflection{0.0};
        double m_MaxDeflection{0.0};

        std::unique_ptr<ISurfaceProperties> m_SurfaceProperties;
    };
}   // namespace Tarcog::ISO15099
