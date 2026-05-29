#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include "BSDFDirections.hpp"
#include "BSDFIntegrator.hpp"
#include "PhotovoltaicProperties.hpp"

namespace FenestrationCommon
{
    enum class Side;
    class CSeries;

}   // namespace FenestrationCommon

namespace MultiLayerOptics
{
    class CEquivalentBSDFLayer;
}

namespace SingleLayerOptics
{
    enum class BSDFDirection;
    class CBaseCell;
    class CBeamDirection;
    class BSDFDirections;
    class CMaterialPhotovoltaicSample;

    //! Closed set of BSDF layer behaviours. Replaces the former CBSDFLayer ->
    //! CSpecularBSDFLayer / CUniformDiffuseBSDFLayer / CDirectionalBSDFLayer (and its
    //! CDirectionalDiffuse / CHomogeneousDiffuse / CMaterialDirectionalDiffuse / CMatrix
    //! variants) / PhotovoltaicSpecularBSDFLayer inheritance tree.
    enum class BSDFLayerKind
    {
        Specular,         //!< no diffuse distribution
        UniformDiffuse,   //!< isotropic 1/pi spread of the cell's dir-dif properties
        Directional       //!< per-outgoing-patch spread, weighted by the configured WeightFn
    };

    //! Selects which lambda the WeightFn is evaluated against (Directional only):
    //!   Outgoing - per outgoing patch (e.g. matrix-style 1/lam density conversion)
    //!   Incoming - per incoming patch (e.g. Lambertian 1/(pi - lam_in) row-constant)
    enum class WeightSource
    {
        Outgoing,
        Incoming
    };

    //! Single non-virtual BSDF layer. The BSDFLayerKind discriminant + optional directional
    //! weighting + optional photovoltaic state carry everything the old hierarchy did. Methods
    //! dispatch by switching on m_Kind; construction goes through CBSDFLayerMaker.
    class CBSDFLayer
    {
    public:
        using WeightFn = std::function<double(double)>;

        //! Specular / UniformDiffuse layer (no per-patch weighting).
        CBSDFLayer(std::shared_ptr<CBaseCell> t_Cell,
                   const BSDFHemisphere & t_Directions,
                   BSDFLayerKind kind,
                   bool emissivityPolynomialApplicable = false);

        //! Directional layer (weighted outgoing/incoming distribution).
        CBSDFLayer(std::shared_ptr<CBaseCell> t_Cell,
                   const BSDFHemisphere & t_Directions,
                   WeightFn weightFn,
                   WeightSource source,
                   bool emissivityPolynomialApplicable);

        void setSourceData(const FenestrationCommon::CSeries & t_SourceData);

        // BSDF results for the enire spectrum range of the material in the cell
        BSDFIntegrator getResults();

        const BSDFDirections & getDirections(BSDFDirection t_Side) const;

        // BSDF results for each wavelenght given in specular cell
        std::vector<BSDFIntegrator> getWavelengthResults();
        BSDFIntegrator getResultsAtWavelength(size_t wavelengthIndex);

        int getBandIndex(double t_Wavelength);

        std::vector<double> getBandWavelengths() const;
        void setBandWavelengths(const std::vector<double> & wavelengths);

        std::shared_ptr<CBaseCell> getCell() const;

        bool isEmissivityPolynomialApplicable() const;

        //! Turns this (specular) layer into a photovoltaic one: binds the cell's
        //! CMaterialPhotovoltaicSample (throws if the material is not photovoltaic) and stores
        //! the power table. After this jscPrime/voc/ff return photovoltaic results.
        void assignPowerTable(PVPowerPropertiesTable powerTable);

        [[nodiscard]] std::vector<std::vector<double>>
          jscPrime(FenestrationCommon::Side t_Side,
                   const std::vector<double> & wavelengths = std::vector<double>()) const;

        [[nodiscard]] std::vector<double> voc(const std::vector<double> & electricalCurrent) const;
        [[nodiscard]] std::vector<double> ff(const std::vector<double> & electricalCurrent) const;

