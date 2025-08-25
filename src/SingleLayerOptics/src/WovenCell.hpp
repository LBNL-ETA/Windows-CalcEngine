#ifndef WOVENCELL_H
#define WOVENCELL_H

#include <memory>

#include "UniformDiffuseCell.hpp"

namespace SingleLayerOptics
{
    class CWovenCellDescription;
    class ICellDescription;
    class CBeamDirection;

    class CWovenCell : public CUniformDiffuseCell
    {
    public:
        CWovenCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                   const std::shared_ptr<ICellDescription> & t_Cell);

        double T_dir_dif(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;
        double R_dir_dif(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;

        std::vector<double> T_dir_dif_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) override;
        double T_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_Direction,
                                       size_t wavelengthIndex) override;

        std::vector<double> R_dir_dif_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) override;
        double R_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_Direction,
                                       size_t wavelengthIndex) override;

    private:
        std::shared_ptr<CWovenCellDescription> getCellAsWoven() const;

        double Tscatter_single(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction) const;
        std::vector<double> T_scatter_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) const;
    };

}   // namespace SingleLayerOptics

#endif
