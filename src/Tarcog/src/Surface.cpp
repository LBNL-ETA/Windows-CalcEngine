#include <cmath>
#include <stdexcept>

#include "BaseLayer.hpp"
#include "Surface.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        //////////////////////////////////////////////////////////////////////////////
        /// Surface
        //////////////////////////////////////////////////////////////////////////////
        Surface::Surface() :
            m_SurfaceProperties(std::make_unique<ConstantSurfaceProperties>(0.84, 0))
        {
            calculateReflectance();
        }

        Surface::Surface(double const t_Emissivity, double const t_Transmittance) :
            m_Emissivity(t_Emissivity),
            m_Transmittance(t_Transmittance),
            m_SurfaceProperties(
              std::make_unique<ConstantSurfaceProperties>(m_Emissivity, m_Transmittance))
        {
            calculateReflectance();
        }

        Surface::Surface(Surface const & t_Surface)
        {
            operator=(t_Surface);
        }

        Surface & Surface::operator=(Surface const & t_Surface)
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

        void Surface::setTemperature(double const t_Temperature)
        {
            m_Temperature = t_Temperature;
        }

        void Surface::setJ(double const t_J)
        {
            m_J = t_J;
        }

        void Surface::applyDeflection(double const t_MeanDeflection, double const t_MaxDeflection)
        {
            m_MeanDeflection = t_MeanDeflection;
            m_MaxDeflection = t_MaxDeflection;
        }

        double Surface::getTemperature() const
        {
            return m_Temperature;
        }

        double Surface::getEmissivity() const
        {
            return m_SurfaceProperties->emissivity(m_Temperature);
        }

        double Surface::getReflectance() const
        {
            return m_Reflectance;
        }

        double Surface::getTransmittance() const
        {
            return m_SurfaceProperties->transmittance(m_Temperature);
        }

        double Surface::J() const
        {
            return m_J;
        }

        double Surface::getMeanDeflection() const
        {
            return m_MeanDeflection;
        }

        double Surface::getMaxDeflection() const
        {
            return m_MaxDeflection;
        }

        double Surface::emissivePowerTerm() const
        {
            using ConstantsData::STEFANBOLTZMANN;

            return STEFANBOLTZMANN * m_Emissivity * pow(m_Temperature, 3);
        }

        void Surface::calculateReflectance()
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

        void Surface::initializeStart(double const t_Temperature)
        {
            using ConstantsData::STEFANBOLTZMANN;

            m_Temperature = t_Temperature;
            m_J = STEFANBOLTZMANN * pow(m_Temperature, 4);
        }

        void Surface::initializeStart(double const t_Temperature, double const t_Radiation)
        {
            m_Temperature = t_Temperature;
            m_J = t_Radiation;
        }

        std::shared_ptr<Surface> Surface::clone() const
        {
            return std::make_shared<Surface>(*this);
        }

        Surface::Surface(const std::vector<std::pair<double, double>> & t_Emissivity,
                           const std::vector<std::pair<double, double>> & t_Transmittance) :
            m_SurfaceProperties(
              std::make_unique<ThermochromicSurfaceProperties>(t_Emissivity, t_Transmittance))
        {}

    }   // namespace ISO15099

}   // namespace Tarcog
