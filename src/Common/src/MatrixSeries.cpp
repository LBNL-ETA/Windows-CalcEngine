#include <cassert>
#include <stdexcept>

#include <algorithm>

#include <WCECommon.hpp>

#include "MatrixSeries.hpp"
#include "SquareMatrix.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "Utility.hpp"


namespace FenestrationCommon
{
    CMatrixSeries::CMatrixSeries(const size_t t_Size1, const size_t t_Size2, size_t seriesSize) :
        m_Size1(t_Size1), m_Size2(t_Size2)
    {
        m_Matrix = std::vector<std::vector<CSeries>>(m_Size1);
        for(size_t i = 0; i < m_Size1; ++i)
        {
            m_Matrix[i].resize(m_Size2);
            for(size_t j = 0; j < m_Size2; ++j)
            {
                m_Matrix[i][j] = CSeries(seriesSize);
            }
        }
    }

    CMatrixSeries::CMatrixSeries(const CMatrixSeries & t_MatrixSeries)
    {
        *this = t_MatrixSeries;
    }

    CMatrixSeries & CMatrixSeries::operator=(CMatrixSeries const & t_MatrixSeries)
    {
        m_Size1 = t_MatrixSeries.m_Size1;
        m_Size2 = t_MatrixSeries.m_Size2;
        m_Matrix = std::vector<std::vector<CSeries>>(m_Size1);
        for(size_t i = 0; i < m_Size1; ++i)
        {
            m_Matrix[i].resize(m_Size2);
            for(size_t j = 0; j < m_Size2; ++j)
            {
                m_Matrix[i][j] = CSeries(t_MatrixSeries.m_Matrix[i][j]);
            }
        }
        return *this;
    }

    void CMatrixSeries::addProperty(const size_t i,
                                    const size_t j,
                                    const double t_Wavelength,
                                    const double t_Value)
    {
        m_Matrix[i][j].addProperty(t_Wavelength, t_Value);
    }

    void CMatrixSeries::addProperties(const size_t i,
                                      const double t_Wavelength,
                                      const std::vector<double> & t_Values)
    {
        for(size_t j = 0; j < t_Values.size(); ++j)
        {
            m_Matrix[i][j].addProperty(t_Wavelength, t_Values[j]);
        }
    }

    void CMatrixSeries::setPropertiesAtIndex(size_t index,
                                             size_t i,
                                             double t_Wavelength,
                                             const std::vector<double> & t_Values)
    {
        for(size_t j = 0; j < t_Values.size(); ++j)
        {
            m_Matrix[i][j].setPropertyAtIndex(index, t_Wavelength, t_Values[j]);
        }
    }

    void CMatrixSeries::addProperties(const double t_Wavelength, const SquareMatrix & t_Matrix)
    {
        for(size_t i = 0; i < m_Matrix.size(); ++i)
        {
            assert(m_Matrix.size() == t_Matrix.size());
            for(size_t j = 0; j < m_Matrix[i].size(); ++j)
            {
                m_Matrix[i][j].addProperty(t_Wavelength, t_Matrix(i, j));
            }
        }
    }

    void CMatrixSeries::setPropertiesAtIndex(size_t index,
                                             double t_Wavelength,
                                             const SquareMatrix & t_Matrix)
    {
        for(size_t i = 0; i < m_Matrix.size(); ++i)
        {
            assert(m_Matrix.size() == t_Matrix.size());
            for(size_t j = 0; j < m_Matrix[i].size(); ++j)
            {
                m_Matrix[i][j].setPropertyAtIndex(index, t_Wavelength, t_Matrix(i, j));
            }
        }
    }

    void CMatrixSeries::addSeries(const size_t i, const size_t j, const CSeries & series)
    {
        m_Matrix[i][j] = series;
    }

