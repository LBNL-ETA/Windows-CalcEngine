#ifndef BSDFINTEGRATOR_H
#define BSDFINTEGRATOR_H

#include <memory>
#include <vector>
#include <map>
#include <WCECommon.hpp>

#include "BSDFDirections.hpp"

namespace FenestrationCommon
{
    class SquareMatrix;
    enum class Side;
    enum class PropertySimple;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    // Layer results from BSDF directions.
    class BSDFIntegrator
    {
    public:
        BSDFIntegrator() = default;
        BSDFIntegrator(const BSDFIntegrator & t_Integrator) = default;
        BSDFIntegrator(const CBSDFDirections & t_Directions);

        // Result matrices
        FenestrationCommon::SquareMatrix & getMatrix(FenestrationCommon::Side t_Side,
                                                     FenestrationCommon::PropertySimple t_Property);

        [[nodiscard]] const FenestrationCommon::SquareMatrix &
          at(FenestrationCommon::Side t_Side, FenestrationCommon::PropertySimple t_Property) const;

        void setMatrices(const FenestrationCommon::SquareMatrix & t_Tau,
                         const FenestrationCommon::SquareMatrix & t_Rho,
                         FenestrationCommon::Side t_Side);

        // Direct-direct components
        [[nodiscard]] double DirDir(FenestrationCommon::Side t_Side,
                                    FenestrationCommon::PropertySimple t_Property,
                                    double t_Theta = 0,
                                    double t_Phi = 0) const;
        [[nodiscard]] double DirDir(FenestrationCommon::Side t_Side,
                                    FenestrationCommon::PropertySimple t_Property,
                                    size_t Index) const;

        // Directional hemispherical results for every direction in BSDF definition
        std::vector<double> DirHem(FenestrationCommon::Side t_Side,
                                   FenestrationCommon::PropertySimple t_Property);
        std::vector<double> Abs(FenestrationCommon::Side t_Side);

        // Directional hemispherical results for given Theta and Phi direction
        [[nodiscard]] double DirHem(FenestrationCommon::Side t_Side,
                                    FenestrationCommon::PropertySimple t_Property,
                                    double t_Theta,
                                    double t_Phi);
        [[nodiscard]] double Abs(FenestrationCommon::Side t_Side, double t_Theta, double t_Phi);
        [[nodiscard]] double Abs(FenestrationCommon::Side t_Side, size_t Index);

        // std::shared_ptr< const CBSDFDirections > getDirections() const;

        [[nodiscard]] double DiffDiff(FenestrationCommon::Side t_Side,
                                      FenestrationCommon::PropertySimple t_Property);

        [[nodiscard]] double AbsDiffDiff(FenestrationCommon::Side t_Side);

        // Lambda values for the layer.
        [[nodiscard]] std::vector<double> lambdaVector() const;
        [[nodiscard]] FenestrationCommon::SquareMatrix lambdaMatrix() const;

        [[nodiscard]] size_t getNearestBeamIndex(double t_Theta, double t_Phi) const;

    protected:
        CBSDFDirections m_Directions;
        size_t m_DimMatrices;

    private:
        // Hemispherical integration over m_Directions
        [[nodiscard]] double integrate(FenestrationCommon::SquareMatrix const & t_Matrix) const;

        void calcDiffuseDiffuse();
        void calcHemispherical();

        std::map<std::pair<FenestrationCommon::Side, FenestrationCommon::PropertySimple>,
                 FenestrationCommon::SquareMatrix>
          m_Matrix;
        std::map<std::pair<FenestrationCommon::Side, FenestrationCommon::PropertySimple>,
                 std::vector<double>>
          m_Hem;
        std::map<FenestrationCommon::Side, std::vector<double>> m_Abs;

        bool m_HemisphericalCalculated;
        bool m_DiffuseDiffuseCalculated;
        FenestrationCommon::
          mmap<double, FenestrationCommon::Side, FenestrationCommon::PropertySimple>
            m_DiffDiff;
    };

}   // namespace SingleLayerOptics

#endif
