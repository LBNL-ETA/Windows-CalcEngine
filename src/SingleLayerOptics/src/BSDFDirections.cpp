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
    ///  CBSDFDefinition
    /////////////////////////////////////////////////////////////////

    CBSDFDefinition::CBSDFDefinition(const double t_Theta, const size_t t_NumOfPhis) :
        m_Theta(t_Theta),
        m_NumOfPhis(t_NumOfPhis)
    {}

    double CBSDFDefinition::theta() const
    {
        return m_Theta;
    }

    size_t CBSDFDefinition::numOfPhis() const
    {
        return m_NumOfPhis;
    }

    /////////////////////////////////////////////////////////////////
    ///  BSDFDirections
    /////////////////////////////////////////////////////////////////

    BSDFDirections::BSDFDirections(const std::vector<CBSDFDefinition> & t_Definitions,
                                     const BSDFDirection t_Side)
    {
        std::vector<double> thetaAngles(t_Definitions.size());
        std::transform(std::begin(t_Definitions), std::end(t_Definitions), std::begin(thetaAngles),
                       [](const CBSDFDefinition & val) -> double { return val.theta();});

        std::vector<size_t> numPhiAngles(t_Definitions.size());
        std::transform(std::begin(t_Definitions), std::end(t_Definitions), std::begin(numPhiAngles),
                       [](const CBSDFDefinition & val) -> size_t { return val.numOfPhis();});

        CThetaLimits ThetaLimits(thetaAngles);
        const auto thetaLimits{ThetaLimits.getThetaLimits()};

        double lowerTheta = thetaLimits[0];
        for(size_t thetaIndex = 1; thetaIndex < thetaLimits.size(); ++thetaIndex)
        {
            double upperTheta = thetaLimits[thetaIndex];
            const auto currentThetaLimits = createAngleLimits(lowerTheta, upperTheta, thetaIndex);

            const auto nPhis = numPhiAngles[thetaIndex - 1];
            CPhiLimits phiAngles(nPhis);
            auto phiLimits = phiAngles.getPhiLimits();
            auto lowerPhi {correctPhiForOutgoingDireciton(t_Side, nPhis, phiLimits[0])};
            for(size_t j = 1; j < phiLimits.size(); ++j)
            {
                const auto upperPhi = correctPhiForOutgoingDireciton(t_Side, nPhis, phiLimits[j]);
                AngleLimits currentPhiLimits(lowerPhi, upperPhi);
                m_Patches.emplace_back(currentThetaLimits, currentPhiLimits);
                lowerPhi = upperPhi;
            }
            lowerTheta = upperTheta;
        }

        // build lambda std::vector and matrix
        size_t size = m_Patches.size();
        // m_LambdaVector = std::make_shared<std::vector<double>>();
        m_LambdaMatrix = SquareMatrix(size);
        for(size_t i = 0; i < size; ++i)
        {
            m_LambdaVector.push_back(m_Patches[i].lambda());
            m_LambdaMatrix(i, i) = m_Patches[i].lambda();
        }
    }

    double BSDFDirections::correctPhiForOutgoingDireciton(const BSDFDirection & t_Side,
                                                        const size_t nPhis,
                                                        double currentPhi)
    {
        return (t_Side == BSDFDirection::Outgoing && nPhis != 1) ? currentPhi + 180 : currentPhi;
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

    /////////////////////////////////////////////////////////////////
    ///  CBSDFHemisphere
    /////////////////////////////////////////////////////////////////

    CBSDFHemisphere::CBSDFHemisphere(const BSDFBasis t_Basis)
    {
        std::vector<CBSDFDefinition> aDefinitions;
        switch(t_Basis)
        {
            case BSDFBasis::Small:
                aDefinitions = {{0, 1}, {13, 1}, {26, 1}, {39, 1}, {52, 1}, {65, 1}, {80.75, 1}};
                break;
            case BSDFBasis::Quarter:
                aDefinitions = {{0, 1}, {18, 8}, {36, 12}, {54, 12}, {76.5, 8}};
                break;
            case BSDFBasis::Half:
                aDefinitions = {
                  {0, 1}, {13, 8}, {26, 12}, {39, 16}, {52, 20}, {65, 12}, {80.75, 8}};
                break;
            case BSDFBasis::Full:
                aDefinitions = {{0, 1},
                                {10, 8},
                                {20, 16},
                                {30, 20},
                                {40, 24},
                                {50, 24},
                                {60, 24},
                                {70, 16},
                                {82.5, 12}};
                break;
            default:
                throw std::runtime_error("Incorrect definition of the basis.");
        }
        m_Directions.insert(std::make_pair(BSDFDirection::Incoming,
                                           BSDFDirections(aDefinitions, BSDFDirection::Incoming)));
        m_Directions.insert(std::make_pair(BSDFDirection::Outgoing,
                                           BSDFDirections(aDefinitions, BSDFDirection::Outgoing)));
    }

    CBSDFHemisphere::CBSDFHemisphere(const std::vector<CBSDFDefinition> & t_Definitions) :
        m_Directions(
          {{BSDFDirection::Incoming, BSDFDirections(t_Definitions, BSDFDirection::Incoming)},
           {BSDFDirection::Outgoing, BSDFDirections(t_Definitions, BSDFDirection::Outgoing)}})
    {}

    const BSDFDirections & CBSDFHemisphere::getDirections(const BSDFDirection tDirection) const
    {
        return m_Directions.at(tDirection);
    }

    CBSDFHemisphere CBSDFHemisphere::create(BSDFBasis t_Basis)
    {
        return CBSDFHemisphere(t_Basis);
    }

    CBSDFHemisphere CBSDFHemisphere::create(const std::vector<CBSDFDefinition> & t_Definitions)
    {
        return CBSDFHemisphere(t_Definitions);
    }

}   // namespace SingleLayerOptics
