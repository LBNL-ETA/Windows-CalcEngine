#pragma once

#include <vector>
#include <optional>

#include "SquareMatrix.hpp"

namespace FenestrationCommon
{
    class CSeries;
    enum class IntegrationType;


    struct MatrixAtWavelength
    {
        double x;
        SquareMatrix matrix;
    };

    class CMatrixSeries
    {
    public:
        CMatrixSeries() = default;
        CMatrixSeries(size_t t_Size1, size_t t_Size2, size_t seriesSize = 0u);
        CMatrixSeries(const CMatrixSeries & t_MatrixSeries);
        CMatrixSeries & operator=(CMatrixSeries const & t_MatrixSeries);

        // add property at specific series position
        void addProperty(size_t i, size_t j, double t_Wavelength, double t_Value);
        void addProperties(size_t i, double t_Wavelength, const std::vector<double> & t_Values);
        void setPropertiesAtIndex(size_t index, size_t i, double t_Wavelength, const std::vector<double> & t_Values);
        void addProperties(double t_Wavelength, const SquareMatrix & t_Matrix);
        void setPropertiesAtIndex(size_t index, double t_Wavelength, const SquareMatrix & t_Matrix);

        void addSeries(size_t i, size_t j, const CSeries & series);

        // Multiply all series in matrix with provided one
        void mMult(const CSeries & t_Series);

        // Multiplication of several series with matrix series
        void mMult(const std::vector<CSeries> & t_Series);

        std::vector<CSeries> & operator[](size_t index);

        void integrate(IntegrationType t_Integration,
                       double normalizationCoefficient,
                       const std::optional<std::vector<double>> & integrationPoints);

        void interpolate(const std::vector<double> & t_Wavelengths);

        [[nodiscard]] std::vector<std::vector<double>> getSums(
          double minLambda, double maxLambda, const std::vector<double> & t_ScaleValue) const;

        [[nodiscard]] std::vector<std::vector<double>> getSums(double minLambda,
                                                               double maxLambda) const;

        SquareMatrix getSquaredMatrixSums(double minLambda,
                                          double maxLambda,
                                          const std::vector<double> & t_ScaleValue) const;

        [[nodiscard]] std::vector<MatrixAtWavelength> seriesMatrices() const;

        [[nodiscard]] size_t size1() const;
        [[nodiscard]] size_t size2() const;

    private:
        std::vector<std::vector<CSeries>> m_Matrix;
        size_t m_Size1{};
        size_t m_Size2{};
    };

}   // namespace FenestrationCommon
