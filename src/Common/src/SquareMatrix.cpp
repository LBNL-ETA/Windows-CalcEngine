#include <stdexcept>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "SquareMatrix.hpp"

namespace FenestrationCommon
{
    namespace
    {
        // Tile size for cache-blocked GEMM. 64 doubles == 512 bytes per row tile,
        // so a 64x64 block fits comfortably in L1 on every modern CPU.
        constexpr std::size_t kBlock = 64;
    }   // namespace

    SquareMatrix::SquareMatrix(const std::size_t tSize) :
        m_size(tSize), m_Data(tSize * tSize, 0.0)
    {}

    SquareMatrix::SquareMatrix(const std::initializer_list<std::vector<double>> & tInput) :
        m_size(tInput.size()), m_Data(tInput.size() * tInput.size(), 0.0)
    {
        std::size_t i = 0;
        for(const auto & row : tInput)
        {
            // Caller is expected to provide square input; copy whatever fits.
            const std::size_t cols = std::min(row.size(), m_size);
            std::copy(row.begin(), row.begin() + cols, m_Data.begin() + i * m_size);
            ++i;
        }
    }

    SquareMatrix::SquareMatrix(const std::vector<std::vector<double>> & tInput) :
        m_size(tInput.size()), m_Data(tInput.size() * tInput.size(), 0.0)
    {
        for(std::size_t i = 0; i < m_size; ++i)
        {
            const std::size_t cols = std::min(tInput[i].size(), m_size);
            std::copy(tInput[i].begin(), tInput[i].begin() + cols, m_Data.begin() + i * m_size);
        }
    }

    SquareMatrix::SquareMatrix(std::vector<std::vector<double>> && tInput) :
        m_size(tInput.size()), m_Data(tInput.size() * tInput.size(), 0.0)
    {
        for(std::size_t i = 0; i < m_size; ++i)
        {
            const std::size_t cols = std::min(tInput[i].size(), m_size);
            std::copy(tInput[i].begin(), tInput[i].begin() + cols, m_Data.begin() + i * m_size);
        }
    }

    std::size_t SquareMatrix::size() const
    {
        return m_size;
    }

    void SquareMatrix::setZeros()
    {
        std::fill(m_Data.begin(), m_Data.end(), 0.0);
    }

    void SquareMatrix::setIdentity()
    {
        setZeros();
        for(size_t i = 0; i < m_size; ++i)
        {
            m_Data[i * m_size + i] = 1.0;
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
            m_Data[i * m_size + i] = tInput[i];
        }
    }

    SquareMatrix SquareMatrix::inverse() const
    {
        const size_t n = m_size;
        SquareMatrix inv(n);

        // 1) Factor once
        const auto LU = this->LU();
        const double * const lu = LU.m_Data.data();
        double * const invd = inv.m_Data.data();

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
                const double * lu_i = lu + i * n;
                for(size_t j = 0; j < i; ++j)
                    s -= lu_i[j] * y[j];
                y[i] = s;
            }

