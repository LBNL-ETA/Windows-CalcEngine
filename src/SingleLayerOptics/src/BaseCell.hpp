#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "CellDescription.hpp"
#include "VenetianSegments.hpp"

namespace FenestrationCommon
{
    enum class Side;
    class CSeries;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class CMaterial;
    class CBeamDirection;

    //! Closed set of optical cell variants. Replaces the former CBaseCell -> CSpecularCell /
    //! CUniformDiffuseCell / CDirectionalDiffuseCell / CHomogeneousDiffuseCell /
    //! CMaterialDirectionalDiffuseCell / CPerforatedCell / CWovenCell / CVenetianCell
    //! inheritance tree (including the CVenetianBase virtual diamond).
    enum class CellKind
    {
        Specular,
        UniformDiffuse,
        DirectionalDiffuse,
        HomogeneousDiffuse,
        MaterialDirectionalDiffuse,
        Woven,
        Venetian,
    };

    //! Per-wavelength energy machinery owned only by Venetian cells. The geometry-bundle
    //! sharing introduced by PR #253 lives inside CVenetianEnergy / CVenetianCellEnergy.
    struct VenetianEnergyState
    {
        CVenetianEnergy m_Energy;
        std::vector<CVenetianEnergy> m_EnergiesBand;
    };

    //! Single non-virtual optical cell. The CellKind discriminant + CellDescription variant
    //! + optional VenetianEnergyState carry everything the old hierarchy did. No virtuals,
    //! no diamond. Methods dispatch by switching on m_Kind.
    class CBaseCell
    {
    public:
        CBaseCell();
        CBaseCell(std::shared_ptr<CMaterial> material,
                  CellDescription description,
                  CellKind kind,
                  double rotation = 0);

        void setSourceData(FenestrationCommon::CSeries const & sourceData);
        void setBandWavelengths(std::vector<double> const & wavelengths);

        // ----- direct-direct -----
        double T_dir_dir(FenestrationCommon::Side, CBeamDirection const &);
        double R_dir_dir(FenestrationCommon::Side, CBeamDirection const &);
        double T_dir_dir_at_wavelength(FenestrationCommon::Side,
                                       CBeamDirection const &,
                                       std::size_t wavelengthIndex);
        double R_dir_dir_at_wavelength(FenestrationCommon::Side,
                                       CBeamDirection const &,
                                       std::size_t wavelengthIndex);
        std::vector<double> T_dir_dir_band(FenestrationCommon::Side, CBeamDirection const &);
        std::vector<double> R_dir_dir_band(FenestrationCommon::Side, CBeamDirection const &);

        // ----- single-direction diffuse (Uniform / Woven / Venetian-uniform) -----
        double T_dir_dif(FenestrationCommon::Side, CBeamDirection const &);
        double R_dir_dif(FenestrationCommon::Side, CBeamDirection const &);
        std::vector<double> T_dir_dif_band(FenestrationCommon::Side, CBeamDirection const &);
        std::vector<double> R_dir_dif_band(FenestrationCommon::Side, CBeamDirection const &);
        double T_dir_dif_at_wavelength(FenestrationCommon::Side,
                                       CBeamDirection const &,
                                       std::size_t wavelengthIndex);
        double R_dir_dif_at_wavelength(FenestrationCommon::Side,
                                       CBeamDirection const &,
                                       std::size_t wavelengthIndex);

        // ----- two-direction diffuse (Directional / Homogeneous / MaterialDirDif / Venetian-directional) -----
        double T_dir_dif(FenestrationCommon::Side,
                         CBeamDirection const & incoming,
                         CBeamDirection const & outgoing);
        double R_dir_dif(FenestrationCommon::Side,
                         CBeamDirection const & incoming,
                         CBeamDirection const & outgoing);
        std::vector<double> T_dir_dif_band(FenestrationCommon::Side,
                                           CBeamDirection const & incoming,
                                           CBeamDirection const & outgoing);
        std::vector<double> R_dir_dif_band(FenestrationCommon::Side,
                                           CBeamDirection const & incoming,
                                           CBeamDirection const & outgoing);
        double T_dir_dif_by_wavelength(FenestrationCommon::Side,
                                       CBeamDirection const & incoming,
                                       CBeamDirection const & outgoing,
                                       std::size_t wavelengthIndex);
        double R_dir_dif_by_wavelength(FenestrationCommon::Side,
                                       CBeamDirection const & incoming,
                                       CBeamDirection const & outgoing,
                                       std::size_t wavelengthIndex);

        //! Venetian-only diffuse-diffuse properties (computed from the slat energy balance).
        //! Asserts m_Kind == Venetian; not meaningful for other kinds.
        double T_dif_dif(FenestrationCommon::Side);
        double R_dif_dif(FenestrationCommon::Side);

        // ----- bookkeeping -----
        std::vector<double> getBandWavelengths() const;
        int getBandIndex(double wavelength) const;
        std::size_t getBandSize() const;
        double getMinLambda() const;
        double getMaxLambda() const;
        void Flipped(bool flipped);
        std::shared_ptr<CMaterial> getMaterial();

        [[nodiscard]] CellKind kind() const noexcept
        {
            return m_Kind;
        }
        [[nodiscard]] CellDescription const & description() const noexcept
        {
            return m_CellDescription;
        }

    private:
        void generateVenetianEnergy();

        std::shared_ptr<CMaterial> m_Material;
        CellDescription m_CellDescription;
        CellKind m_Kind{CellKind::Specular};
        double m_CellRotation{0.0};
        std::optional<VenetianEnergyState> m_Venetian;
    };

    //! Factory functions — preferred construction path. Each pairs the right CellKind with
    //! the description alternative the kind expects, and (for Venetian) triggers the energy
    //! machinery setup inside the constructor.
    CBaseCell makeSpecularCell(std::shared_ptr<CMaterial> material);
    CBaseCell makeUniformDiffuseCell(std::shared_ptr<CMaterial> material,
                                     CellDescription description);
    CBaseCell makeDirectionalDiffuseCell(std::shared_ptr<CMaterial> material,
                                         CellDescription description);
    CBaseCell makeHomogeneousDiffuseCell(std::shared_ptr<CMaterial> material,
                                         CellDescription description);
    CBaseCell makeMaterialDirectionalDiffuseCell(std::shared_ptr<CMaterial> material,
                                                 CellDescription description);
    CBaseCell makeWovenCell(std::shared_ptr<CMaterial> material,
                            CWovenCellDescription description);
    CBaseCell makeVenetianCell(std::shared_ptr<CMaterial> material,
                               CVenetianCellDescription description,
                               double rotation = 0);
}   // namespace SingleLayerOptics
