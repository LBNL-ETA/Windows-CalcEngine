#include <cmath>
#include <memory>
#include <unordered_map>
#include <functional>

#include <WCECommon.hpp>

#include "SupportPillar.hpp"


namespace Tarcog::ISO15099
{

    double cellArea(Tarcog::ISO15099::CellSpacingType type, double r)
    {
        static const std::unordered_map<CellSpacingType, std::function<double(double)>>
          areaCalculators = {
            {CellSpacingType::Square, [](double r) { return r * r; }},
            {CellSpacingType::ShiftedSquare,
             [](double r) { return FenestrationCommon::nTagonArea(5, r); }},
            {CellSpacingType::ShiftedRotatedSquare,
             [](double r) { return FenestrationCommon::nTagonArea(5, r); }},
          };

        auto it = areaCalculators.find(type);
        if(it != areaCalculators.end())
        {
            return it->second(r);
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
            double n = 1 + std::sqrt(width / length);
            double m = (1 + width / length) * std::sqrt(width / length);
            double k1 = WCE_PI * std::sqrt(2) / std::sqrt(m)
                        * (1 - n * std::pow(2, 1.0 / 4) / (4 * std::pow(m, 1.0 / 4)));
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

#include <cmath>
#include <algorithm>

    double annulusCylinderPillarThermalResistance(double glass1Conductivity,
                                                  double glass2Conductivity,
                                                  double pillarConductivity,
                                                  double height,
                                                  double innerRadius,
                                                  double outerRadius)
    {
        using FenestrationCommon::WCE_PI;

        constexpr auto epsilon{1e-9};

        if(innerRadius <= 0)
        {
            innerRadius = epsilon;
        }

        if(innerRadius >= outerRadius)
        {
            outerRadius = innerRadius + epsilon;
        }

        double eps = innerRadius / outerRadius;

        auto commonFunc = [height, pillarConductivity, innerRadius, outerRadius]() -> double {
            return height
                   / (pillarConductivity * WCE_PI
                      * (std::pow(outerRadius, 2) - std::pow(innerRadius, 2)));
        };

        auto rc_smaller_or_equal_than_1_1 = [eps, outerRadius](double k_glass) -> double {
            return 1 / (std::pow(WCE_PI, 2) * k_glass * outerRadius)
                   * (std::log(16) + std::log((1 + eps) / (1 - eps))) / (1 + eps);
        };

        auto rc_bigger_than_1_1 = [eps, outerRadius](double k_glass) -> double {
            double a = WCE_PI / (8 * k_glass * outerRadius);
            double b = std::acos(eps) + std::sqrt(1 - std::pow(eps, 2)) * std::atanh(eps);
            double c = 1 + (0.0143 * std::pow(std::tan(1.28 * eps), 3)) / eps;

            return a * (1 / (b * c));
        };

        if((1 / eps) <= 1.1)
        {
            return rc_smaller_or_equal_than_1_1(glass1Conductivity)
                   + rc_smaller_or_equal_than_1_1(glass2Conductivity) + commonFunc();
        }

        return rc_bigger_than_1_1(glass1Conductivity) + rc_bigger_than_1_1(glass2Conductivity)
               + commonFunc();
    }


    ////////////////////////////////////////////////////////////////////////////
    ////  UniversalSupportPillar
    ////////////////////////////////////////////////////////////////////////////
    UniversalSupportPillar::UniversalSupportPillar(const CIGUGapLayer & layer,
                                                   double materialConductivity,
                                                   double cellArea) :
        CIGUGapLayer(layer), m_MaterialConductivity(materialConductivity), m_CellArea(cellArea)
    {}

    void UniversalSupportPillar::calculateConvectionOrConductionFlow()
    {
        CIGUGapLayer::calculateConvectionOrConductionFlow();
        if(!isCalculated())
        {
            m_ConductiveConvectiveCoeff += conductivityOfPillarArray();
        }
    }

    double UniversalSupportPillar::singlePillarThermalResistance()
    {
        return universalPillarThermalResistance(getPreviousLayer()->getConductivity(),
                                                getNextLayer()->getConductivity(),
                                                m_MaterialConductivity,
                                                m_Thickness,
                                                areaOfContact());
    }

    double UniversalSupportPillar::conductivityOfPillarArray()
    {
        return 1 / (m_CellArea * singlePillarThermalResistance());
    }

    double UniversalSupportPillar::materialConductivity() const
    {
        return m_MaterialConductivity;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CylindricalPillar
    ////////////////////////////////////////////////////////////////////////////
    CylindricalPillarLayer::CylindricalPillarLayer(const CIGUGapLayer & layer,
                                                   const CylindricalPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea),
        m_Radius(data.radius)
    {}

    double CylindricalPillarLayer::areaOfContact()
    {
        return ConstantsData::WCE_PI * m_Radius * m_Radius;
    }

    std::shared_ptr<CBaseLayer> CylindricalPillarLayer::clone() const
    {
        return std::make_shared<CylindricalPillarLayer>(*this);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  SphericalPillar
    ////////////////////////////////////////////////////////////////////////////
    SphericalPillarLayer::SphericalPillarLayer(const CIGUGapLayer & layer,
                                               const SphericalPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea),
        m_RadiusOfContact(data.radiusOfContact)
    {}

    double SphericalPillarLayer::areaOfContact()
    {
        return ConstantsData::WCE_PI * m_RadiusOfContact * m_RadiusOfContact;
    }

    std::shared_ptr<CBaseLayer> SphericalPillarLayer::clone() const
    {
        return std::make_shared<SphericalPillarLayer>(*this);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  RectangularPillar
    ////////////////////////////////////////////////////////////////////////////
    RectangularPillarLayer::RectangularPillarLayer(const CIGUGapLayer & layer,
                                                   const RectangularPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea),
        m_PillarLength(data.length),
        m_PillarWidth(data.width)
    {}

    double RectangularPillarLayer::areaOfContact()
    {
        return m_PillarWidth * m_PillarLength;
    }

    std::shared_ptr<CBaseLayer> RectangularPillarLayer::clone() const
    {
        return std::make_shared<RectangularPillarLayer>(*this);
    }

    double RectangularPillarLayer::singlePillarThermalResistance()
    {
        if(std::max(m_PillarWidth, m_PillarLength) / std::min(m_PillarWidth, m_PillarLength) < 2)
        {
            return UniversalSupportPillar::singlePillarThermalResistance();
        }

        return linearBearingPillarThermalResistance(getPreviousLayer()->getConductivity(),
                                                    getNextLayer()->getConductivity(),
                                                    materialConductivity(),
                                                    m_Thickness,
                                                    m_PillarLength,
                                                    m_PillarWidth);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  TriangularPillar
    ////////////////////////////////////////////////////////////////////////////
    TriangularPillarLayer::TriangularPillarLayer(const CIGUGapLayer & layer,
                                                 const PolygonalPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea),
        m_PillarLength(data.length)
    {}

    std::shared_ptr<CBaseLayer> TriangularPillarLayer::clone() const
    {
        return std::make_shared<TriangularPillarLayer>(*this);
    }

    double TriangularPillarLayer::areaOfContact()
    {
        return std::sqrt(3) / 4 * m_PillarLength * m_PillarLength;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  PentagonPillar
    ////////////////////////////////////////////////////////////////////////////
    PentagonPillarLayer::PentagonPillarLayer(const CIGUGapLayer & layer,
                                             const PolygonalPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea), m_PillarLength(data.length)
    {}

    std::shared_ptr<CBaseLayer> PentagonPillarLayer::clone() const
    {
        return std::make_shared<PentagonPillarLayer>(*this);
    }

    double PentagonPillarLayer::areaOfContact()
    {
        return 5 * m_PillarLength * m_PillarLength
               * std::pow(std::sin(36 * FenestrationCommon::WCE_PI / 180)
                            / std::tan(36 * FenestrationCommon::WCE_PI / 180),
                          2);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  HexagonPillar
    ////////////////////////////////////////////////////////////////////////////
    HexagonPillarLayer::HexagonPillarLayer(const CIGUGapLayer & layer, const PolygonalPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea), m_PillarLength(data.length)
    {}

    std::shared_ptr<CBaseLayer> HexagonPillarLayer::clone() const
    {
        return std::make_shared<HexagonPillarLayer>(*this);
    }

    double HexagonPillarLayer::areaOfContact()
    {
        return 6 * m_PillarLength * m_PillarLength
               * std::pow(std::sin(36 * FenestrationCommon::WCE_PI / 180)
                            / std::tan(36 * FenestrationCommon::WCE_PI / 180),
                          2);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  LinearBearingPillar
    ////////////////////////////////////////////////////////////////////////////
    LinearBearingPillarLayer::LinearBearingPillarLayer(const CIGUGapLayer & layer,
                                                       const LinearBearingPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea),
        m_PillarLength(data.length),
        m_PillarWidth(data.width)
    {}

    std::shared_ptr<CBaseLayer> LinearBearingPillarLayer::clone() const
    {
        return std::make_shared<LinearBearingPillarLayer>(*this);
    }

    double LinearBearingPillarLayer::areaOfContact()
    {
        return m_PillarLength * m_PillarWidth;
    }

    double LinearBearingPillarLayer::singlePillarThermalResistance()
    {
        return linearBearingPillarThermalResistance(getPreviousLayer()->getConductivity(),
                                                    getNextLayer()->getConductivity(),
                                                    materialConductivity(),
                                                    m_Thickness,
                                                    m_PillarLength,
                                                    m_PillarWidth);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  TruncatedConePillar
    ////////////////////////////////////////////////////////////////////////////
    TruncatedConePillarLayer::TruncatedConePillarLayer(const CIGUGapLayer & layer,
                                                       const TruncatedConePillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea),
        m_Radius1(data.radius1),
        m_Radius2(data.radius2)
    {}

    std::shared_ptr<CBaseLayer> TruncatedConePillarLayer::clone() const
    {
        return std::make_shared<TruncatedConePillarLayer>(*this);
    }

    double TruncatedConePillarLayer::areaOfContact()
    {
        return 0;   // Not used in this case, so we will just return zero
    }

    double TruncatedConePillarLayer::singlePillarThermalResistance()
    {
        auto rpGen = [](double GlassConductance, double radius) -> double {
            return 1 / (4 * GlassConductance * radius);
        };

        using FenestrationCommon::WCE_PI;

        return rpGen(getPreviousLayer()->getConductivity(), m_Radius1)
               + rpGen(getNextLayer()->getConductivity(), m_Radius2)
               + m_Thickness / (materialConductivity() * WCE_PI * std::pow(m_Radius1, 2));
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  AnnulusCylinderPillar
    ////////////////////////////////////////////////////////////////////////////
    AnnulusCylinderPillarLayer::AnnulusCylinderPillarLayer(const CIGUGapLayer & layer,
                                                           const AnnulusCylinderPillar & data) :
        UniversalSupportPillar(layer, data.materialConductivity, data.cellArea),
        m_InnerRadius(data.innerRadius),
        m_OuterRadius(data.outerRadius)
    {
        if(m_InnerRadius > m_OuterRadius)
        {
            throw std::runtime_error("Inner radius cannot be bigger than outer radius.");
        }
    }

    std::shared_ptr<CBaseLayer> AnnulusCylinderPillarLayer::clone() const
    {
        return std::make_shared<AnnulusCylinderPillarLayer>(*this);
    }

    double AnnulusCylinderPillarLayer::areaOfContact()
    {
        return 0;
    }

    double AnnulusCylinderPillarLayer::singlePillarThermalResistance()
    {
        return annulusCylinderPillarThermalResistance(getPreviousLayer()->getConductivity(),
                                                      getNextLayer()->getConductivity(),
                                                      materialConductivity(),
                                                      m_Thickness,
                                                      m_InnerRadius,
                                                      m_OuterRadius);
    }
}   // namespace Tarcog::ISO15099
