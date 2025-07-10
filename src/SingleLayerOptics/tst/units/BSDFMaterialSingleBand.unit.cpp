#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCESpectralAveraging.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestBSDFMaterialSingleBand : public testing::Test
{
public:
    BSDFHemisphere m_Hemisphere{BSDFHemisphere::create(BSDFBasis::Small)};
    std::shared_ptr<CMaterialSingleBandBSDF> m_Material;
    std::vector<std::vector<double>> m_Tf;
    std::vector<std::vector<double>> m_Tb;
    std::vector<std::vector<double>> m_Rf;
    std::vector<std::vector<double>> m_Rb;

    std::vector<std::vector<double>> loadTf()
    {
        std::vector<std::vector<double>> data{
          {2.033760, 0.022174, 0.022174, 0.022174, 0.022174, 0.022174, 0.022174},
          {0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223},
          {0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223},
          {0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461},
          {0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461},
          {0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551},
          {0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551}};

        return data;
    }

    std::vector<std::vector<double>> loadRf()
    {
        std::vector<std::vector<double>> data{
          {0.148154, 0.148805, 0.148805, 0.148805, 0.148805, 0.148805, 0.148805},
          {0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762},
          {0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762},
          {0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041},
          {0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041},
          {0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675},
          {0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675}};

        return data;
    }


protected:
    virtual void SetUp()
    {
        m_Tf = loadTf();
        m_Tb = m_Tf;
        m_Rf = loadRf();
        m_Rb = m_Rf;
        m_Material =
          std::make_shared<CMaterialSingleBandBSDF>(m_Tf, m_Tb, m_Rf, m_Rb, m_Hemisphere);
    }
};

TEST_F(TestBSDFMaterialSingleBand, TestProperties)
{
    SCOPED_TRACE("Begin Test: Properties for a single band BSDF material.");

    auto incomingDirections = m_Hemisphere.getDirections(BSDFDirection::Incoming);
    auto outgoingDirections = m_Hemisphere.getDirections(BSDFDirection::Outgoing);
    auto outgoingLambdas = m_Hemisphere.getDirections(BSDFDirection::Outgoing).lambdaVector();

    // Test to make sure matrix returned by material is the same that it was constructed with
    EXPECT_EQ(m_Material->getBSDFMatrix(Property::T, Side::Front), m_Tf);

    // Test to make sure that the value returned by normal incidence matches the
    // value at normal in the BSDF * outgoing lambda at normal outgoing
    EXPECT_EQ(m_Material->getProperty(Property::T, Side::Front), m_Tf[0][0] * outgoingLambdas[0]);

    // Test to make sure an off-normal incoming and outgoing angle produce the same result
    // as the value of the patch * lambda of outgoing direction
    double incomingTheta = 37;
    double incomingPhi = 76;
    double outgoingTheta = 62;
    double outgoingPhi = 23;
    CBeamDirection incomingDirection(incomingTheta, incomingPhi);
    CBeamDirection outgoingDirection(outgoingTheta, outgoingPhi);
    auto incomingIdx =
      incomingDirections.getNearestBeamIndex(incomingDirection.theta(), incomingDirection.phi());
    auto outgoingIdx =
      outgoingDirections.getNearestBeamIndex(outgoingDirection.theta(), outgoingDirection.phi());
    EXPECT_NEAR(
      m_Material->getProperty(Property::T, Side::Front, incomingDirection, outgoingDirection),
      m_Tf[incomingIdx][outgoingIdx] * outgoingLambdas[outgoingIdx],
      1e-6);

    // Test to make sure absorptance calculation at off-normal angles produces
    // 1 - sum(outgoing transmissions for incoming angle - sum(outgoing reflectances for incoming
    // angle)
    double tfHem = 0;
    double rfHem = 0;
    for(size_t oIdx = 0; oIdx < outgoingDirections.size(); ++oIdx)
    {
        tfHem += m_Tf[oIdx][incomingIdx] * outgoingLambdas[oIdx];
        rfHem += m_Rf[oIdx][incomingIdx] * outgoingLambdas[oIdx];
    }
    EXPECT_NEAR(
      m_Material->getProperty(Property::Abs, Side::Front, incomingDirection, outgoingDirection),
      1 - tfHem - rfHem,
      1e-6);

    // Test to make sure getBandProperties returns two copies of the same value returned
    // by getProperty
    double propValue =
      m_Material->getProperty(Property::T, Side::Front, incomingDirection, outgoingDirection);
    std::vector<double> expectedBandProperties{propValue, propValue};
    auto test{m_Material->getBandProperties(
      Property::T, Side::Front, incomingDirection, outgoingDirection)};
    EXPECT_EQ(
      m_Material->getBandProperties(Property::T, Side::Front, incomingDirection, outgoingDirection),
      expectedBandProperties);
}

TEST_F(TestBSDFMaterialSingleBand, TestPropertyAtWavelength)
{
    double theta = 0;
    double phi = 0;
    CBeamDirection incomingDirection(theta, phi);
    CBeamDirection outgoingDirection(theta, phi);
    size_t wavelengthIndex{1u};

    const auto correct{0.081878};
    const auto result = m_Material->getBandProperty(
      Property::T, Side::Front, wavelengthIndex, incomingDirection, outgoingDirection);

    EXPECT_NEAR(correct, result, 1e-6);
}
