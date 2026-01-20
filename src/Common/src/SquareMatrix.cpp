#include <stdexcept>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "SquareMatrix.hpp"

namespace FenestrationCommon
{
    SquareMatrix::SquareMatrix(const std::size_t tSize) :
        m_size(tSize), m_Matrix(tSize * tSize, 0.0)
    {}

    SquareMatrix::SquareMatrix(const std::initializer_list<std::vector<double>> & tInput) :
        m_size(tInput.size()), m_Matrix(m_size * m_size, 0.0)
    {
        size_t i = 0;
        for(const auto & vec : tInput)
        {
            for(size_t j = 0; j < vec.size(); ++j)
            {
                m_Matrix[i * m_size + j] = vec[j];
            }
            ++i;
        }
    }

    SquareMatrix::SquareMatrix(const std::vector<std::vector<double>> & tInput) :
        m_size(tInput.size()), m_Matrix(m_size * m_size)
    {
        for(size_t i = 0; i < m_size; ++i)
        {
            for(size_t j = 0; j < m_size; ++j)
            {
                m_Matrix[i * m_size + j] = tInput[i][j];
            }
        }
    }

    SquareMatrix::SquareMatrix(std::vector<std::vector<double>> && tInput) :
        m_size(tInput.size()), m_Matrix(m_size * m_size)
    {
        for(size_t i = 0; i < m_size; ++i)
        {
            for(size_t j = 0; j < m_size; ++j)
            {
                m_Matrix[i * m_size + j] = tInput[i][j];
            }
        }
    }

    std::size_t SquareMatrix::size() const
    {
        return m_size;
    }

    void SquareMatrix::setZeros()
    {
        std::fill(m_Matrix.begin(), m_Matrix.end(), 0.0);
    }

    void SquareMatrix::setIdentity()
    {
        setZeros();
        for(size_t i = 0; i < m_size; ++i)
        {
            m_Matrix[i * m_size + i] = 1.0;
        }
    }

    void SquareMatrix::setDiagonal(const std::vector<double> & tInput)
    {
        if(tInput.size() != m_size)
        {
            throw std::runtime_error("Matrix and vector must be same size.");
        }

        for(size_t i = 0; i < m_size; ++i)
        {
            m_Matrix[i * m_size + i] = tInput[i];
        }
    }

    SquareMatrix SquareMatrix::inverse() const
    {
        const size_t n = m_size;
        SquareMatrix inv(n);

        // 1) Factor once
        const auto LU = this->LU();

        // 2) Scratch once (avoid reallocs per column)
        std::vector<double> y(n), d(n);

        // 3) Solve A * X = I, one RHS at a time (identity columns)
        for(size_t col = 0; col < n; ++col)
        {
            // d := e_col
            std::fill(d.begin(), d.end(), 0.0);
            d[col] = 1.0;

            // Forward: L * y = d (unit diagonal L)
            for(size_t i = 0; i < n; ++i)
            {
                double s = d[i];
                for(size_t j = 0; j < i; ++j)
                    s -= LU(i, j) * y[j];
                y[i] = s;
            }

            // Back: U * x = y
            for(int i = int(n) - 1; i >= 0; --i)
            {
                double s = y[size_t(i)];
                for(size_t j = size_t(i) + 1; j < n; ++j)
                    s -= LU(size_t(i), j) * inv(j, col);
                inv(size_t(i), col) = s / LU(size_t(i), size_t(i));
            }
        }
        return inv;
    }

    double SquareMatrix::operator()(const std::size_t i, const std::size_t j) const
    {
        return m_Matrix[i * m_size + j];
    }

    double & SquareMatrix::operator()(const std::size_t i, const std::size_t j)
    {
        return m_Matrix[i * m_size + j];
    }

    SquareMatrix SquareMatrix::LU() const
    {
        SquareMatrix D(*this);

        for(size_t k = 0; k <= m_size - 2; ++k)
        {
            for(size_t j = k + 1; j <= m_size - 1; ++j)
            {
                const auto x = D(j, k) / D(k, k);
                for(size_t i = k; i <= m_size - 1; ++i)
                {
                    D(j, i) = D(j, i) - x * D(k, i);
                }
                D(j, k) = x;
            }
        }

        return D;
    }

