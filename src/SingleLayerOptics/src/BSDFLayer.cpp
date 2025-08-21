#include "BSDFLayer.hpp"
#include "BaseCell.hpp"
#include "BSDFDirections.hpp"
#include "BSDFIntegrator.hpp"
#include "MaterialDescription.hpp"
#include "BSDFPatch.hpp"
#include "WCECommon.hpp"
#include "BeamDirection.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CBSDFLayer::CBSDFLayer(const std::shared_ptr<CBaseCell> & t_Cell,
                           const BSDFHemisphere & t_Hemisphere) :
        m_BSDFHemisphere(t_Hemisphere),
        m_Cell(t_Cell),
        m_Results(m_BSDFHemisphere.getDirections(BSDFDirection::Incoming)),
        m_Calculated(false)
    {
        // TODO: Maybe to refactor results to incoming and outgoing if not affecting speed.
        // This is not necessary before axisymmetry is introduced
    }

    void CBSDFLayer::setSourceData(const CSeries & t_SourceData)
    {
        m_Cell->setSourceData(t_SourceData);
        m_Calculated = false;
    }

    const BSDFDirections & CBSDFLayer::getDirections(const BSDFDirection t_Side) const
    {
        return m_BSDFHemisphere.getDirections(t_Side);
    }

    BSDFIntegrator CBSDFLayer::getResults()
    {
        if(!m_Calculated)
        {
            calculate();
            m_Calculated = true;
        }
        return m_Results;
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

    void CBSDFLayer::calculate_dir_dir_wl(size_t wavelengthIndex, BSDFIntegrator & results)
    {
        for(Side aSide : allSides())
        {
            const auto & aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
            size_t size = aDirections.size();
            for(size_t i = 0; i < size; ++i)
            {
                const CBeamDirection aDirection = aDirections[i].centerPoint();
                const auto aTau =
                  m_Cell->T_dir_dir_at_wavelength(aSide, aDirection, wavelengthIndex);
                const auto aRho =
                  m_Cell->R_dir_dir_at_wavelength(aSide, aDirection, wavelengthIndex);
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
                const CBeamDirection aDirection = aDirections[directionIndex].centerPoint();
                calcDiffuseDistribution_byWavelength(aSide, aDirection, directionIndex, wavelengthIndex, results);
            }
        }
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
                const CBeamDirection aDirection = aDirections[i].centerPoint();
                const double Lambda = aDirections[i].lambda();

                const double aTau = m_Cell->T_dir_dir(t_Side, aDirection);
                const double aRho = m_Cell->R_dir_dir(t_Side, aDirection);

                tau(i, i) += aTau / Lambda;
                rho(i, i) += aRho / Lambda;
            }
            m_Results.setMatrices(tau, rho, t_Side);
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
                const CBeamDirection aDirection = aDirections[i].centerPoint();
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
                const CBeamDirection aDirection = aDirections[i].centerPoint();
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
                const CBeamDirection aDirection = aDirections[i].centerPoint();
                calcDiffuseDistribution_wv(aSide, aDirection, i, results);
            }
        }
    }

    void CBSDFLayer::calculate()
    {
        calc_dir_dir();
        calc_dir_dif();
    }

    std::vector<BSDFIntegrator> CBSDFLayer::calculate_wv()
    {
        std::vector<BSDFIntegrator> results(m_Cell->getBandSize(), m_BSDFHemisphere.getDirections(BSDFDirection::Incoming));

        calc_dir_dir_wv(results);
        calc_dir_dif_wv(results);

        return results;
    }

    std::shared_ptr<CBaseCell> CBSDFLayer::getCell() const
    {
        return m_Cell;
    }

    std::vector<std::vector<double>>
      CBSDFLayer::jscPrime(Side, const std::vector<double> & wavelengths) const
    {
        const auto wls = wavelengths.empty() ? getBandWavelengths() : wavelengths;
        const auto innerSize{m_BSDFHemisphere.getDirections(BSDFDirection::Incoming).size()};

        return {wls.size(), std::vector<double>(innerSize, 0)};
    }

    std::vector<double> CBSDFLayer::voc(const std::vector<double> & electricalCurrent) const
    {
        return std::vector<double>(electricalCurrent.size(), 0);
    }

    std::vector<double> CBSDFLayer::ff(const std::vector<double> & electricalCurrent) const
    {
        return std::vector<double>(electricalCurrent.size(), 0);
    }
}   // namespace SingleLayerOptics
