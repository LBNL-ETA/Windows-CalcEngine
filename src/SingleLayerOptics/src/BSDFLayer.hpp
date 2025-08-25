#pragma once

#include <memory>
#include <vector>

#include "BSDFDirections.hpp"
#include "BSDFIntegrator.hpp"

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

    // Base class for handling BSDF Layer
    class CBSDFLayer
    {
    public:
        virtual ~CBSDFLayer() = default;
        CBSDFLayer(const std::shared_ptr<CBaseCell> & t_Cell, const BSDFHemisphere & t_Directions);

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

        [[nodiscard]] virtual std::vector<std::vector<double>> jscPrime(
            FenestrationCommon::Side t_Side,
            const std::vector<double> & wavelengths = std::vector<double>()) const;

        [[nodiscard]] virtual std::vector<double> voc(const std::vector<double> & electricalCurrent) const;
        [[nodiscard]] virtual std::vector<double> ff(const std::vector<double> & electricalCurrent) const;

    protected:
        // Diffuse calculation distribution will be calculated here. It will depend on base classes.
        // It can for example be uniform or directional. In case of specular layers there will be no
        // any diffuse distribution
        virtual void calcDiffuseDistribution(const FenestrationCommon::Side aSide,
                                             const CBeamDirection & t_Direction,
                                             const size_t t_DirectionIndex) = 0;

        virtual void calcDiffuseDistribution_wv(const FenestrationCommon::Side aSide,
                                                const CBeamDirection & t_Direction,
                                                const size_t t_DirectionIndex,
                                                std::vector<BSDFIntegrator> & results) = 0;

        virtual void calcDiffuseDistribution_byWavelength(const FenestrationCommon::Side aSide,
                                                          const CBeamDirection & t_Direction,
                                                          const size_t t_DirectionIndex,
                                                          size_t wavelengthIndex,
                                                          BSDFIntegrator & results) = 0;

        // BSDF layer is not calculated by default because it is time consuming process and in some
        // cases this call is not necessary. However, refactoring is needed since there is no reason
        // to create CBSDFLayer if it will not be calculated
        void calculate();
        std::vector<BSDFIntegrator> calculate_wv();

        const BSDFHemisphere m_BSDFHemisphere;
        std::shared_ptr<CBaseCell> m_Cell;
        BSDFIntegrator m_Results;

    private:
        void calc_dir_dir();
        void calc_dir_dif();
        // Keeps state of the object. Calculations are not done by default (in constructor)
        // because they are time-consuming.
        bool m_Calculated;

        // Calculation of results over each wavelength
        void calc_dir_dir_wv(std::vector<BSDFIntegrator> & results);
        void calc_dir_dif_wv(std::vector<BSDFIntegrator> & results);

        void calculate_dir_dir_wl(size_t wavelengthIndex, BSDFIntegrator & results) const;
        void calculate_dir_dif_wv(size_t wavelengthIndex, BSDFIntegrator & results);
    };

}   // namespace SingleLayerOptics
