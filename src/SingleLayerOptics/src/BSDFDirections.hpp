#pragma once

#include <vector>
#include <memory>
#include <map>

#include "WCECommon.hpp"
#include "BSDFPatch.hpp"
#include "BeamDirection.hpp"

namespace SingleLayerOptics
{
    class BSDFDefinition
    {
    public:
        BSDFDefinition(double t_Theta, size_t t_NumOfPhis);
        [[nodiscard]] double theta() const;
        [[nodiscard]] size_t numOfPhis() const;

    private:
        double m_Theta;
        size_t m_NumOfPhis;
    };

    enum class BSDFDirection
    {
        Incoming,
        Outgoing
    };

    class EnumBSDFDirection : public FenestrationCommon::Enum<BSDFDirection>
    {};

    inline EnumBSDFDirection::Iterator begin(EnumBSDFDirection)
    {
        return EnumBSDFDirection::Iterator(static_cast<int>(BSDFDirection::Incoming));
    }

    inline EnumBSDFDirection::Iterator end(EnumBSDFDirection)
    {
        return EnumBSDFDirection::Iterator(static_cast<int>(BSDFDirection::Outgoing) + 1);
    }

    class BSDFDirections
    {
    public:
        BSDFDirections() = default;
        explicit BSDFDirections(const std::vector<BSDFDefinition> & t_Definitions);
        [[nodiscard]] size_t size() const;
        const CBSDFPatch & operator[](size_t Index) const;
        std::vector<CBSDFPatch>::iterator begin();
        std::vector<CBSDFPatch>::iterator end();

        [[nodiscard]] std::vector<CBSDFPatch>::const_iterator begin() const;
        [[nodiscard]] std::vector<CBSDFPatch>::const_iterator end() const;

        [[nodiscard]] std::vector<double> lambdaVector() const;
        [[nodiscard]] std::vector<double> profileAngles() const;
        [[nodiscard]] const FenestrationCommon::SquareMatrix & lambdaMatrix() const;

        // returns index of element that is closest to given Theta and Phi angles
        [[nodiscard]] size_t getNearestBeamIndex(double t_Theta, double t_Phi) const;

    private:
        std::vector<CBSDFPatch> m_Patches;
        std::vector<double> m_LambdaVector;
        FenestrationCommon::SquareMatrix m_LambdaMatrix;

        //! Function that will create angle limits based on patch index.
        AngleLimits createAngleLimits(double lowerAngle, double upperAngle, size_t patchIndex);
        static double correctPhiForOutgoingDirection(double currentPhi);
        std::vector<CBSDFPatch> createBSDFPatches(const std::vector<double> & thetaAngles,
                                                  const std::vector<size_t> & numPhiAngles);
        static std::vector<double>
          getThetaAngles(const std::vector<BSDFDefinition> & t_Definitions);
        static std::vector<size_t>
          getNumberOfPhiAngles(const std::vector<BSDFDefinition> & t_Definitions);

        static std::vector<double> getLambdaVector(std::vector<CBSDFPatch> patches);
        static FenestrationCommon::SquareMatrix
          setLambdaMatrix(const std::vector<double> & lambdas);
    };

    enum class BSDFBasis
    {
        Small,
        Quarter,
        Half,
        Full
    };

    class BSDFHemisphere
    {
    public:
        static BSDFHemisphere create(BSDFBasis t_Basis);
        static BSDFHemisphere create(const std::vector<BSDFDefinition> & t_Definitions);

        [[nodiscard]] const BSDFDirections & getDirections(BSDFDirection t_Side) const;

        [[nodiscard]] std::vector<double> profileAngles(BSDFDirection t_Side) const;

    private:
        static const std::map<BSDFBasis, std::vector<BSDFDefinition>> bsdfDefinition;

        // Construction for pre-defined basis
        explicit BSDFHemisphere(BSDFBasis t_Basis);
        // Construction for custom basis
        explicit BSDFHemisphere(const std::vector<BSDFDefinition> & t_Definitions);

        std::map<BSDFDirection, BSDFDirections> m_Directions;
        static std::map<BSDFDirection, BSDFDirections>
          generateBSDFDirections(const std::vector<BSDFDefinition> & t_Definitions);
    };

}   // namespace SingleLayerOptics