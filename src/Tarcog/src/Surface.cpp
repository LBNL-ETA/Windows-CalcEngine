#include <cmath>
#include <stdexcept>

#include "BaseLayer.hpp"
#include "Surface.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        //////////////////////////////////////////////////////////////////////////////
        /// ISurface
        //////////////////////////////////////////////////////////////////////////////
        ISurface::ISurface() :
            m_SurfaceProperties(std::make_unique<ConstantSurfaceProperties>(0.84, 0))
        {
            calculateReflectance();
        }

        ISurface::ISurface(double const t_Emissivity, double const t_Transmittance) :
            m_Emissivity(t_Emissivity),
            m_Transmittance(t_Transmittance),
            m_SurfaceProperties(
              std::make_unique<ConstantSurfaceProperties>(m_Emissivity, m_Transmittance))
        {
            calculateReflectance();
        }

        ISurface::ISurface(ISurface const & t_Surface)
        {
            operator=(t_Surface);
        }

        ISurface & ISurface::operator=(ISurface const & t_Surface)
        {
            m_Emissivity = t_Surface.m_Emissivity;
            m_Transmittance = t_Surface.m_Transmittance;
            m_Temperature = t_Surface.m_Temperature;
            m_J = t_Surface.m_J;
            m_MaxDeflection = t_Surface.m_MaxDeflection;
            m_MeanDeflection = t_Surface.m_MeanDeflection;
            m_SurfaceProperties = t_Surface.m_SurfaceProperties->clone();
            calculateReflectance();

            return *this;
        }

        void ISurface::setTemperature(double const t_Temperature)
        {
            m_Temperature = t_Temperature;
        }

        void ISurface::setJ(double const t_J)
        {
            m_J = t_J;
        }

        void ISurface::applyDeflection(double const t_MeanDeflection, double const t_MaxDeflection)
        {
            m_MeanDeflection = t_MeanDeflection;
            m_MaxDeflection = t_MaxDeflection;
        }

        double ISurface::getTemperature() const
        {
            return m_Temperature;
        }

        double ISurface::getEmissivity() const
        {
            return m_SurfaceProperties->emissivity(m_Temperature);
        }

        double ISurface::getReflectance() const
        {
            return m_Reflectance;
        }

        double ISurface::getTransmittance() const
        {
            return m_SurfaceProperties->transmittance(m_Temperature);
        }

        double ISurface::J() const
        {
            return m_J;
        }

        double ISurface::getMeanDeflection() const
        {
            return m_MeanDeflection;
        }

        double ISurface::getMaxDeflection() const
        {
            return m_MaxDeflection;
        }

        double ISurface::emissivePowerTerm() const
        {
            using ConstantsData::STEFANBOLTZMANN;

            return STEFANBOLTZMANN * m_Emissivity * pow(m_Temperature, 3);
        }

        void ISurface::calculateReflectance()
        {
            if(m_Emissivity + m_Transmittance > 1)
            {
                throw std::runtime_error(
                  "Sum of emittance and transmittance cannot be greater than one.");
            }
            else
            {
                m_Reflectance = 1 - m_Emissivity - m_Transmittance;
            }
        }

        void ISurface::initializeStart(double const t_Temperature)
        {
            using ConstantsData::STEFANBOLTZMANN;

            m_Temperature = t_Temperature;
            m_J = STEFANBOLTZMANN * pow(m_Temperature, 4);
        }

        void ISurface::initializeStart(double const t_Temperature, double const t_Radiation)
        {
            m_Temperature = t_Temperature;
            m_J = t_Radiation;
        }

        std::shared_ptr<ISurface> ISurface::clone() const
        {
            return std::make_shared<ISurface>(*this);
        }

        ISurface::ISurface(const std::vector<std::pair<double, double>> & t_Emissivity,
                           const std::vector<std::pair<double, double>> & t_Transmittance) :
            m_SurfaceProperties(
              std::make_unique<ThermochromicSurfaceProperties>(t_Emissivity, t_Transmittance))
        {}

    }   // namespace ISO15099

}   // namespace Tarcog