    void CMatrixSeries::mMult(const CSeries & t_Series)
    {
        // Parallelization here did not show any improvements. Program was performing even slower.
        for(size_t i = 0; i < m_Matrix.size(); ++i)
        {
            for(size_t j = 0; j < m_Matrix[i].size(); ++j)
            {
                assert(t_Series.size() == m_Matrix[i][j].size());
                m_Matrix[i][j] = m_Matrix[i][j] * t_Series;
            }
        }
    }

    void CMatrixSeries::mMult(const std::vector<CSeries> & t_Series)
    {
        const size_t n = m_Matrix.size();

        FenestrationCommon::executeInParallel<size_t>(0u, n - 1u, [this, &t_Series](size_t i) {
            std::transform(begin(m_Matrix[i]),
                           end(m_Matrix[i]),
                           begin(m_Matrix[i]),
                           [&](const CSeries & elem) { return elem * t_Series[i]; });
        });
    }

    std::vector<CSeries> & CMatrixSeries::operator[](const size_t index)
    {
        return m_Matrix[index];
    }

    namespace
    {
        template<typename Function>
        void parallelProcess(const std::vector<std::vector<CSeries>> & matrix, Function && func)
        {
            FenestrationCommon::executeInParallel<size_t>(0, matrix.size() - 1, [&](size_t i) {
                const auto & row = matrix[i];
                for(size_t j = 0; j < row.size(); ++j)
                {
                    func(i, j);
                }
            });
        }
    }   // namespace

    void CMatrixSeries::integrate(const IntegrationType t_Integration,
                                  double normalizationCoefficient,
                                  const std::optional<std::vector<double>> & integrationPoints)
    {
        parallelProcess(m_Matrix, [&](size_t i, size_t j) {
            m_Matrix[i][j] =
              m_Matrix[i][j].integrate(t_Integration, normalizationCoefficient, integrationPoints);
        });
    }

    void CMatrixSeries::interpolate(const std::vector<double> & t_Wavelengths)
    {
        parallelProcess(m_Matrix, [&](size_t i, size_t j) {
            m_Matrix[i][j] = m_Matrix[i][j].interpolate(t_Wavelengths);
        });
    }

    std::vector<std::vector<double>>
      CMatrixSeries::getSums(const double minLambda,
                             const double maxLambda,
                             const std::vector<double> & t_ScaleValue) const
    {
        std::vector<std::vector<double>> Result(m_Matrix.size());
        for(size_t i = 0; i < m_Matrix.size(); ++i)
        {
            if(m_Matrix[i].size() != t_ScaleValue.size())
            {
                throw std::runtime_error(
                  "Size of vector for scaling must be same as size of the matrix.");
            }

            for(size_t j = 0; j < m_Matrix[i].size(); ++j)
            {
                Result[i].push_back(m_Matrix[i][j].sum(minLambda, maxLambda) / t_ScaleValue[i]);
            }
        }
        return Result;
    }

    std::vector<std::vector<double>> CMatrixSeries::getSums(const double minLambda,
                                                            const double maxLambda) const
    {
        return getSums(minLambda, maxLambda, std::vector<double>(m_Matrix[0].size(), 1));
    }

    SquareMatrix CMatrixSeries::getSquaredMatrixSums(const double minLambda,
                                                     const double maxLambda,
                                                     const std::vector<double> & t_ScaleValue)
    {
        assert(m_Matrix.size() == m_Matrix[0].size());
        SquareMatrix Res(m_Matrix.size());
        for(size_t i = 0; i < m_Matrix.size(); ++i)
        {
            for(size_t j = 0; j < m_Matrix[i].size(); ++j)
            {
                Res(i, j) = m_Matrix[i][j].sum(minLambda, maxLambda) / t_ScaleValue[i];
            }
        }
        return Res;
    }

    size_t CMatrixSeries::size1() const
    {
        return m_Size1;
    }

    size_t CMatrixSeries::size2() const
    {
        return m_Size2;
    }

}   // namespace FenestrationCommon
