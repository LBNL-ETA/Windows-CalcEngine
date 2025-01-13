#include <cassert>
#include <algorithm>
#include <stdexcept>

#include "BSDFDirections.hpp"
#include "BSDFPatch.hpp"
#include "BSDFThetaLimits.hpp"
#include "BSDFPhiLimits.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    /////////////////////////////////////////////////////////////////
    ///  BSDFDefinition
    /////////////////////////////////////////////////////////////////

    BSDFDefinition::BSDFDefinition(const double t_Theta, const size_t t_NumOfPhis) :
        m_Theta(t_Theta), m_NumOfPhis(t_NumOfPhis)
    {}

    double BSDFDefinition::theta() const
    {
        return m_Theta;
    }

    size_t BSDFDefinition::numOfPhis() const
    {
        return m_NumOfPhis;
    }

    /////////////////////////////////////////////////////////////////
    ///  BSDFDirections
    /////////////////////////////////////////////////////////////////

    BSDFDirections::BSDFDirections(const std::vector<BSDFDefinition> & t_Definitions) :
        m_Patches(createBSDFPatches(
          getThetaAngles(t_Definitions), getNumberOfPhiAngles(t_Definitions))),
        m_LambdaVector(getLambdaVector(m_Patches)),
        m_LambdaMatrix(setLambdaMatrix(m_LambdaVector))
    {
        FenestrationCommon::logMsg("begin BSDFDirections::BSDFDirections");
    }

    std::vector<size_t>
      BSDFDirections::getNumberOfPhiAngles(const std::vector<BSDFDefinition> & t_Definitions)
    {
        FenestrationCommon::logMsg("begin BSDFDirections::getNumberOfPhiAngles");
        std::vector<size_t> numPhiAngles(t_Definitions.size());
        FenestrationCommon::logMsg("begin std::transform");
        std::transform(std::begin(t_Definitions),
                       std::end(t_Definitions),
                       std::begin(numPhiAngles),
                       [](const BSDFDefinition & val) -> size_t { return val.numOfPhis(); });
        FenestrationCommon::logMsg("end BSDFDirections::getNumberOfPhiAngles");
        return numPhiAngles;
    }

    std::vector<double>
      BSDFDirections::getThetaAngles(const std::vector<BSDFDefinition> & t_Definitions)
    {
        FenestrationCommon::logMsg("begin BSDFDirections::getThetaAngles");
        std::vector<double> thetaAngles(t_Definitions.size());
        FenestrationCommon::logMsg("before std::transform");
        std::transform(std::begin(t_Definitions),
                       std::end(t_Definitions),
                       std::begin(thetaAngles),
                       [](const BSDFDefinition & val) -> double { return val.theta(); });
        FenestrationCommon::logMsg("end BSDFDirections::getThetaAngles");
        return thetaAngles;
    }

    std::vector<CBSDFPatch>
      BSDFDirections::createBSDFPatches(const std::vector<double> & thetaAngles,
                                        const std::vector<size_t> & numPhiAngles)
    {
        FenestrationCommon::logMsg("begin createBSDFPatches with thetaAngles.size = "
                                   + std::to_string(thetaAngles.size())
                                   + " numPhiAngles.size = " + std::to_string(numPhiAngles.size()));

        std::vector<CBSDFPatch> patches;
        FenestrationCommon::logMsg("before CThetaLimits ThetaLimits");
        CThetaLimits ThetaLimits(thetaAngles);
        FenestrationCommon::logMsg("before const auto thetaLimits{");
        const auto thetaLimits{ThetaLimits.getThetaLimits()};

        double lowerTheta = thetaLimits[0];
        for(size_t thetaIndex = 1; thetaIndex < thetaLimits.size(); ++thetaIndex)
        {
            FenestrationCommon::logMsg("in for(size_t thetaIndex = 1; thetaIndex < thetaLimits.size(); ++thetaIndex) with thetaIndex = " + std::to_string(thetaIndex));
            double upperTheta = thetaLimits[thetaIndex];
            FenestrationCommon::logMsg("before auto currentThetaLimits = createAngleLimits");
            const auto currentThetaLimits = createAngleLimits(lowerTheta, upperTheta, thetaIndex);
            FenestrationCommon::logMsg("before auto nPhis = numPhiAngles[thetaIndex - 1]");
            const auto nPhis = numPhiAngles[thetaIndex - 1];
            FenestrationCommon::logMsg("before CPhiLimits phiAngles(nPhis)");
            CPhiLimits phiAngles(nPhis);
            FenestrationCommon::logMsg("before phiLimits = phiAngles.getPhiLimits();");
            auto phiLimits = phiAngles.getPhiLimits();
            FenestrationCommon::logMsg("before lowerPhi{correctPhiForOutgoingDirection(phiLimits[0])};");
            auto lowerPhi{correctPhiForOutgoingDirection(phiLimits[0])};
            for(size_t j = 1; j < phiLimits.size(); ++j)
            {
                FenestrationCommon::logMsg("in for(size_t j = 1; j < phiLimits.size(); ++j) with j = " + std::to_string(j));
                const auto upperPhi = correctPhiForOutgoingDirection(phiLimits[j]); 
                FenestrationCommon::logMsg("before AngleLimits currentPhiLimits(lowerPhi, upperPhi);");
                AngleLimits currentPhiLimits(lowerPhi, upperPhi);
                FenestrationCommon::logMsg(
                  "before patches.emplace_back(currentThetaLimits, currentPhiLimits);");
                patches.emplace_back(currentThetaLimits, currentPhiLimits);
                FenestrationCommon::logMsg("before lowerPhi = upperPhi with lowerPhi = "
                                           + std::to_string(lowerPhi)
                                           + " and upperPhi = " + std::to_string(upperPhi));
                lowerPhi = upperPhi;
            }
            FenestrationCommon::logMsg("before lowerTheta = upperTheta with lowerTheta = "
                                       + std::to_string(lowerTheta)
                                       + " and upperTheta = " + std::to_string(upperTheta));

            lowerTheta = upperTheta;
        }

        FenestrationCommon::logMsg("end createBSDFPatches with thetaAngles.size = "
                                   + std::to_string(thetaAngles.size())
                                   + " numPhiAngles.size = " + std::to_string(numPhiAngles.size()));
        return patches;
    }

    double BSDFDirections::correctPhiForOutgoingDirection(double currentPhi)
    {
        return (currentPhi > 360) ? currentPhi - 360 : currentPhi;
    }


    size_t BSDFDirections::size() const
    {
        return m_Patches.size();
    }

    const CBSDFPatch & BSDFDirections::operator[](size_t Index) const
    {
        return m_Patches[Index];
    }

    std::vector<CBSDFPatch>::iterator BSDFDirections::begin()
    {
        return m_Patches.begin();
    }

    std::vector<CBSDFPatch>::iterator BSDFDirections::end()
    {
        return m_Patches.end();
    }

    std::vector<double> BSDFDirections::lambdaVector() const
    {
        return m_LambdaVector;
    }

    const SquareMatrix & BSDFDirections::lambdaMatrix() const
    {
        FenestrationCommon::logMsg("begin BSDFDirections::lambdaMatrix where m_LambdaMatrix.size = " + std::to_string(m_LambdaMatrix.size()));
        return m_LambdaMatrix;
    }

    size_t BSDFDirections::getNearestBeamIndex(const double t_Theta, const double t_Phi) const
    {
        auto it = std::find_if(m_Patches.begin(), m_Patches.end(), [&](const CBSDFPatch & a) {
            return a.isInPatch(t_Theta, t_Phi);
        });

        if(it == m_Patches.end())
        {
            throw std::runtime_error("Could not find nearest beam index");
        }

        size_t index = size_t(std::distance(m_Patches.begin(), it));
        return index;
    }

    AngleLimits
      BSDFDirections::createAngleLimits(double lowerAngle, double upperAngle, size_t patchIndex)
    {
        return patchIndex == 1 ? AngleLimits(upperAngle) : AngleLimits(lowerAngle, upperAngle);
    }

    std::vector<double> BSDFDirections::getLambdaVector(std::vector<CBSDFPatch> patches)
    {
        FenestrationCommon::logMsg("begin BSDFDirections::getLambdaVector");
        std::vector<double> lambda(patches.size());
        FenestrationCommon::logMsg("before std::transform");
        std::transform(std::begin(patches),
                       std::end(patches),
                       std::begin(lambda),
                       [](const CBSDFPatch & val) -> double { return val.lambda(); });
        FenestrationCommon::logMsg("end BSDFDirections::getLambdaVector");
        return lambda;
    }

    FenestrationCommon::SquareMatrix
      BSDFDirections::setLambdaMatrix(const std::vector<double> & lambdas)
    {
        FenestrationCommon::logMsg("begin BSDFDirections::setLambdaMatrix");
        FenestrationCommon::SquareMatrix lambdaMatrix(lambdas.size());
        FenestrationCommon::logMsg("before lambdaMatrix.setDiagonal(lambdas)");
        lambdaMatrix.setDiagonal(lambdas);
        FenestrationCommon::logMsg("end BSDFDirections::setLambdaMatrix");
        return lambdaMatrix;
    }

    std::vector<double> BSDFDirections::profileAngles() const
    {
        std::vector<double> angles;
        for(const auto & patch : m_Patches)
        {
            angles.push_back(patch.centerPoint().profileAngle());
        }
        return angles;
    }

    /////////////////////////////////////////////////////////////////
    ///  BSDFHemisphere
    /////////////////////////////////////////////////////////////////

    const std::map<BSDFBasis, std::vector<BSDFDefinition>> BSDFHemisphere::bsdfDefinition = {
      {BSDFBasis::Small, {{0, 1}, {13, 1}, {26, 1}, {39, 1}, {52, 1}, {65, 1}, {80.75, 1}}},
      {BSDFBasis::Quarter, {{0, 1}, {18, 8}, {36, 12}, {54, 12}, {76.5, 8}}},
      {BSDFBasis::Half, {{0, 1}, {13, 8}, {26, 12}, {39, 16}, {52, 20}, {65, 12}, {80.75, 8}}},
      {BSDFBasis::Full,
       {{0, 1}, {10, 8}, {20, 16}, {30, 20}, {40, 24}, {50, 24}, {60, 24}, {70, 16}, {82.5, 12}}}};

    BSDFHemisphere::BSDFHemisphere(const BSDFBasis t_Basis) :
        m_Directions(generateBSDFDirections(bsdfDefinition.at(t_Basis)))
    {}

    BSDFHemisphere::BSDFHemisphere(const std::vector<BSDFDefinition> & t_Definitions) :
        m_Directions(generateBSDFDirections(t_Definitions))
    {}

    const BSDFDirections & BSDFHemisphere::getDirections(const BSDFDirection tDirection) const
    {
        return m_Directions.at(tDirection);
    }

    BSDFHemisphere BSDFHemisphere::create(BSDFBasis t_Basis)
    {
        return BSDFHemisphere(t_Basis);
    }

    BSDFHemisphere BSDFHemisphere::create(const std::vector<BSDFDefinition> & t_Definitions)
    {
        return BSDFHemisphere(t_Definitions);
    }

    std::map<BSDFDirection, BSDFDirections>
      BSDFHemisphere::generateBSDFDirections(const std::vector<BSDFDefinition> & t_Definitions)
    {
        return {{BSDFDirection::Incoming, BSDFDirections(t_Definitions)},
                {BSDFDirection::Outgoing, BSDFDirections(t_Definitions)}};
    }

    std::vector<double> BSDFHemisphere::profileAngles(BSDFDirection t_Side) const
    {
        return m_Directions.at(t_Side).profileAngles();
    }

}   // namespace SingleLayerOptics
