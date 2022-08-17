#include "BSDFIntegrator.hpp"
#include "BSDFDirections.hpp"
#include "BSDFPatch.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    BSDFIntegrator::BSDFIntegrator(const BSDFDirections & t_Directions) :
        m_Directions(t_Directions),
        m_DimMatrices(m_Directions.size()),
        m_DirectHemisphericalCalculated(false),
        m_DiffuseDiffuseCalculated(false)
    {
        for(auto t_Side : EnumSide())
        {
            for(auto t_Property : EnumPropertySimple())
            {
                m_Matrix[std::make_pair(t_Side, t_Property)] = SquareMatrix(m_DimMatrices);
                m_DirectHemispherical[std::make_pair(t_Side, t_Property)] = std::vector<double>(m_DimMatrices);
            }
        }
    }

    double BSDFIntegrator::DiffDiff(const Side t_Side, const PropertySimple t_Property)
    {
        calcDiffuseDiffuse();
        return m_DiffDiff.at(t_Side, t_Property);
    }

    SquareMatrix & BSDFIntegrator::getMatrix(const Side t_Side, const PropertySimple t_Property)
    {
        return m_Matrix[std::make_pair(t_Side, t_Property)];
    }

    const FenestrationCommon::SquareMatrix &
      BSDFIntegrator::at(const FenestrationCommon::Side t_Side,
                         const FenestrationCommon::PropertySimple t_Property) const
    {
        return m_Matrix.at(std::make_pair(t_Side, t_Property));
    }

    void BSDFIntegrator::setMatrices(const SquareMatrix & t_Tau,
                                     const SquareMatrix & t_Rho,
                                     Side t_Side)
    {
        m_Matrix[std::make_pair(t_Side, PropertySimple::T)] = t_Tau;
        m_Matrix[std::make_pair(t_Side, PropertySimple::R)] = t_Rho;
    }

    double BSDFIntegrator::DirDir(const Side t_Side,
                                  const PropertySimple t_Property,
                                  const double t_Theta,
                                  const double t_Phi) const
    {
        const auto index = m_Directions.getNearestBeamIndex(t_Theta, t_Phi);
        const auto lambda = m_Directions.lambdaVector()[index];
        const auto tau = at(t_Side, t_Property)(index, index);
        return tau * lambda;
    }

    double BSDFIntegrator::DirDir(const Side t_Side,
                                  const PropertySimple t_Property,
                                  const size_t Index) const
    {
        const auto lambda = m_Directions.lambdaVector()[Index];
        const auto tau = at(t_Side, t_Property)(Index, Index);
        return tau * lambda;
    }

    std::vector<double> BSDFIntegrator::DirHem(const FenestrationCommon::Side t_Side,
                                               const FenestrationCommon::PropertySimple t_Property)
    {
        calcHemispherical();
        return m_DirectHemispherical.at(std::make_pair(t_Side, t_Property));
    }

    std::vector<double> BSDFIntegrator::Abs(Side t_Side)
    {
        calcHemispherical();
        return m_Abs.at(t_Side);
    }

    double BSDFIntegrator::DirHem(const Side t_Side,
                                  const PropertySimple t_Property,
                                  const double t_Theta,
                                  const double t_Phi)
    {
        const auto index = m_Directions.getNearestBeamIndex(t_Theta, t_Phi);
        return DirHem(t_Side, t_Property)[index];
    }

    double BSDFIntegrator::Abs(const Side t_Side, const double t_Theta, const double t_Phi)
    {
        const auto index = m_Directions.getNearestBeamIndex(t_Theta, t_Phi);
        return Abs(t_Side)[index];
    }

    double BSDFIntegrator::Abs(const Side t_Side, const size_t Index)
    {
        return Abs(t_Side)[Index];
    }

    std::vector<double> BSDFIntegrator::lambdaVector() const
    {
        return m_Directions.lambdaVector();
    }

    SquareMatrix BSDFIntegrator::lambdaMatrix() const
    {
        return m_Directions.lambdaMatrix();
    }

    double BSDFIntegrator::integrate(const SquareMatrix & t_Matrix) const
    {
        using ConstantsData::WCE_PI;
        double sum = 0;
        for(size_t i = 0; i < m_DimMatrices; ++i)
        {
            for(size_t j = 0; j < m_DimMatrices; ++j)
            {
                sum += t_Matrix(i, j) * m_Directions[i].lambda() * m_Directions[j].lambda();
            }
        }
        return sum / WCE_PI;
    }

    void BSDFIntegrator::calcDiffuseDiffuse()
    {
        if(!m_DiffuseDiffuseCalculated)
        {
            for(auto t_Side : EnumSide())
            {
                for(auto t_PropertySimple : EnumPropertySimple())
                {
                    m_DiffDiff(t_Side, t_PropertySimple) =
                      integrate(getMatrix(t_Side, t_PropertySimple));
                }
            }
            m_DiffuseDiffuseCalculated = true;
        }
    }

    size_t BSDFIntegrator::getNearestBeamIndex(const double t_Theta, const double t_Phi) const
    {
        return m_Directions.getNearestBeamIndex(t_Theta, t_Phi);
    }

    void BSDFIntegrator::calcHemispherical()
    {
        if(!m_DirectHemisphericalCalculated)
        {
            for(Side t_Side : EnumSide())
            {
                for(PropertySimple t_PropertySimple : EnumPropertySimple())
                {
                    m_DirectHemispherical[{t_Side, t_PropertySimple}] =
                      m_Directions.lambdaVector() * m_Matrix.at({t_Side, t_PropertySimple});
                }
                m_Abs[t_Side] = std::vector<double>();
            }

            const auto size = m_DirectHemispherical[{Side::Front, PropertySimple::T}].size();
            for(size_t i = 0; i < size; ++i)
            {
                for(Side t_Side : EnumSide())
                {
                    m_Abs.at(t_Side).push_back(1.0 - m_DirectHemispherical.at({t_Side, PropertySimple::T})[i]
                                               - m_DirectHemispherical.at({t_Side, PropertySimple::R})[i]);
                }
            }
            m_DirectHemisphericalCalculated = true;
        }
    }
    double BSDFIntegrator::AbsDiffDiff(FenestrationCommon::Side t_Side)
    {
        return 1 - DiffDiff(t_Side, PropertySimple::T) - DiffDiff(t_Side, PropertySimple::R);
    }

    void BSDFIntegrator::resetCalculatedResults()
    {
        m_DirectHemisphericalCalculated = false;
        m_DiffuseDiffuseCalculated = false;
    }

}   // namespace SingleLayerOptics
