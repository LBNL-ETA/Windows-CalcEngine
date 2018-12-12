#ifndef BSDFDIRECTIONS_H
#define BSDFDIRECTIONS_H

#include <vector>
#include <memory>
#include <map>

#include "WCECommon.hpp"

namespace SingleLayerOptics
{
    class CBSDFPatch;
    class CBeamDirection;

    class CBSDFDefinition
    {
    public:
        CBSDFDefinition(const double t_Theta, const size_t t_NumOfPhis);
        double theta() const;
        size_t numOfPhis() const;

    private:
        double m_Theta;
        size_t m_NumOfPhis;
    };

    enum class BSDFDirection
    {
        Incoming,
        Outgoing
    };

    class CBSDFDirections
    {
    public:
        CBSDFDirections(const std::vector<CBSDFDefinition> & t_Definitions, BSDFDirection t_Side);
        size_t size() const;
        std::shared_ptr<const CBSDFPatch> operator[](size_t Index) const;
        std::vector<std::shared_ptr<CBSDFPatch>>::iterator begin();
        std::vector<std::shared_ptr<CBSDFPatch>>::iterator end();

        std::vector<double> lambdaVector() const;
        const FenestrationCommon::SquareMatrix & lambdaMatrix() const;

        // returns index of element that is closest to given Theta and Phi angles
        size_t getNearestBeamIndex(double t_Theta, double t_Phi) const;

    private:
        std::vector<std::shared_ptr<CBSDFPatch>> m_Patches;
        std::vector<double> m_LambdaVector;
        FenestrationCommon::SquareMatrix m_LambdaMatrix;
    };

    enum class BSDFBasis
    {
        Small,
        Quarter,
        Half,
        Full
    };

    class CBSDFHemisphere
    {
    public:
        static CBSDFHemisphere create(BSDFBasis t_Basis);
        static CBSDFHemisphere create(const std::vector<CBSDFDefinition> & t_Definitions);

        const CBSDFDirections & getDirections(BSDFDirection t_Side) const;

    private:
        // Construction for pre-defined basis
        explicit CBSDFHemisphere(BSDFBasis t_Basis);
        // Construction for custom basis
        explicit CBSDFHemisphere(const std::vector<CBSDFDefinition> & t_Definitions);

        std::map<BSDFDirection, CBSDFDirections> m_Directions;
    };

}   // namespace SingleLayerOptics

#endif