    protected:
        // Diffuse distribution dispatched on m_Kind: none for Specular, isotropic for
        // UniformDiffuse, weighted per-outgoing-patch for Directional.
        void calcDiffuseDistribution(FenestrationCommon::Side aSide,
                                     const CBeamDirection & t_Direction,
                                     size_t t_DirectionIndex);

        void calcDiffuseDistribution_wv(FenestrationCommon::Side aSide,
                                        const CBeamDirection & t_Direction,
                                        size_t t_DirectionIndex,
                                        std::vector<BSDFIntegrator> & results);

        void calcDiffuseDistribution_byWavelength(FenestrationCommon::Side aSide,
                                                  const CBeamDirection & t_Direction,
                                                  size_t t_DirectionIndex,
                                                  size_t wavelengthIndex,
                                                  BSDFIntegrator & results);

        // BSDF layer is not calculated by default because it is time consuming process and in some
        // cases this call is not necessary.
        void calculate();
        std::vector<BSDFIntegrator> calculate_wv();

        const BSDFHemisphere m_BSDFHemisphere;
        std::shared_ptr<CBaseCell> m_Cell;
        std::optional<BSDFIntegrator> m_Results;

    private:
        void calc_dir_dir();
        void calc_dir_dif();

        // Calculation of results over each wavelength
        void calc_dir_dir_wv(std::vector<BSDFIntegrator> & results);
        void calc_dir_dif_wv(std::vector<BSDFIntegrator> & results);

        void calculate_dir_dir_wl(size_t wavelengthIndex, BSDFIntegrator & results) const;
        void calculate_dir_dif_wv(size_t wavelengthIndex, BSDFIntegrator & results);

        void invalidate() noexcept;

        // Diffuse-distribution bodies for the individual kinds.
        void uniformDiffuse(FenestrationCommon::Side aSide,
                            const CBeamDirection & t_Direction,
                            size_t t_DirectionIndex);
        void uniformDiffuse_wv(FenestrationCommon::Side aSide,
                               const CBeamDirection & t_Direction,
                               size_t t_DirectionIndex,
                               std::vector<BSDFIntegrator> & results);
        void uniformDiffuse_byWavelength(FenestrationCommon::Side aSide,
                                         const CBeamDirection & t_Direction,
                                         size_t t_DirectionIndex,
                                         size_t wavelengthIndex,
                                         BSDFIntegrator & results);
        void directional(FenestrationCommon::Side aSide,
                         const CBeamDirection & t_Direction,
                         size_t t_DirectionIndex);
        void directional_wv(FenestrationCommon::Side aSide,
                            const CBeamDirection & t_Direction,
                            size_t t_DirectionIndex,
                            std::vector<BSDFIntegrator> & results);
        void directional_byWavelength(FenestrationCommon::Side aSide,
                                      const CBeamDirection & t_Direction,
                                      size_t t_DirectionIndex,
                                      size_t wavelengthIndex,
                                      BSDFIntegrator & results);

        // Directional weighting machinery (only populated/used for BSDFLayerKind::Directional).
        std::span<const double> weights() const noexcept
        {
            return {m_weights.data(), m_weights.size()};
        }

        template<class F>
        void for_each_outgoing_(const size_t inIdx, F && f) const
        {
            const auto & dirs = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);
            const auto wght = weights();
            const size_t numOut = dirs.size();
            const double sIn = (m_weightSource == WeightSource::Incoming) ? wght[inIdx] : 1.0;
            for(size_t out = 0; out < numOut; ++out)
            {
                const CBeamDirection & oDir = dirs[out].centerPoint();
                const double sOut = (m_weightSource == WeightSource::Outgoing) ? wght[out] : 1.0;
                f(out, oDir, sIn * sOut);
            }
        }

        BSDFLayerKind m_Kind{BSDFLayerKind::Specular};
        bool m_EmissivityPolynomialApplicable{false};

        std::vector<double> m_lambdas;
        std::vector<double> m_weights;
        WeightSource m_weightSource{WeightSource::Outgoing};

        std::shared_ptr<CMaterialPhotovoltaicSample> m_PVMaterial;
        PVPowerPropertiesTable m_PVPowerTable;
    };

}   // namespace SingleLayerOptics