            // Back: U * x = y
            for(int i = int(n) - 1; i >= 0; --i)
            {
                const size_t ui = size_t(i);
                double s = y[ui];
                const double * lu_i = lu + ui * n;
                for(size_t j = ui + 1; j < n; ++j)
                    s -= lu_i[j] * invd[j * n + col];
                invd[ui * n + col] = s / lu_i[ui];
            }
        }
        return inv;
    }

    double SquareMatrix::operator()(const std::size_t i, const std::size_t j) const
    {
        return m_Data[i * m_size + j];
    }

    double & SquareMatrix::operator()(const std::size_t i, const std::size_t j)
    {
        return m_Data[i * m_size + j];
    }

    SquareMatrix SquareMatrix::LU() const
    {
        SquareMatrix D(*this);
        const size_t n = m_size;
        double * const d = D.m_Data.data();

        for(size_t k = 0; k + 1 < n; ++k)
        {
            const double pivot = d[k * n + k];
            for(size_t j = k + 1; j < n; ++j)
            {
                const double x = d[j * n + k] / pivot;
                double * row_j = d + j * n;
                const double * row_k = d + k * n;
                for(size_t i = k; i < n; ++i)
                {
                    row_j[i] -= x * row_k[i];
                }
                row_j[k] = x;
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
            const double * row = m_Data.data() + i * m_size;
            for(size_t j = 0; j < m_size; ++j)
            {
                const double absCellValue = std::abs(row[j]);
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
        const size_t n = m_size;
        double * const m = m_Data.data();

        std::vector<size_t> index(n);
        std::vector<double> vv = checkSingularity();

        for(size_t j = 0; j < n; ++j)
        {
            for(int i = 0; i <= int(j) - 1; ++i)
            {
                double sum = m[size_t(i) * n + j];
                for(int k = 0; k <= i - 1; ++k)
                {
                    sum -= m[size_t(i) * n + size_t(k)] * m[size_t(k) * n + j];
                }
                m[size_t(i) * n + j] = sum;
            }

            double aamax = 0.0;
            size_t imax = j;

            for(size_t i = j; i < n; ++i)
            {
                double sum = m[i * n + j];
                for(int k = 0; k <= int(j) - 1; ++k)
                {
                    sum -= m[i * n + size_t(k)] * m[size_t(k) * n + j];
                }
                m[i * n + j] = sum;
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
                for(size_t k = 0; k < n; ++k)
                {
                    std::swap(m[imax * n + k], m[j * n + k]);
                }
                vv[imax] = vv[j];
            }
            index[j] = imax;
            if(m[j * n + j] == 0.0)
            {
                m[j * n + j] = TINY;
            }
            if(j != (n - 1))
            {
                const double dum = 1.0 / m[j * n + j];
                for(size_t i = j + 1; i < n; ++i)
                {
                    m[i * n + j] *= dum;
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
                res.m_Data[j * m_size + i] = m_Data[j * m_size + i] * tInput[i];
            }
        }

        return res;
    }

    std::vector<std::vector<double>> SquareMatrix::getMatrix() const
    {
        std::vector<std::vector<double>> out(m_size, std::vector<double>(m_size));
        for(size_t i = 0; i < m_size; ++i)
        {
            std::copy(m_Data.begin() + i * m_size,
                      m_Data.begin() + (i + 1) * m_size,
                      out[i].begin());
        }
        return out;
    }

    SquareMatrix SquareMatrix::operator*(const SquareMatrix & rhs) const
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        const size_t n = m_size;
        SquareMatrix out{n};

        const double * const A = m_Data.data();
        const double * const B = rhs.m_Data.data();
        double * const C = out.m_Data.data();

        // Cache-blocked GEMM with the i-k-j loop ordering. The inner j-loop
        // walks contiguous memory in both B and C, so the compiler can
        // easily auto-vectorise it.
        for(size_t ii = 0; ii < n; ii += kBlock)
        {
            const size_t i_end = std::min(ii + kBlock, n);
            for(size_t kk = 0; kk < n; kk += kBlock)
            {
                const size_t k_end = std::min(kk + kBlock, n);
                for(size_t jj = 0; jj < n; jj += kBlock)
                {
                    const size_t j_end = std::min(jj + kBlock, n);
                    for(size_t i = ii; i < i_end; ++i)
                    {
                        double * const c_row = C + i * n;
                        const double * const a_row = A + i * n;
                        for(size_t k = kk; k < k_end; ++k)
                        {
                            const double aik = a_row[k];
                            const double * const b_row = B + k * n;
                            for(size_t j = jj; j < j_end; ++j)
                            {
                                c_row[j] += aik * b_row[j];
                            }
                        }
                    }
                }
            }
        }
        return out;
    }

    SquareMatrix & SquareMatrix::operator*=(const SquareMatrix & rhs)
    {
        *this = (*this) * rhs;
        return *this;
    }

    SquareMatrix SquareMatrix::operator+(const SquareMatrix & rhs) const
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        SquareMatrix out{m_size};
        const std::size_t total = m_size * m_size;
        for(std::size_t k = 0; k < total; ++k)
        {
            out.m_Data[k] = m_Data[k] + rhs.m_Data[k];
        }
        return out;
    }

    SquareMatrix & SquareMatrix::operator+=(const SquareMatrix & rhs)
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        const std::size_t total = m_size * m_size;
        for(std::size_t k = 0; k < total; ++k)
        {
            m_Data[k] += rhs.m_Data[k];
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
        const std::size_t total = m_size * m_size;
        for(std::size_t k = 0; k < total; ++k)
        {
            out.m_Data[k] = m_Data[k] - rhs.m_Data[k];
        }
        return out;
    }

    SquareMatrix & SquareMatrix::operator-=(const SquareMatrix & rhs)
    {
        if(m_size != rhs.m_size)
        {
            throw std::runtime_error("Matrices must be identical in size.");
        }

        const std::size_t total = m_size * m_size;
        for(std::size_t k = 0; k < total; ++k)
        {
            m_Data[k] -= rhs.m_Data[k];
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
            const double * row = m_Data.data() + i * m_size;
            double sum = 0.0;
            for(size_t j = 0; j < m_size; ++j)
            {
                sum += row[j] * v[j];
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

    //////////////////////////////////////////////////////////////////////////
    // LUFactor
    //////////////////////////////////////////////////////////////////////////

    LUFactor::LUFactor(const SquareMatrix & A) : m_size(A.m_size), m_LU(A.m_Data)
    {
        // Same Doolittle factorisation as SquareMatrix::LU(), no pivoting.
        const std::size_t n = m_size;
        if(n == 0)
        {
            return;
        }
        double * const d = m_LU.data();
        for(std::size_t k = 0; k + 1 < n; ++k)
        {
            const double pivot = d[k * n + k];
            for(std::size_t j = k + 1; j < n; ++j)
            {
                const double x = d[j * n + k] / pivot;
                double * row_j = d + j * n;
                const double * row_k = d + k * n;
                for(std::size_t i = k; i < n; ++i)
                {
                    row_j[i] -= x * row_k[i];
                }
                row_j[k] = x;
            }
        }
    }

    SquareMatrix LUFactor::solveRight(const SquareMatrix & B) const
    {
        if(B.m_size != m_size)
        {
            throw std::runtime_error("LUFactor::solveRight: size mismatch.");
        }

        const std::size_t n = m_size;
        SquareMatrix X(n);

        const double * const lu = m_LU.data();
        const double * const b = B.m_Data.data();
        double * const x = X.m_Data.data();

        // Step 1: solve L * Y = B (L unit-diagonal). Stored in-place into X.
        // Both reads of "previous rows" and writes of "current row" are
        // contiguous in memory, so the inner j-loop auto-vectorises.
        for(std::size_t i = 0; i < n; ++i)
        {
            double * const row_x = x + i * n;
            const double * const row_b = b + i * n;
            for(std::size_t j = 0; j < n; ++j)
            {
                row_x[j] = row_b[j];
            }
            const double * const row_l = lu + i * n;
            for(std::size_t k = 0; k < i; ++k)
            {
                const double lik = row_l[k];
                const double * const row_xk = x + k * n;
                for(std::size_t j = 0; j < n; ++j)
                {
                    row_x[j] -= lik * row_xk[j];
                }
            }
        }

        // Step 2: solve U * X = Y in place (X currently holds Y).
        for(std::size_t ii = n; ii-- > 0;)
        {
            double * const row_x = x + ii * n;
            const double * const row_u = lu + ii * n;
            for(std::size_t k = ii + 1; k < n; ++k)
            {
                const double uik = row_u[k];
                const double * const row_xk = x + k * n;
                for(std::size_t j = 0; j < n; ++j)
                {
                    row_x[j] -= uik * row_xk[j];
                }
            }
            const double inv_uii = 1.0 / row_u[ii];
            for(std::size_t j = 0; j < n; ++j)
            {
                row_x[j] *= inv_uii;
            }
        }

        return X;
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
