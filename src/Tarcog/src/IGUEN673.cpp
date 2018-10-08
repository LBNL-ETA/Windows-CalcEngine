#include "IGUEN673.hpp"
#include "WCECommon.hpp"

namespace Tarcog
{
    namespace EN673
    {
        Glass::Glass(const double Conductivity,
                     const double Thickness,
                     const double emissFront,
                     const double emissBack) :
            Thickness(Thickness),
            Conductivity(Conductivity),
            EmissFront(emissFront),
            EmissBack(emissBack)
        {}

        Gap::Gap(const double Thickness, const double Pressure, const Gases::CGas & tGas) :
            Thickness(Thickness),
            Pressure(Pressure),
            Gas(tGas)
        {}

        IGU::BaseLayer::BaseLayer(const double thickness, double & t1, double & t2) :
            m_Thickness(thickness),
            T1(t1),
            T2(t2),
            EmissivityFront(0.84),
            EmissivityBack(0.84)
        {}

        double IGU::BaseLayer::getEmissivityFront() const
        {
            return EmissivityFront;
        }

        void IGU::BaseLayer::setEmissivityFront(double tEmissivityFront)
        {
            EmissivityFront = tEmissivityFront;
        }

        double IGU::BaseLayer::getEmissivityBack() const
        {
            return EmissivityBack;
        }

        void IGU::BaseLayer::setEmissivityBack(double tEmissivityBack)
        {
            EmissivityBack = tEmissivityBack;
        }

        IGU::GapLayer::GapLayer(const Gap & gap, double & t1, double & t2) :
            BaseLayer(gap.Thickness, t1, t2),
            pressure(gap.Pressure),
            m_Gas(gap.Gas)
        {}

        double IGU::GapLayer::thermalConductance()
        {
            using ConstantsData::STEFANBOLTZMANN;

            const double Tm = (T1 + T2) / 2.0;

            m_Gas.setTemperatureAndPressure(Tm, pressure);
            const auto prop = m_Gas.getGasProperties();
            const auto convection = prop.m_ThermalConductivity / m_Thickness;
            const auto radiation = 4 * STEFANBOLTZMANN * 1
                                   / (1 / EmissivityFront + 1 / EmissivityBack - 1)
                                   * std::pow(Tm, 3);
            return convection + radiation;
        }

        IGU::SolidLayer::SolidLayer(const Glass & glass, double & t1, double & t2) :
            BaseLayer(glass.Thickness, t1, t2),
            m_Conductivity(glass.Conductivity)
        {}

        double IGU::SolidLayer::thermalConductance()
        {
            return m_Conductivity / m_Thickness;
        }

        Environment::Environment(double Temperature, double filmCoefficient) :
            Temperature(Temperature),
            filmCoefficient(filmCoefficient)
        {}

        IGU::IGU(const Environment & interior, const Environment & exterior, const Glass & glass) :
            interior(interior),
            exterior(exterior)
        {
            temperature.push_back(exterior.Temperature + 3);
            temperature.push_back(exterior.Temperature + 6);
            layers.emplace_back(new SolidLayer(glass, temperature[0], temperature[1]));
        }

        void IGU::addGlass(const Glass & glass)
        {
            temperature.push_back(temperature[temperature.size() - 1] + 3);
            auto gap = layers[layers.size() - 1].get();
            if(dynamic_cast<GapLayer *>(gap))
            {
                gap->setEmissivityBack(glass.EmissFront);
                layers.emplace_back(new SolidLayer(
                  glass, temperature[temperature.size() - 2], temperature[temperature.size() - 1]));
            }
            else
            {
                throw std::runtime_error("Cannot put two consecutive glass layers to IGU.");
            }
        }

        void IGU::addGap(const Gap & gap)
        {
            temperature.push_back(temperature[temperature.size() - 1] + 3);
            auto solid = layers[layers.size() - 1].get();
            if(dynamic_cast<SolidLayer *>(solid))
            {
                layers.emplace_back(new GapLayer(
                  gap, temperature[temperature.size() - 2], temperature[temperature.size() - 1]));
                layers[layers.size() - 1]->setEmissivityFront(solid->getEmissivityBack());
            }
            else
            {
                throw std::runtime_error("Cannot put two consecutive gap layers to IGU.");
            }
        }

        double IGU::Uvalue()
        {
            double value = 1 / exterior.filmCoefficient + 1 / interior.filmCoefficient;
            auto valLayers = 0.0;
            for(auto i = 0u; i < layers.size(); ++i)
            {
                valLayers += 1 / layers[i]->thermalConductance();
            }
            return 1 / (value + valLayers);
        }
    }   // namespace EN673
}   // namespace Tarcog
