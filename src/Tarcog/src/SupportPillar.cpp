#include <cmath>
#include <memory>

#include <WCECommon.hpp>

#include "SupportPillar.hpp"


namespace Tarcog::ISO15099
{
    ////////////////////////////////////////////////////////////////////////////
    ////  CSupportPillar
    ////////////////////////////////////////////////////////////////////////////
    UniversalSupportPillar::UniversalSupportPillar(const CIGUGapLayer & layer,
                                 double materialConductivity,
                                                   double cellArea) :
        CIGUGapLayer(layer),
        m_MaterialConductivity(materialConductivity),
        m_CellArea(cellArea)
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
        auto aveCond{(getPreviousLayer()->getConductivity() + getNextLayer()->getConductivity())
                     / 2};
        return std::sqrt(ConstantsData::WCE_PI) / (2 * aveCond * std::sqrt(areaOfContact()))
               + m_Thickness / (m_MaterialConductivity * areaOfContact());
    }

    double UniversalSupportPillar::conductivityOfPillarArray()
    {
        return 1 / (m_CellArea * singlePillarThermalResistance());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CylindricalPillar
    ////////////////////////////////////////////////////////////////////////////
    CylindricalPillar::CylindricalPillar(const CIGUGapLayer & layer,
                                         double radius,
                                         double materialConductivity,
                                         double cellArea) :
        UniversalSupportPillar(layer, materialConductivity, cellArea), m_Radius(radius)
    {}

    double CylindricalPillar::areaOfContact()
    {
        return ConstantsData::WCE_PI * m_Radius * m_Radius;
    }

    std::shared_ptr<CBaseLayer> CylindricalPillar::clone() const
    {
        return std::make_shared<CylindricalPillar>(*this);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  SphericalPillar
    ////////////////////////////////////////////////////////////////////////////
    SphericalPillar::SphericalPillar(const CIGUGapLayer & layer,
                                     double radiusOfContact,
                                     double materialConductivity,
                                     double cellArea) :
        UniversalSupportPillar(layer, materialConductivity, cellArea), m_RadiusOfContact(radiusOfContact)
    {}

    double SphericalPillar::areaOfContact()
    {
        return ConstantsData::WCE_PI * m_RadiusOfContact * m_RadiusOfContact;
    }

    std::shared_ptr<CBaseLayer> SphericalPillar::clone() const
    {
        return std::make_shared<SphericalPillar>(*this);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  RectangularPillar
    ////////////////////////////////////////////////////////////////////////////
    RectangularPillar::RectangularPillar(const CIGUGapLayer & layer,
                                         double length,
                                         double width,
                                         double materialConductivity,
                                         double cellArea) :
        UniversalSupportPillar(layer, materialConductivity, cellArea),
        m_PillarLength(length),
        m_PillarWidth(width)
    {}

    double RectangularPillar::areaOfContact()
    {
        return m_PillarWidth * m_PillarLength;
    }

    std::shared_ptr<CBaseLayer> RectangularPillar::clone() const
    {
        return std::make_shared<RectangularPillar>(*this);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  TriangularPillar
    ////////////////////////////////////////////////////////////////////////////
    TriangularPillar::TriangularPillar(const CIGUGapLayer & layer,
                                       double length,
                                       double materialConductivity,
                                       double cellArea) :
        UniversalSupportPillar(layer, materialConductivity, cellArea), m_PillarLength(length)
    {}

    std::shared_ptr<CBaseLayer> TriangularPillar::clone() const
    {
        return std::make_shared<TriangularPillar>(*this);
    }

    double TriangularPillar::areaOfContact()
    {
        return std::sqrt(3) / 4 * m_PillarLength * m_PillarLength;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  PentagonPillar
    ////////////////////////////////////////////////////////////////////////////
    PentagonPillar::PentagonPillar(const CIGUGapLayer & layer,
                                   double length,
                                   double materialConductivity,
                                   double cellArea) :
        UniversalSupportPillar(layer, materialConductivity, cellArea), m_PillarLength(length)
    {}

    std::shared_ptr<CBaseLayer> PentagonPillar::clone() const
    {
        return std::make_shared<PentagonPillar>(*this);
    }

    double PentagonPillar::areaOfContact()
    {
        return 5 * m_PillarLength * m_PillarLength
               * std::pow(std::sin(36 * FenestrationCommon::WCE_PI / 180)
                            / std::tan(36 * FenestrationCommon::WCE_PI / 180),
                          2);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  HexagonPillar
    ////////////////////////////////////////////////////////////////////////////
    HexagonPillar::HexagonPillar(const CIGUGapLayer & layer,
                                 double length,
                                 double materialConductivity,
                                 double cellArea) :
        UniversalSupportPillar(layer, materialConductivity, cellArea), m_PillarLength(length)
    {}

    std::shared_ptr<CBaseLayer> HexagonPillar::clone() const
    {
        return std::make_shared<HexagonPillar>(*this);
    }

    double HexagonPillar::areaOfContact()
    {
        return 6 * m_PillarLength * m_PillarLength
               * std::pow(std::sin(36 * FenestrationCommon::WCE_PI / 180)
                            / std::tan(36 * FenestrationCommon::WCE_PI / 180),
                          2);
    }
}   // namespace Tarcog::ISO15099
