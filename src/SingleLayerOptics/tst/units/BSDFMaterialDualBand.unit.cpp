#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCESpectralAveraging.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestBSDFMaterialDualBand : public testing::Test
{
public:
    BSDFHemisphere m_Hemisphere{BSDFHemisphere::create(BSDFBasis::Small)};
    std::shared_ptr<CMaterialSingleBandBSDF> m_MaterialVis;
    std::shared_ptr<CMaterialSingleBandBSDF> m_MaterialSol;
    std::shared_ptr<CMaterialDualBandBSDF> m_Material;
    std::vector<std::vector<double>> m_TfVis;
    std::vector<std::vector<double>> m_TbVis;
    std::vector<std::vector<double>> m_RfVis;
    std::vector<std::vector<double>> m_RbVis;
    std::vector<std::vector<double>> m_TfSol;
    std::vector<std::vector<double>> m_TbSol;
    std::vector<std::vector<double>> m_RfSol;
    std::vector<std::vector<double>> m_RbSol;

    std::vector<std::vector<double>> loadTfVis()
    {
        std::vector<std::vector<double>> data{
          {1.033760, 0.022174, 0.022174, 0.022174, 0.022174, 0.022174, 0.022174},
          {0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223},
          {0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223},
          {0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461},
          {0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461},
          {0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551},
          {0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551}};

        return data;
    }

    std::vector<std::vector<double>> loadRfVis()
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

    std::vector<std::vector<double>> loadTfSol()
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

    std::vector<std::vector<double>> loadRfSol()
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
        m_TfVis = loadTfVis();
        m_TbVis = m_TfVis;
        m_RfVis = loadRfVis();
        m_RbVis = m_RfVis;
        m_MaterialVis =
          std::make_shared<CMaterialSingleBandBSDF>(m_TfVis,
                                                    m_TbVis,
                                                    m_RfVis,
                                                    m_RbVis,
                                                    m_Hemisphere);
        m_TfSol = loadTfSol();
        m_TbSol = m_TfSol;
        m_RfSol = loadRfSol();
        m_RbSol = m_RfSol;
        m_MaterialSol =
          std::make_shared<CMaterialSingleBandBSDF>(m_TfSol,
                                                    m_TbSol,
                                                    m_RfSol,
                                                    m_RbSol,
                                                    m_Hemisphere);
        m_Material = std::make_shared<CMaterialDualBandBSDF>(m_MaterialVis, m_MaterialSol);
        m_Material->createRangesFromRatio(ConstantsData::NIRRatio);
    }
};

TEST_F(TestBSDFMaterialDualBand, TestProperties)
{
    SCOPED_TRACE("Begin Test: Properties for a single band BSDF material.");

    auto incomingDirections = m_Hemisphere.getDirections(BSDFDirection::Incoming);
    auto outgoingDirections = m_Hemisphere.getDirections(BSDFDirection::Outgoing);
    auto outgoingLambdas = m_Hemisphere.getDirections(BSDFDirection::Outgoing).lambdaVector();

    // Test to make sure that the value returned by normal incidence matches the
    // value returned by the CMaterialSingleBandBSDF that represents the full range
    EXPECT_EQ(m_Material->getProperty(Property::T, Side::Front),
              m_MaterialSol->getProperty(Property::T, Side::Front));

    // Test to make sure an off-normal incoming and outgoing angle produce the same result
    // as the value returned by the CMaterialSingleBandBSDF that represents the full range
    double incomingTheta = 0;
    double incomingPhi = 0;
    double outgoingTheta = 0;
    double outgoingPhi = 0;
    CBeamDirection incomingDirection(incomingTheta, incomingPhi);
    CBeamDirection outgoingDirection(outgoingTheta, outgoingPhi);
    EXPECT_EQ(
      m_Material->getProperty(Property::T, Side::Front, incomingDirection, outgoingDirection),
      m_MaterialSol->getProperty(Property::T, Side::Front, incomingDirection, outgoingDirection));

    const auto wavelengths{m_Material->getBandWavelengths()};
    const std::vector<double> correctWavelengths{0.3, 0.38, 0.780002, 2.5};
    EXPECT_EQ(wavelengths.size(), correctWavelengths.size());
    EXPECT_EQ(wavelengths, correctWavelengths);

    // Test to make sure getBandProperties returns the correctly scaled values
    // for each of the calculated wavelength bands.
    auto bandProperties =
      m_Material->getBandProperties(Property::T, Side::Front, incomingDirection, outgoingDirection);
    std::vector<double> expectedBandProperties{0.121977, 0.0416186, 0.121977, 0.121977};

    EXPECT_EQ(bandProperties.size(), expectedBandProperties.size());
    for(size_t i = 0u; i < expectedBandProperties.size(); ++i)
    {
        EXPECT_NEAR(expectedBandProperties[i], bandProperties[i], 1e-6);
    }
}

TEST_F(TestBSDFMaterialDualBand, TestPropertyAtWavelength)
{
    double theta = 0;
    double phi = 0;
    CBeamDirection incomingDirection(theta, phi);
    CBeamDirection outgoingDirection(theta, phi);
    size_t wavelengthIndex{1u};

    const auto correct{0.0416186};
    const auto result = m_Material->getBandProperty(
      Property::T, Side::Front, wavelengthIndex, incomingDirection, outgoingDirection);

    EXPECT_NEAR(correct, result, 1e-6);
}