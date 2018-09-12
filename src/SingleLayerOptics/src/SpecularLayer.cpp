#include "SpecularLayer.hpp"

namespace SingleLayerOptics
{
    SpecularLayer::SpecularLayer(const CSpecularCell & m_Cell) : m_Cell(m_Cell)
    {}

    SpecularLayer SpecularLayer::createLayer(const std::shared_ptr<CMaterial> & t_Material)
    {
        auto aCell = CSpecularCell(t_Material);
        return SpecularLayer(aCell);
    }

    std::vector<double> SpecularLayer::R_dir_dir_band(FenestrationCommon::Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Cell.R_dir_dir_band(t_Side, t_Direction);
    }

    std::vector<double> SpecularLayer::T_dir_dir_band(FenestrationCommon::Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Cell.T_dir_dir_band(t_Side, t_Direction);
    }

    double SpecularLayer::R_dir_dir(FenestrationCommon::Side t_Side,
                                    const CBeamDirection & t_Direction)
    {
        return m_Cell.R_dir_dir(t_Side, t_Direction);
    }

    double SpecularLayer::T_dir_dir(FenestrationCommon::Side t_Side,
                                    const CBeamDirection & t_Direction)
    {
        return m_Cell.T_dir_dir(t_Side, t_Direction);
    }

    std::vector<double> SpecularLayer::getBandWavelengths() const
    {
        return m_Cell.getBandWavelengths();
    }

    void SpecularLayer::setSourceData(std::shared_ptr<FenestrationCommon::CSeries> t_SourceData)
    {
        m_Cell.setSourceData(t_SourceData);
    }
}   // namespace SingleLayerOptics