    std::vector<double> SquareMatrix::checkSingularity() const
    {
        std::vector<double> vv;
        vv.reserve(m_size);

        for(size_t i = 0; i < m_size; ++i)
        {
            double aamax = 0.0;
            for(size_t j = 0; j < m_size; ++j)
            {
                const double absCellValue = std::abs(m_Matrix[i * m_size + j]);
                if(absCellValue > aamax)
                {
                    aamax = absCellValue;
                }
            }
            if(aamax == 0)
            {
                assert(aamax != 0);
            }
            vv.push_back(1.0 / aamax);
        }

        return vv;
    }

    std::vector<size_t> SquareMatrix::makeUpperTriangular()
    {
        const double TINY = 1e-20;

        std::vector<size_t> index(m_size);
        std::vector<double> vv = checkSingularity();

        for(size_t j = 0; j < m_size; ++j)
        {
            for(int i = 0; i <= int(j) - 1; ++i)
            {
                double sum = m_Matrix[i * m_size + j];
                for(int k = 0; k <= i - 1; ++k)
                {
                    sum = sum - m_Matrix[i * m_size + k] * m_Matrix[k * m_size + j];
                }
                m_Matrix[i * m_size + j] = sum;
            }

            double aamax = 0.0;
            size_t imax = j;

            for(size_t i = j; i < m_size; ++i)
            {
                double sum = m_Matrix[i * m_size + j];
                for(int k = 0; k <= int(j) - 1; ++k)
                {
                    sum = sum - m_Matrix[i * m_size + k] * m_Matrix[k * m_size + j];
                }
                m_Matrix[i * m_size + j] = sum;
                const double dum = vv[i] * std::abs(sum);
                if(dum >= aamax)
                {
                    imax = i;
                    aamax = dum;
                }
            }

            if(j != imax)
            {
                // Swap rows j and imax
                for(size_t k = 0; k < m_size; ++k)
                {
                    const double dum = m_Matrix[imax * m_size + k];
                    m_Matrix[imax * m_size + k] = m_Matrix[j * m_size + k];
                    m_Matrix[j * m_size + k] = dum;
                }
                vv[imax] = vv[j];
            }
            index[j] = imax;
            if(m_Matrix[j * m_size + j] == 0.0)
            {
                m_Matrix[j * m_size + j] = TINY;
            }
            if(j != (m_size - 1))
            {
                const double dum = 1.0 / m_Matrix[j * m_size + j];
                for(size_t i = j + 1; i < m_size; ++i)
                {
                    m_Matrix[i * m_size + j] = m_Matrix[i * m_size + j] * dum;
                }
            }
        }

        return index;
    }

    SquareMatrix SquareMatrix::mmultRows(const std::vector<double> & tInput)
    {
        if(m_size != tInput.size())
        {
            throw std::runtime_error("Vector and matrix do not have same size.");
        }

        SquareMatrix res{m_size};
        for(size_t i = 0; i < m_size; ++i)
        {
            for(size_t j = 0; j < m_size; ++j)
            {
                res(j, i) = m_Matrix[j * m_size + i] * tInput[i];
            }
        }

        return res;
    }

    std::vector<std::vector<double>> SquareMatrix::getMatrix() const
    {
        std::vector<std::vector<double>> result(m_size, std::vector<double>(m_size));
        for(size_t i = 0; i < m_size; ++i)
        {
            for(size_t j = 0; j < m_size; ++j)
            {
                result[i][j] = m_Matrix[i * m_size + j];
            }
        }
        return result;
    }

    SquareMatrix SquareMatrix::operator*(const SquareMatrix & rhs) const
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        SquareMatrix out{m_size};

