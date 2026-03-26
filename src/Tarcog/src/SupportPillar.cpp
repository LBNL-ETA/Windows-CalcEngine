#include <memory>
#include <unordered_map>
#include <functional>

#include <WCECommon.hpp>

#include "SupportPillar.hpp"


namespace Tarcog::ISO15099
{

    double pillarCellArea(CellSpacingType type, double sp)
    {
        static const std::unordered_map<CellSpacingType, std::function<double(double)>>
          areaCalculators = {
            {CellSpacingType::Square, [](double val) { return val * val; }},
            {CellSpacingType::ShiftedSquare, [](double val) { return val * val; }},
            {CellSpacingType::ShiftedRotatedSquare, [](double val) { return val * val; }},
            {CellSpacingType::NumberOfPillarsPerArea, [](double val) { return val * val; }}};

        auto itr = areaCalculators.find(type);
        if(itr != areaCalculators.end())
        {
            return itr->second(sp);
        }

        return 0;
    }


    double universalPillarThermalResistance(double glassConductivityHotSide,
                                            double glassConductivityColdSide,
                                            double pillarConductivity,
                                            double pillarHeight,
                                            double areaOfContact)
    {
        auto conductivityOneSide = [areaOfContact](double glassConductivity) -> double {
            return std::sqrt(ConstantsData::WCE_PI)
                   / (4 * glassConductivity * std::sqrt(areaOfContact));
        };

        return conductivityOneSide(glassConductivityHotSide)
               + conductivityOneSide(glassConductivityColdSide)
               + pillarHeight / (pillarConductivity * areaOfContact);
    }

    double linearBearingPillarThermalResistance(double glassConductivityHotSide,
                                                double glassConductivityColdSide,
                                                double pillarConductivity,
                                                double pillarHeight,
                                                double length,
                                                double width)
    {
        const auto ratio{length / width};

        auto resistanceGen = [width, length](double glassConductivity) -> double {
            using FenestrationCommon::WCE_PI;
            double num = 1 + std::sqrt(width / length);
            double mag = (1 + width / length) * std::sqrt(width / length);
            double k1 = WCE_PI * std::sqrt(2) / std::sqrt(mag)
                        * (1 - num * std::pow(2, 1.0 / 4) / (4 * std::pow(mag, 1.0 / 4)));
            return 1 / (4 * glassConductivity * length) * (2 / std::sqrt(WCE_PI) * k1);
        };

        if(ratio >= 2)
        {
            return resistanceGen(glassConductivityHotSide)
                   + resistanceGen(glassConductivityColdSide)
                   + pillarHeight / (pillarConductivity * length * width);
        }

        return universalPillarThermalResistance(glassConductivityHotSide,
                                                glassConductivityColdSide,
                                                pillarConductivity,
                                                pillarHeight,
                                                length * width);
    }

    namespace Helper
    {
        double rCS(const double kGlass, const double innerRadius, const double outerRadius)
        {
            using FenestrationCommon::WCE_PI;

            const auto eps{innerRadius / outerRadius};

            if((1 / eps) <= 1.1)
            {
                return 1 / (std::pow(WCE_PI, 2) * kGlass * outerRadius)
                       * (std::log(16) + std::log((1 + eps) / (1 - eps))) / (1 + eps);
            }

            double aaa = WCE_PI / (8 * kGlass * outerRadius);
            double bbb = std::acos(eps) + std::sqrt(1 - std::pow(eps, 2)) * std::atanh(eps);
            double ccc = 1 + (0.0143 * std::pow(std::tan(1.28 * eps), 3)) / eps;

            return aaa * (1 / (bbb * ccc));
        }

        double adjustInnerRadius(const double innerRadius)
        {
            constexpr auto epsilon{1e-9};
            return innerRadius > 0 ? innerRadius : epsilon;
        }

        double adjustOuterRadius(const double innerRadius, const double outerRadius)
        {
            constexpr auto epsilon{1e-9};
            return innerRadius >= outerRadius ? innerRadius + epsilon : outerRadius;
        }
    }   // namespace Helper


    double annulusCylinderPillarThermalResistance(const double pillarConductivity,
                                                  const double height,
                                                  double areaOfContact,
                                                  double rcGlass1,
                                                  double rcGlass2)
    {
        return rcGlass1 + rcGlass2 + height / (pillarConductivity * areaOfContact);
    }

    double cShapedCylinderPillarThermalResistance(double coverageFraction,
                                                  double pillarConductivity,
                                                  double height,
                                                  double areaOfContact,
                                                  double rcGlass1,
                                                  double rcGlass2)
    {
        return annulusCylinderPillarThermalResistance(
                 pillarConductivity, height, areaOfContact, rcGlass1, rcGlass2)
               / std::sqrt(0.94 * coverageFraction);
    }


    ////////////////////////////////////////////////////////////////////////////
    ////  PillarGapLayer
    ////////////////////////////////////////////////////////////////////////////

    PillarGapLayer::PillarGapLayer(const CIGUGapLayer & gapLayer,
                                   double materialConductivity,
                                   double cellArea,
                                   AreaFn areaOfContactFn,
                                   ResistanceFn thermalResistanceFn) :
        CIGUGapLayer(gapLayer),
        m_MaterialConductivity(materialConductivity),
        m_CellArea(cellArea),
        m_AreaOfContactFn(std::move(areaOfContactFn)),
        m_ThermalResistanceFn(std::move(thermalResistanceFn))
    {}

    PillarGapLayer::PillarGapLayer(const CIGUGapLayer & gapLayer,
                                   double measuredConductance) :
        CIGUGapLayer(gapLayer),
        m_MeasuredConductance(measuredConductance)
    {}

    void PillarGapLayer::calculateConvectionOrConductionFlow()
    {
        if(m_MeasuredConductance.has_value())
        {
            if(!isCalculated())
            {
                m_ConductiveConvectiveCoeff = m_MeasuredConductance.value();
                setCalculated();
            }
        }
        else
        {
            CIGUGapLayer::calculateConvectionOrConductionFlow();
            if(!isCalculated())
            {
                m_ConductiveConvectiveCoeff += conductivityOfPillarArray();
            }
        }
    }

    double PillarGapLayer::conductivityOfPillarArray()
    {
        const auto area = m_AreaOfContactFn();
        const auto resistance = m_ThermalResistanceFn(
          getPreviousLayer()->getConductivity(),
          getNextLayer()->getConductivity(),
          m_MaterialConductivity,
          m_Thickness,
          area);
        return 1 / (m_CellArea * resistance);
    }

    std::shared_ptr<CBaseLayer> PillarGapLayer::clone() const
    {
        return std::make_shared<PillarGapLayer>(*this);
    }

}   // namespace Tarcog::ISO15099
