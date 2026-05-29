#include <cmath>
#include <mutex>
#include <ranges>
#include <stdexcept>
#include <utility>

#include <WCECommon.hpp>

#include "BSDFLayer.hpp"
#include "BaseCell.hpp"
#include "BSDFDirections.hpp"
#include "BSDFIntegrator.hpp"
#include "MaterialDescription.hpp"
#include "BSDFPatch.hpp"
#include "BeamDirection.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    namespace
    {
        //! Guards CMaterialPhotovoltaicSample::jscPrime, which is not thread-safe.
        std::mutex jscPrimeLock;
    }   // namespace

    CBSDFLayer::CBSDFLayer(std::shared_ptr<CBaseCell> t_Cell,
                           const BSDFHemisphere & t_Directions,
                           BSDFLayerKind kind,
                           bool emissivityPolynomialApplicable) :
        m_BSDFHemisphere(t_Directions),
        m_Cell(std::move(t_Cell)),
        m_Results(std::nullopt),
        m_Kind(kind),
        m_EmissivityPolynomialApplicable(emissivityPolynomialApplicable)
    {
        // TODO: Maybe to refactor results to incoming and outgoing if not affecting speed.
        // This is not necessary before axisymmetry is introduced
    }

    CBSDFLayer::CBSDFLayer(std::shared_ptr<CBaseCell> t_Cell,
                           const BSDFHemisphere & t_Directions,
                           WeightFn weightFn,
                           WeightSource source,
                           bool emissivityPolynomialApplicable) :
        m_BSDFHemisphere(t_Directions),
        m_Cell(std::move(t_Cell)),
        m_Results(std::nullopt),
        m_Kind(BSDFLayerKind::Directional),
        m_EmissivityPolynomialApplicable(emissivityPolynomialApplicable),
        m_lambdas(t_Directions.getDirections(BSDFDirection::Outgoing).lambdaVector()),
        m_weights(m_lambdas.size(), 1.0),
        m_weightSource(source)
    {
        if(weightFn)
        {
            std::ranges::transform(m_lambdas, m_weights.begin(), weightFn);
        }
    }

    void CBSDFLayer::setSourceData(const CSeries & t_SourceData)
    {
        m_Cell->setSourceData(t_SourceData);
        invalidate();
    }

    const BSDFDirections & CBSDFLayer::getDirections(const BSDFDirection t_Side) const
    {
        return m_BSDFHemisphere.getDirections(t_Side);
    }

    BSDFIntegrator CBSDFLayer::getResults()
    {
        if(!m_Results)
        {
            calculate();
        }
        return m_Results.value();
    }

    std::vector<BSDFIntegrator> CBSDFLayer::getWavelengthResults()
    {
        return calculate_wv();
    }

    BSDFIntegrator CBSDFLayer::getResultsAtWavelength(size_t wavelengthIndex)
    {
        BSDFIntegrator results{m_BSDFHemisphere.getDirections(BSDFDirection::Incoming)};
        calculate_dir_dir_wl(wavelengthIndex, results);
        calculate_dir_dif_wv(wavelengthIndex, results);
        return results;
    }

    void CBSDFLayer::calculate_dir_dir_wl(size_t wavelengthIndex, BSDFIntegrator & results) const
    {
        for(Side aSide : allSides())
        {
            const auto & aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
            size_t size = aDirections.size();
            for(size_t i = 0; i < size; ++i)
            {
                const CBeamDirection & aDirection = aDirections[i].centerPoint();
                const auto aTau = m_Cell->T_dir_dir_band(aSide, aDirection)[wavelengthIndex];
                const auto aRho = m_Cell->R_dir_dir_band(aSide, aDirection)[wavelengthIndex];
                double Lambda = aDirections[i].lambda();

                auto & tau = results.getMatrix(aSide, PropertySurface::T);
                auto & rho = results.getMatrix(aSide, PropertySurface::R);
                tau(i, i) += aTau / Lambda;
                rho(i, i) += aRho / Lambda;
            }
        }
    }

    void CBSDFLayer::calculate_dir_dif_wv(size_t wavelengthIndex, BSDFIntegrator & results)
    {
        for(Side aSide : allSides())
        {
            const auto & aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);

            for(size_t directionIndex = 0; directionIndex < aDirections.size(); ++directionIndex)
            {
                const CBeamDirection & aDirection = aDirections[directionIndex].centerPoint();
                calcDiffuseDistribution_byWavelength(
                  aSide, aDirection, directionIndex, wavelengthIndex, results);
            }
        }
    }

    void CBSDFLayer::invalidate() noexcept
    {
        m_Results.reset();
    }

    int CBSDFLayer::getBandIndex(const double t_Wavelength)
    {
        return m_Cell->getBandIndex(t_Wavelength);
    }

    std::vector<double> CBSDFLayer::getBandWavelengths() const
    {
        return m_Cell->getBandWavelengths();
    }

    void CBSDFLayer::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        m_Cell->setBandWavelengths(wavelengths);
    }

    void CBSDFLayer::calc_dir_dir()
    {
        for(Side t_Side : allSides())
        {
            BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
            size_t size = aDirections.size();
            SquareMatrix tau{size};
            SquareMatrix rho{size};
            for(size_t i = 0; i < size; ++i)
            {
                const CBeamDirection & aDirection = aDirections[i].centerPoint();
                const double Lambda = aDirections[i].lambda();

                const double aTau = m_Cell->T_dir_dir(t_Side, aDirection);
                const double aRho = m_Cell->R_dir_dir(t_Side, aDirection);

                tau(i, i) += aTau / Lambda;
                rho(i, i) += aRho / Lambda;
            }
            m_Results->setMatrices(tau, rho, t_Side);
        }
    }

    void CBSDFLayer::calc_dir_dir_wv(std::vector<BSDFIntegrator> & results)
    {
        for(Side aSide : allSides())
        {
            const auto & aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
            size_t size = aDirections.size();
            for(size_t i = 0; i < size; ++i)
            {
                const CBeamDirection & aDirection = aDirections[i].centerPoint();
                std::vector<double> aTau = m_Cell->T_dir_dir_band(aSide, aDirection);
                std::vector<double> aRho = m_Cell->R_dir_dir_band(aSide, aDirection);
                double Lambda = aDirections[i].lambda();
                size_t numWV = aTau.size();
                for(size_t j = 0; j < numWV; ++j)
                {
                    auto & tau = results[j].getMatrix(aSide, PropertySurface::T);
                    auto & rho = results[j].getMatrix(aSide, PropertySurface::R);
                    tau(i, i) += aTau[j] / Lambda;
                    rho(i, i) += aRho[j] / Lambda;
                }
            }
        }
    }

    void CBSDFLayer::calc_dir_dif()
    {
        for(Side aSide : allSides())
        {
            const auto & aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);

            size_t size = aDirections.size();
            for(size_t i = 0; i < size; ++i)
            {
                const CBeamDirection & aDirection = aDirections[i].centerPoint();
                calcDiffuseDistribution(aSide, aDirection, i);
            }
        }
    }

    void CBSDFLayer::calc_dir_dif_wv(std::vector<BSDFIntegrator> & results)
    {
        for(Side aSide : allSides())
        {
            const auto & aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);

            size_t size = aDirections.size();
            for(size_t i = 0; i < size; ++i)
            {
                const CBeamDirection & aDirection = aDirections[i].centerPoint();
                calcDiffuseDistribution_wv(aSide, aDirection, i, results);
            }
        }
    }

    void CBSDFLayer::calculate()
    {
        if(!m_Results)
        {
            m_Results.emplace(m_BSDFHemisphere.getDirections(BSDFDirection::Incoming));
        }
        calc_dir_dir();
        calc_dir_dif();
    }

    std::vector<BSDFIntegrator> CBSDFLayer::calculate_wv()
    {
        std::vector<BSDFIntegrator> results(
          m_Cell->getBandSize(), m_BSDFHemisphere.getDirections(BSDFDirection::Incoming));

        calc_dir_dir_wv(results);
        calc_dir_dif_wv(results);

        return results;
    }

    std::shared_ptr<CBaseCell> CBSDFLayer::getCell() const
    {
        return m_Cell;
    }

    bool CBSDFLayer::isEmissivityPolynomialApplicable() const
    {
        return m_EmissivityPolynomialApplicable;
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  diffuse distribution dispatch
    //////////////////////////////////////////////////////////////////////////////////

    void CBSDFLayer::calcDiffuseDistribution(const Side aSide,
                                             const CBeamDirection & t_Direction,
                                             const size_t t_DirectionIndex)
    {
        switch(m_Kind)
        {
            case BSDFLayerKind::UniformDiffuse:
                uniformDiffuse(aSide, t_Direction, t_DirectionIndex);
                return;
            case BSDFLayerKind::Directional:
                directional(aSide, t_Direction, t_DirectionIndex);
                return;
            case BSDFLayerKind::Specular:
            default:
                // No diffuse calculations are necessary for specular layer.
                return;
        }
    }

    void CBSDFLayer::calcDiffuseDistribution_wv(const Side aSide,
                                                const CBeamDirection & t_Direction,
                                                const size_t t_DirectionIndex,
                                                std::vector<BSDFIntegrator> & results)
    {
        switch(m_Kind)
        {
            case BSDFLayerKind::UniformDiffuse:
                uniformDiffuse_wv(aSide, t_Direction, t_DirectionIndex, results);
                return;
            case BSDFLayerKind::Directional:
                directional_wv(aSide, t_Direction, t_DirectionIndex, results);
                return;
            case BSDFLayerKind::Specular:
            default:
                return;
        }
    }

    void CBSDFLayer::calcDiffuseDistribution_byWavelength(const Side aSide,
                                                          const CBeamDirection & t_Direction,
                                                          const size_t t_DirectionIndex,
                                                          size_t wavelengthIndex,
                                                          BSDFIntegrator & results)
    {
        switch(m_Kind)
        {
            case BSDFLayerKind::UniformDiffuse:
                uniformDiffuse_byWavelength(
                  aSide, t_Direction, t_DirectionIndex, wavelengthIndex, results);
                return;
            case BSDFLayerKind::Directional:
                directional_byWavelength(
                  aSide, t_Direction, t_DirectionIndex, wavelengthIndex, results);
                return;
            case BSDFLayerKind::Specular:
            default:
                return;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  UniformDiffuse bodies (formerly CUniformDiffuseBSDFLayer)
    //////////////////////////////////////////////////////////////////////////////////

    void CBSDFLayer::uniformDiffuse(const Side aSide,
                                    const CBeamDirection & t_Direction,
                                    const size_t t_DirectionIndex)
    {
        auto & Tau = m_Results->getMatrix(aSide, PropertySurface::T);
        auto & Rho = m_Results->getMatrix(aSide, PropertySurface::R);

        const double aTau = m_Cell->T_dir_dif(aSide, t_Direction);
        const double Ref = m_Cell->R_dir_dif(aSide, t_Direction);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        const size_t size = aDirections.size();

        for(size_t j = 0; j < size; ++j)
        {
            using ConstantsData::WCE_PI;

            Tau(j, t_DirectionIndex) += aTau / WCE_PI;
            Rho(j, t_DirectionIndex) += Ref / WCE_PI;
        }
    }

    void CBSDFLayer::uniformDiffuse_wv(const Side aSide,
                                       const CBeamDirection & t_Direction,
                                       const size_t t_DirectionIndex,
                                       std::vector<BSDFIntegrator> & results)
    {
        std::vector<double> aTau = m_Cell->T_dir_dif_band(aSide, t_Direction);
        std::vector<double> Ref = m_Cell->R_dir_dif_band(aSide, t_Direction);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        const size_t size = aDirections.size();

        for(size_t i = 0; i < size; ++i)
        {
            const size_t numWV = aTau.size();
            for(size_t j = 0; j < numWV; ++j)
            {
                using ConstantsData::WCE_PI;

                auto & tau = results[j].getMatrix(aSide, PropertySurface::T);
                auto & rho = results[j].getMatrix(aSide, PropertySurface::R);
                tau(i, t_DirectionIndex) += aTau[j] / WCE_PI;
                rho(i, t_DirectionIndex) += Ref[j] / WCE_PI;
            }
        }
    }

    void CBSDFLayer::uniformDiffuse_byWavelength(const Side aSide,
                                                 const CBeamDirection & t_Direction,
                                                 const size_t t_DirectionIndex,
                                                 size_t wavelengthIndex,
                                                 BSDFIntegrator & results)
    {
        const auto aTau = m_Cell->T_dir_dif_at_wavelength(aSide, t_Direction, wavelengthIndex);
        const auto Ref = m_Cell->R_dir_dif_at_wavelength(aSide, t_Direction, wavelengthIndex);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        const size_t size = aDirections.size();

        for(size_t i = 0; i < size; ++i)
        {
            using ConstantsData::WCE_PI;

            auto & tau = results.getMatrix(aSide, PropertySurface::T);
            auto & rho = results.getMatrix(aSide, PropertySurface::R);
            tau(i, t_DirectionIndex) += aTau / WCE_PI;
            rho(i, t_DirectionIndex) += Ref / WCE_PI;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  Directional bodies (formerly CDirectionalBSDFLayer)
    //////////////////////////////////////////////////////////////////////////////////

    void CBSDFLayer::directional(const Side side,
                                 const CBeamDirection & inDir,
                                 const size_t inIdx)
    {
        auto & tau = m_Results->getMatrix(side, PropertySurface::T);
        auto & rho = m_Results->getMatrix(side, PropertySurface::R);

        for_each_outgoing_(inIdx, [&](size_t out, const CBeamDirection & oDir, double s) {
            const double T = m_Cell->T_dir_dif(side, inDir, oDir);
            const double R = m_Cell->R_dir_dif(side, inDir, oDir);
            tau(out, inIdx) += T * s;
            rho(out, inIdx) += R * s;
        });
    }

    void CBSDFLayer::directional_wv(const Side side,
                                    const CBeamDirection & inDir,
                                    const size_t inIdx,
                                    std::vector<BSDFIntegrator> & results)
    {
        for_each_outgoing_(inIdx, [&](size_t out, const CBeamDirection & oDir, double s) {
            auto t = m_Cell->T_dir_dif_band(side, inDir, oDir);
            auto r = m_Cell->R_dir_dif_band(side, inDir, oDir);
            const size_t N = t.size();
            for(size_t j = 0; j < N; ++j)
            {
                auto & tau = results[j].getMatrix(side, PropertySurface::T);
                auto & rho = results[j].getMatrix(side, PropertySurface::R);
                tau(out, inIdx) += t[j] * s;
                rho(out, inIdx) += r[j] * s;
            }
        });
    }

    void CBSDFLayer::directional_byWavelength(const Side side,
                                              const CBeamDirection & inDir,
                                              const size_t inIdx,
                                              size_t wavelengthIndex,
                                              BSDFIntegrator & res)
    {
        auto & tau = res.getMatrix(side, PropertySurface::T);
        auto & rho = res.getMatrix(side, PropertySurface::R);

        for_each_outgoing_(inIdx, [&](size_t out, const CBeamDirection & oDir, double s) {
            const double T = m_Cell->T_dir_dif_by_wavelength(side, inDir, oDir, wavelengthIndex);
            const double R = m_Cell->R_dir_dif_by_wavelength(side, inDir, oDir, wavelengthIndex);
            tau(out, inIdx) += T * s;
            rho(out, inIdx) += R * s;
        });
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  photovoltaic (formerly PhotovoltaicSpecularBSDFLayer)
    //////////////////////////////////////////////////////////////////////////////////

    void CBSDFLayer::assignPowerTable(PVPowerPropertiesTable powerTable)
    {
        const auto material = m_Cell->getMaterial();
        m_PVMaterial = std::dynamic_pointer_cast<CMaterialPhotovoltaicSample>(material);
        if(!m_PVMaterial)
        {
            throw std::runtime_error("Incorrect material for photovoltaic layer.");
        }
        m_PVPowerTable = std::move(powerTable);
    }

    std::vector<std::vector<double>>
      CBSDFLayer::jscPrime(Side t_Side, const std::vector<double> & wavelengths) const
    {
        if(!m_PVMaterial)
        {
            const auto wls = wavelengths.empty() ? getBandWavelengths() : wavelengths;
            const auto innerSize{m_BSDFHemisphere.getDirections(BSDFDirection::Incoming).size()};
            return {wls.size(), std::vector<double>(innerSize, 0)};
        }

        std::lock_guard guard(jscPrimeLock);
        std::vector<std::vector<double>> result;
        auto jscPrime{m_PVMaterial->jscPrime(t_Side)};
        if(jscPrime.size() != wavelengths.size())
        {
            jscPrime = jscPrime.interpolate(wavelengths);
        }
        for(const auto & jsc : jscPrime)
        {
            std::vector<double> curVal;

            // No angular dependence for jsc for now. Every direction will have the same
            // electricity generation.
            for(size_t i = 0u; i < m_BSDFHemisphere.getDirections(BSDFDirection::Incoming).size();
                ++i)
            {
                curVal.push_back(jsc.value());
            }
            result.push_back(curVal);
        }
        return result;
    }

    std::vector<double> CBSDFLayer::voc(const std::vector<double> & electricalCurrent) const
    {
        if(!m_PVMaterial)
        {
            return std::vector<double>(electricalCurrent.size(), 0);
        }
        std::vector<double> result;
        result.reserve(electricalCurrent.size());
        for(const auto & cur : electricalCurrent)
        {
            result.push_back(m_PVPowerTable.voc(cur));
        }
        return result;
    }

    std::vector<double> CBSDFLayer::ff(const std::vector<double> & electricalCurrent) const
    {
        if(!m_PVMaterial)
        {
            return std::vector<double>(electricalCurrent.size(), 0);
        }
        std::vector<double> result;
        result.reserve(electricalCurrent.size());
        for(const auto & cur : electricalCurrent)
        {
            result.push_back(m_PVPowerTable.ff(cur));
        }
        return result;
    }
}   // namespace SingleLayerOptics