        const size_t N = m_size;
        for(size_t i = 0; i < N; ++i)
        {
            const size_t iN = i * N;
            for(size_t k = 0; k < N; ++k)
            {
                const double aik = m_Matrix[iN + k];
                const size_t kN = k * N;
                for(size_t j = 0; j < N; ++j)
                {
                    out.m_Matrix[iN + j] += aik * rhs.m_Matrix[kN + j];
                }
            }
        }
        return out;
    }

    SquareMatrix & SquareMatrix::operator*=(const SquareMatrix & rhs)
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        SquareMatrix out{m_size};

        const size_t N = m_size;
        for(size_t i = 0; i < N; ++i)
        {
            const size_t iN = i * N;
            for(size_t k = 0; k < N; ++k)
            {
                const double aik = m_Matrix[iN + k];
                const size_t kN = k * N;
                for(size_t j = 0; j < N; ++j)
                {
                    out.m_Matrix[iN + j] += aik * rhs.m_Matrix[kN + j];
                }
            }
        }
        m_Matrix.swap(out.m_Matrix);
        return *this;
    }

    SquareMatrix SquareMatrix::operator+(const SquareMatrix & rhs) const
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        SquareMatrix out{m_size};
        const size_t total = m_size * m_size;
        for(size_t i = 0; i < total; ++i)
        {
            out.m_Matrix[i] = m_Matrix[i] + rhs.m_Matrix[i];
        }
        return out;
    }

    SquareMatrix & SquareMatrix::operator+=(const SquareMatrix & rhs)
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        const size_t total = m_size * m_size;
        for(size_t i = 0; i < total; ++i)
        {
            m_Matrix[i] += rhs.m_Matrix[i];
        }
        return *this;
    }

    SquareMatrix SquareMatrix::operator-(const SquareMatrix & rhs) const
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        SquareMatrix out{m_size};
        const size_t total = m_size * m_size;
        for(size_t i = 0; i < total; ++i)
        {
            out.m_Matrix[i] = m_Matrix[i] - rhs.m_Matrix[i];
        }
        return out;
    }

    SquareMatrix & SquareMatrix::operator-=(const SquareMatrix & rhs)
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        const size_t total = m_size * m_size;
        for(size_t i = 0; i < total; ++i)
        {
            m_Matrix[i] -= rhs.m_Matrix[i];
        }
        return *this;
    }

    std::vector<double> SquareMatrix::operator*(const std::vector<double> & v) const
    {
        if(v.size() != m_size)
        {
            throw std::runtime_error("Vector and matrix do not have same size.");
        }

        std::vector<double> y(m_size, 0.0);
        for(size_t i = 0; i < m_size; ++i)
        {
            const size_t iN = i * m_size;
            double sum = 0.0;
            for(size_t j = 0; j < m_size; ++j)
            {
                sum += m_Matrix[iN + j] * v[j];
            }
            y[i] = sum;
        }
        return y;
    }

    std::vector<double> operator*(const std::vector<double> & first, const SquareMatrix & second)
    {
        if(first.size() != second.size())
        {
            throw std::runtime_error("Vector and matrix do not have same size.");
        }

        std::vector<double> res(first.size(), 0.0);

        for(size_t i = 0; i < first.size(); ++i)
        {
            for(size_t j = 0; j < first.size(); ++j)
            {
                res[i] += first[j] * second(j, i);
            }
        }

        return res;
    }

    std::vector<double> operator*(const SquareMatrix & first, const std::vector<double> & second)
    {
        if(first.size() != second.size())
        {
            throw std::runtime_error("Vector and matrix do not have same size.");
        }

        std::vector<double> res(second.size(), 0.0);

        for(size_t i = 0; i < second.size(); ++i)
        {
            for(size_t j = 0; j < second.size(); ++j)
            {
                res[i] += second[j] * first(i, j);
            }
        }

        return res;
    }

    SquareMatrix multiplyWithDiagonalMatrix(const std::vector<double> & tInput,
                                            const SquareMatrix & tMatrix)
    {
        SquareMatrix res{tInput.size()};
        for(size_t i = 0; i < tInput.size(); ++i)
        {
            for(size_t j = 0; j < tInput.size(); ++j)
            {
                res(i, j) = tMatrix(i, j) * tInput[i];
            }
        }
        return res;
    }

    SquareMatrix multiplyWithDiagonalMatrix(const SquareMatrix & tMatrix,
                                            const std::vector<double> & tInput)
    {
        SquareMatrix res{tInput.size()};
        for(size_t i = 0; i < tInput.size(); ++i)
        {
            for(size_t j = 0; j < tInput.size(); ++j)
            {
                res(i, j) = tMatrix(i, j) * tInput[j];
            }
        }
        return res;
    }


}   // namespace FenestrationCommon
