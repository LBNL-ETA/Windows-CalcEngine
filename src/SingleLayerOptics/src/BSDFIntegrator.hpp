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
    typedef std::pair<FenestrationCommon::Side, FenestrationCommon::PropertySimple> pair_Side_PropertySimple;

    // Layer results from BSDF directions.
    class CBSDFIntegrator
    {
    public:
        explicit CBSDFIntegrator(const std::shared_ptr<const CBSDFIntegrator> & t_Integrator);
        explicit CBSDFIntegrator(const std::shared_ptr<const CBSDFDirections> & t_Directions);

        // Result matrices
        FenestrationCommon::SquareMatrix & getMatrix(const FenestrationCommon::Side t_Side, 
            const FenestrationCommon::PropertySimple t_Property);

        const FenestrationCommon::SquareMatrix & at(const FenestrationCommon::Side t_Side,
            const FenestrationCommon::PropertySimple t_Property) const;

        void setResultMatrices(const FenestrationCommon::SquareMatrix & t_Tau,
                               const FenestrationCommon::SquareMatrix & t_Rho,
                               FenestrationCommon::Side t_Side);

        // Direct-direct components
        double DirDir(const FenestrationCommon::Side t_Side,
                      const FenestrationCommon::PropertySimple t_Property,
                      const double t_Theta,
                      const double t_Phi) const;
        double DirDir(const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property, const size_t Index) const;

        // Directional hemispherical results for every direction in BSDF definition
        std::vector<double> DirHem(const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property);
        std::vector<double> Abs(const FenestrationCommon::Side t_Side);

        // Directional hemispherical results for given Theta and Phi direction
        double DirHem(const FenestrationCommon::Side t_Side,
                      const FenestrationCommon::PropertySimple t_Property,
                      const double t_Theta,
                      const double t_Phi);
        double Abs(const FenestrationCommon::Side t_Side, const double t_Theta, const double t_Phi);
        double Abs(const FenestrationCommon::Side t_Side, const size_t Index);

        // std::shared_ptr< const CBSDFDirections > getDirections() const;

        double DiffDiff(const FenestrationCommon::Side t_Side, const FenestrationCommon::PropertySimple t_Property);

        // Lambda values for the layer.
        std::vector<double> lambdaVector() const;
        FenestrationCommon::SquareMatrix lambdaMatrix() const;

        size_t getNearestBeamIndex(const double t_Theta, const double t_Phi) const;

    protected:
        const CBSDFDirections m_Directions;
        size_t m_DimMatrices;

    private:
        // Hemispherical integration over m_Directions
        double integrate(FenestrationCommon::SquareMatrix const & t_Matrix) const;

        void calcDiffuseDiffuse();
        void calcHemispherical();

        std::map<pair_Side_PropertySimple, FenestrationCommon::SquareMatrix> m_Matrix;
        std::map<pair_Side_PropertySimple, std::vector<double>> m_Hem;
        std::map<FenestrationCommon::Side, std::vector<double>> m_Abs;

        bool m_HemisphericalCalculated;
        bool m_DiffuseDiffuseCalculated;
        FenestrationCommon::mmap<double, FenestrationCommon::Side, FenestrationCommon::PropertySimple> m_MapDiffDiff;
    };

}   // namespace SingleLayerOptics

#endif
