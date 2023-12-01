#include <cmath>
#include <memory>

#include <WCECommon.hpp>

#include "SupportPillar.hpp"


namespace Tarcog::ISO15099
{
    PillarCellSpacing::PillarCellSpacing(double sx, double sy) : Sx(sx), Sy(sy)
    {}

    double cellArea(PillarCellSpacing cell)
    {
        return cell.Sx * cell.Sy;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CSupportPillar
    ////////////////////////////////////////////////////////////////////////////
    SupportPillar::SupportPillar(const CIGUGapLayer & layer,
                                 const double materialConductivity,
                                 const PillarCellSpacing & cell) :
        CIGUGapLayer(layer),
        m_MaterialConductivity(materialConductivity),
        m_CellArea(cellArea(cell))
    {}

    void SupportPillar::calculateConvectionOrConductionFlow()
    {
        CIGUGapLayer::calculateConvectionOrConductionFlow();
        if(!isCalculated())
        {
            m_ConductiveConvectiveCoeff += conductivityOfPillarArray();
        }
    }

    double SupportPillar::singlePillarThermalResistance()
    {
        auto aveCond{(getPreviousLayer()->getConductivity() + getNextLayer()->getConductivity())
                     / 2};
        return ConstantsData::WCE_PI / (2 * aveCond * std::sqrt(areaOfContact()))
               + m_Thickness / (m_MaterialConductivity * areaOfContact());
    }

    double SupportPillar::conductivityOfPillarArray()
    {
        return 1 / (m_CellArea * singlePillarThermalResistance());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////  CylindricalPillar
    ////////////////////////////////////////////////////////////////////////////
    CylindricalPillar::CylindricalPillar(const CIGUGapLayer & layer,
                                         double radius,
                                         double materialConductivity,
                                         const PillarCellSpacing & cell) :
        SupportPillar(layer, materialConductivity, cell), m_Radius(radius)
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
                                     const PillarCellSpacing & cell) :
        SupportPillar(layer, materialConductivity, cell), m_RadiusOfContact(radiusOfContact)
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
                                         const PillarCellSpacing & cell) :
        SupportPillar(layer, materialConductivity, cell),
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
}   // namespace Tarcog::ISO15099
