#include <cassert>
#include <stdexcept>
#include <cmath>

#include "BaseShade.hpp"
#include "Surface.hpp"
#include "IGUGapLayer.hpp"
#include "Environment.hpp"
#include "TarcogConstants.hpp"
#include "IGUVentilatedGapLayer.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        auto const OPENING_TOLERANCE = 1e-6;

        ////////////////////////////////////////////////////////////////////////////////////////////////
        /// CShadeOpenings
        ////////////////////////////////////////////////////////////////////////////////////////////////

        CShadeOpenings::CShadeOpenings(double const t_Atop,
                                       double const t_Abot,
                                       double const t_Aleft,
                                       double const t_Aright,
                                       double const t_Afront,
                                       double const t_FrontPorosity) :
            m_Atop(t_Atop),
            m_Abot(t_Abot),
            m_Aleft(t_Aleft),
            m_Aright(t_Aright),
            m_Afront(t_Afront),
            m_FrontPorosity(t_FrontPorosity)
        {
            initialize();
        }

        CShadeOpenings::CShadeOpenings() :
            m_Atop(0), m_Abot(0), m_Aleft(0), m_Aright(0), m_Afront(0)
        {
            initialize();
        }

        void CShadeOpenings::initialize()
        {
            if(m_Atop == 0)
            {
                m_Atop = OPENING_TOLERANCE;
            }

            if(m_Abot == 0)
            {
                m_Abot = OPENING_TOLERANCE;
            }
        }

        double CShadeOpenings::openingMultiplier()
        {
            return (m_Aleft + m_Aright + m_Afront) / (m_Abot + m_Atop);
        }

        double CShadeOpenings::Aeq_bot()
        {
            return m_Abot + 0.5 * m_Atop * openingMultiplier();
        }

        double CShadeOpenings::Aeq_top()
        {
            return m_Atop + 0.5 * m_Abot * openingMultiplier();
        }

        double CShadeOpenings::frontPorositiy() const
        {
            return m_FrontPorosity;
        }

        bool CShadeOpenings::isOpen() const
        {
            return m_Abot > 0 || m_Atop > 0 || m_Aleft > 0 || m_Aright > 0 || m_Afront > 0;
        }

        void CShadeOpenings::checkAndSetDominantWidth(const double gapWidth)
        {
            m_Abot = std::min(gapWidth, m_Abot);
            m_Atop = std::min(gapWidth, m_Atop);
            m_Aleft = std::min(gapWidth, m_Aleft);
            m_Aright = std::min(gapWidth, m_Aright);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////
        /// CIGUShadeLayer
        ////////////////////////////////////////////////////////////////////////////////////////////////

        CIGUShadeLayer::CIGUShadeLayer(
          const double t_Thickness,
          const double t_Conductivity,
          const std::shared_ptr<CShadeOpenings> & t_ShadeOpenings,
          const std::shared_ptr<Tarcog::ISO15099::ISurface> & t_FrontSurface,
          const std::shared_ptr<Tarcog::ISO15099::ISurface> & t_BackSurface) :
            CIGUSolidLayer(t_Thickness, t_Conductivity, t_FrontSurface, t_BackSurface),
            m_ShadeOpenings(t_ShadeOpenings),
            m_MaterialConductivity(t_Conductivity)
        {}

        CIGUShadeLayer::CIGUShadeLayer(std::shared_ptr<CIGUSolidLayer> & t_Layer,
                                       std::shared_ptr<CShadeOpenings> & t_ShadeOpenings) :
            CIGUSolidLayer(*t_Layer),
            m_ShadeOpenings(t_ShadeOpenings),
            m_MaterialConductivity(t_Layer->getConductance())
        {}

        CIGUShadeLayer::CIGUShadeLayer(double t_Thickness, double t_Conductivity) :
            CIGUSolidLayer(t_Thickness, t_Conductivity),
            m_ShadeOpenings(std::make_shared<CShadeOpenings>()),
            m_MaterialConductivity(t_Conductivity)
        {}

        std::shared_ptr<CBaseLayer> CIGUShadeLayer::clone() const
        {
            return std::make_shared<CIGUShadeLayer>(*this);
        }

        bool CIGUShadeLayer::isPermeable() const
        {
            return m_ShadeOpenings->isOpen();
        }

        void CIGUShadeLayer::setDominanthAirflowWidth()
        {
            if(m_PreviousLayer != nullptr)
            {
                m_ShadeOpenings->checkAndSetDominantWidth(m_PreviousLayer->getThickness());
            }
            if(m_NextLayer != nullptr)
            {
                m_ShadeOpenings->checkAndSetDominantWidth(m_NextLayer->getThickness());
            }
        }

        void CIGUShadeLayer::calculateConvectionOrConductionFlow()
        {
            setDominanthAirflowWidth();
            m_Conductivity =
              equivalentConductivity(m_MaterialConductivity, m_ShadeOpenings->frontPorositiy());
            CIGUSolidLayer::calculateConvectionOrConductionFlow();
            assert(m_NextLayer != nullptr);
            assert(m_PreviousLayer != nullptr);

            // This must be set here or gap will be constantly calling this routine back throughout
            // nextLayer property.
            setCalculated();

            if(std::dynamic_pointer_cast<CIGUGapLayer>(m_PreviousLayer) != nullptr
               && std::dynamic_pointer_cast<CIGUGapLayer>(m_NextLayer) != nullptr)
            {
                auto previousGapLayer =
                  std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(m_PreviousLayer);
                auto nextGapLayer = std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(m_NextLayer);
                assert(!previousGapLayer->isVentilationForced()
                       && !nextGapLayer->isVentilationForced());
                calcInBetweenShadeFlow(previousGapLayer, nextGapLayer);
            }
            else if(std::dynamic_pointer_cast<CEnvironment>(m_PreviousLayer) != nullptr
                    && std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(m_NextLayer) != nullptr)
            {
                calcEdgeShadeFlow(std::dynamic_pointer_cast<CEnvironment>(m_PreviousLayer),
                                  std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(m_NextLayer));
            }
            else if(std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(m_PreviousLayer) != nullptr
                    && std::dynamic_pointer_cast<CEnvironment>(m_NextLayer) != nullptr)
            {
                calcEdgeShadeFlow(
                  std::dynamic_pointer_cast<CEnvironment>(m_NextLayer),
                  std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(m_PreviousLayer));
            }
        }

        void CIGUShadeLayer::calcInBetweenShadeFlow(std::shared_ptr<CIGUVentilatedGapLayer> t_Gap1,
                                                    std::shared_ptr<CIGUVentilatedGapLayer> t_Gap2)
        {
            double Tup = t_Gap1->layerTemperature();
            double Tdown = t_Gap2->layerTemperature();
            VentilatedTemperature current{Tdown, Tup};
            double RelaxationParameter = IterationConstants::RELAXATION_PARAMETER_AIRFLOW;
            bool converged = false;
            size_t iterationStep = 0;

            t_Gap1->setFlowGeometry(m_ShadeOpenings->Aeq_bot(), m_ShadeOpenings->Aeq_top());
            t_Gap2->setFlowGeometry(m_ShadeOpenings->Aeq_top(), m_ShadeOpenings->Aeq_bot());

            while(!converged)
            {
                t_Gap1->setInletTemperature(t_Gap2->layerTemperature());
                t_Gap2->setInletTemperature(t_Gap1->layerTemperature());

                VentilatedTemperature previous{current};

                current = t_Gap1->calculateInletAndOutletTemperaturesWithTheAdjecentGap(
                  *t_Gap2, current, previous, RelaxationParameter);

                converged = std::abs(current.outlet - previous.outlet)
                            < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;
                converged = converged
                            && std::abs(current.inlet - previous.inlet)
                                 < IterationConstants::CONVERGENCE_TOLERANCE_AIRFLOW;

                ++iterationStep;
                if(iterationStep > IterationConstants::NUMBER_OF_STEPS)
                {
                    converged = true;
                    throw std::runtime_error("Airflow iterations fail to converge. Maximum number "
                                             "of iteration steps reached.");
                }
                double qv1 = t_Gap1->getGainFlow();
                double qv2 = t_Gap2->getGainFlow();
                t_Gap1->smoothEnergyGain(qv1, qv2);
                t_Gap2->smoothEnergyGain(qv1, qv2);
            }
        }

        void CIGUShadeLayer::calcEdgeShadeFlow(std::shared_ptr<CEnvironment> t_Environment,
                                               std::shared_ptr<CIGUVentilatedGapLayer> t_Gap)
        {
            t_Gap->setFlowGeometry(m_ShadeOpenings->Aeq_bot(), m_ShadeOpenings->Aeq_top());
            const auto environmentTemperature{t_Environment->getGasTemperature()};

            t_Gap->calculateVentilatedAirflow(environmentTemperature);
        }

        double CIGUShadeLayer::equivalentConductivity(const double t_Conductivity,
                                                      const double permeabilityFactor)
        {
            const auto standardPressure{101325.0};   // Pa
            const auto Tf{m_Surface.at(FenestrationCommon::Side::Front)->getTemperature()};
            const auto Tb{m_Surface.at(FenestrationCommon::Side::Back)->getTemperature()};
            Gases::CGas air;
            air.setTemperatureAndPressure((Tf + Tb) / 2, standardPressure);
            const auto airThermalConductivity = air.getGasProperties().m_ThermalConductivity;
            return airThermalConductivity * permeabilityFactor
                   + (1 - permeabilityFactor) * t_Conductivity;
        }
    }   // namespace ISO15099

}   // namespace Tarcog
