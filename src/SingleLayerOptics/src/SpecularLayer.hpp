#pragma once

#include "SpecularCell.hpp"

namespace SingleLayerOptics
{
    class SpecularLayer
    {
    public:
        static SpecularLayer createLayer(const std::shared_ptr<CMaterial> & t_Material);

        // Transmittance averaged over entire wavelength spectrum
        double T_dir_dir(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);

        // Reflectance averaged over entire wavelength spectrum
        double R_dir_dir(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);

        // Transmittance of specular material for entire wavelength spectrum
        std::vector<double> T_dir_dir_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction);

        // Reflectance of specular material over entire wavelength spectrum
        std::vector<double> R_dir_dir_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction);

        std::vector<double> getBandWavelengths() const;
        void setSourceData(std::shared_ptr<FenestrationCommon::CSeries> t_SourceData);

        double getMinLambda() const;
        double getMaxLambda() const;

    private:
        explicit SpecularLayer(const CSpecularCell & m_Cell);
        CSpecularCell m_Cell;
    };

}   // namespace SingleLayerOptics
