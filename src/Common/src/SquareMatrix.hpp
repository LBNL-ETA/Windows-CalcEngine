#pragma once

#include <vector>
#include <cstddef>
#include <new>

namespace FenestrationCommon
{
    // The large row-major matrix buffers below are allocated through mimalloc's
    // per-thread heaps (lock-free under multithreading -- the dominant cost in
    // BSDF matrix math, ~2x faster than the CRT allocator). mimalloc is confined
    // to *these buffers only*: the <mimalloc.h> include lives solely in
    // SquareMatrix.cpp, reached via the two thin wrappers below, so this header
    // carries no mimalloc dependency. We deliberately do NOT override global
    // operator new/delete -- a process-wide override corrupts the heap in MFC/CRT
    // host apps (THERM, WINDOW) where new/delete and malloc/free would otherwise
    // straddle two allocators.
    namespace detail
    {
        void * matrixAlloc(std::size_t bytes) noexcept;
        void matrixFree(void * ptr) noexcept;
    }   // namespace detail

    template<typename ValueT>
    struct MatrixAllocator
    {
        using value_type = ValueT;

        MatrixAllocator() noexcept = default;

        template<typename OtherT>
        MatrixAllocator(const MatrixAllocator<OtherT> &) noexcept
        {}

        ValueT * allocate(const std::size_t num)
        {
            void * const mem = detail::matrixAlloc(num * sizeof(ValueT));
            if(mem == nullptr)
            {
                throw std::bad_alloc{};
            }
            return static_cast<ValueT *>(mem);
        }

        void deallocate(ValueT * const ptr, const std::size_t) noexcept
        {
            detail::matrixFree(ptr);
        }

        template<typename OtherT>
        bool operator==(const MatrixAllocator<OtherT> &) const noexcept
        {
            return true;
        }

        template<typename OtherT>
        bool operator!=(const MatrixAllocator<OtherT> &) const noexcept
        {
            return false;
        }
    };

    // Storage type for the large row-major matrix buffers (see note above).
    using MatrixStorage = std::vector<double, MatrixAllocator<double>>;

    // Works only with double
    class SquareMatrix
    {
    public:
        SquareMatrix(const SquareMatrix &) = default;
        explicit SquareMatrix(std::size_t tSize = 0);
        explicit SquareMatrix(const std::initializer_list<std::vector<double>> & tInput);
        explicit SquareMatrix(const std::vector<std::vector<double>> & tInput);
        explicit SquareMatrix(std::vector<std::vector<double>> && tInput);

        SquareMatrix & operator=(const SquareMatrix & val) = default;

        [[nodiscard]] std::size_t size() const;
        void setZeros();
        void setIdentity();
        void setDiagonal(const std::vector<double> & tInput);

        std::vector<size_t> makeUpperTriangular();

        [[nodiscard]] SquareMatrix inverse() const;

        double operator()(std::size_t i, std::size_t j) const;
        double & operator()(std::size_t i, std::size_t j);

        SquareMatrix mmultRows(const std::vector<double> & tInput);

        [[nodiscard]] std::vector<std::vector<double>> getMatrix() const;

        // Faster member operators (no friends needed)
        SquareMatrix operator*(const SquareMatrix & rhs) const;
        SquareMatrix & operator*=(const SquareMatrix & rhs);

        SquareMatrix operator+(const SquareMatrix & rhs) const;
        SquareMatrix & operator+=(const SquareMatrix & rhs);

        SquareMatrix operator-(const SquareMatrix & rhs) const;
        SquareMatrix & operator-=(const SquareMatrix & rhs);

        // Matrix–vector (right): M * v
        std::vector<double> operator*(const std::vector<double> & v) const;

    private:
        [[nodiscard]] SquareMatrix LU() const;
        [[nodiscard]] std::vector<double> checkSingularity() const;

        // Row-major flat storage. Element (i, j) lives at m_Data[i * m_size + j].
        // Replaces vector<vector<double>> for cache locality and faster GEMM.
        std::size_t m_size;
        MatrixStorage m_Data;

        friend class LUFactor;
    };

    // Reusable LU factorisation (no pivoting, matches SquareMatrix::LU semantics).
    // Lets callers compute A^-1 * B without ever materialising A^-1, which is
    // significantly cheaper than building the full inverse and then doing a GEMM.
    class LUFactor
    {
    public:
        explicit LUFactor(const SquareMatrix & A);

        // Returns A^-1 * B, computed via forward+back substitution against the
        // factorised A. Both passes operate on whole rows of B at once, so the
        // inner loop walks contiguous memory.
        [[nodiscard]] SquareMatrix solveRight(const SquareMatrix & B) const;

        [[nodiscard]] std::size_t size() const { return m_size; }

    private:
        std::size_t m_size;
        // Row-major flat LU. Lower triangle (i > j) holds L's multipliers
        // (L has unit diagonal); upper triangle (i <= j) holds U.
        MatrixStorage m_LU;
    };

    std::vector<double> operator*(const std::vector<double> & first, const SquareMatrix & second);

    SquareMatrix multiplyWithDiagonalMatrix(const std::vector<double> & tInput,
                                            const SquareMatrix & tMatrix);

    SquareMatrix multiplyWithDiagonalMatrix(const SquareMatrix & tMatrix,
                                            const std::vector<double> & tInput);


}   // namespace FenestrationCommon
