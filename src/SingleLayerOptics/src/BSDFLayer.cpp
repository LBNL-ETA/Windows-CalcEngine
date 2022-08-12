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
        m_Calculated(false),
        m_CalculatedWV(false)
    {
        // TODO: Maybe to refactor results to incoming and outgoing if not affecting speed.
        // This is not necessary before axisymmetry is introduced
    }

    void CBSDFLayer::setSourceData(CSeries & t_SourceData)
    {
        m_Cell->setSourceData(t_SourceData);
        m_Calculated = false;
        m_CalculatedWV = false;
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
        if(!m_CalculatedWV)
        {
            calculate_wv();
            m_CalculatedWV = true;
        }
        return m_WVResults;
    }

    BSDFIntegrator CBSDFLayer::getResultsAtWavelength(size_t wavelengthIndex)
    {
        BSDFIntegrator results{m_BSDFHemisphere.getDirections(BSDFDirection::Incoming)};
        calculate_dir_dir_wl(wavelengthIndex, results);
        return results;
    }

    void CBSDFLayer::calculate_dir_dir_wl(size_t wavelengthIndex, BSDFIntegrator & results)
    {
        for(Side aSide : EnumSide())
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

                auto & tau = results.getMatrix(aSide, PropertySimple::T);
                auto & rho = results.getMatrix(aSide, PropertySimple::R);
                tau(i, i) += aTau / Lambda;
                rho(i, i) += aRho / Lambda;
            }
        }
    }

    void CBSDFLayer::calculate_dir_dif_wv(size_t wavelengthIndex, BSDFIntegrator & results)
    {
        for(Side aSide : EnumSide())
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
        m_CalculatedWV = false;
    }

    void CBSDFLayer::calc_dir_dir()
    {
        for(Side t_Side : EnumSide())
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

    void CBSDFLayer::calc_dir_dir_wv()
    {
        for(Side aSide : EnumSide())
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
                    auto & tau = m_WVResults[j].getMatrix(aSide, PropertySimple::T);
                    auto & rho = m_WVResults[j].getMatrix(aSide, PropertySimple::R);
                    tau(i, i) += aTau[j] / Lambda;
                    rho(i, i) += aRho[j] / Lambda;
                }
            }
        }
    }

    void CBSDFLayer::calc_dir_dif()
    {
        for(Side aSide : EnumSide())
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

    void CBSDFLayer::calc_dir_dif_wv()
    {
        for(Side aSide : EnumSide())
        {
            const auto & aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);

            size_t size = aDirections.size();
            for(size_t i = 0; i < size; ++i)
            {
                const CBeamDirection aDirection = aDirections[i].centerPoint();
                calcDiffuseDistribution_wv(aSide, aDirection, i);
            }
        }
    }

    void CBSDFLayer::fillWLResultsFromMaterialCell()
    {
        m_WVResults.clear();
        size_t size = m_Cell->getBandSize();
        for(size_t i = 0; i < size; ++i)
        {
            m_WVResults.emplace_back(m_BSDFHemisphere.getDirections(BSDFDirection::Incoming));
        }
    }

    void CBSDFLayer::calculate()
    {
        fillWLResultsFromMaterialCell();
        calc_dir_dir();
        calc_dir_dif();
    }

    void CBSDFLayer::calculate_wv()
    {
        fillWLResultsFromMaterialCell();
        calc_dir_dir_wv();
        calc_dir_dif_wv();
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
