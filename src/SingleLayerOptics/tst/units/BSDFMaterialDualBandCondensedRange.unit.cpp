#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCESpectralAveraging.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"

using SingleLayerOptics::BSDFHemisphere;
using SingleLayerOptics::BSDFBasis;
using SingleLayerOptics::CMaterialSingleBandBSDF;
using SingleLayerOptics::CMaterialDualBandBSDF;
using SingleLayerOptics::BSDFDirection;
using SingleLayerOptics::CBeamDirection;

class TestBSDFMaterialDualBandCondensedRange : public testing::Test
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

    static std::vector<double> condensedSpectrum()
    {
        return {0.3,
                0.38,
                0.46,
                0.54,
                0.62,
                0.7,
                0.78,
                0.952,
                1.124,
                1.296,
                1.468,
                1.64,
                1.812,
                1.984,
                2.156,
                2.328,
                2.5};
    }

    static std::vector<std::vector<double>> loadTfVis()
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

    static std::vector<std::vector<double>> loadRfVis()
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

    static std::vector<std::vector<double>> loadTfSol()
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

    static std::vector<std::vector<double>> loadRfSol()
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

        m_Material->setBandWavelengths(condensedSpectrum());
    }
};

TEST_F(TestBSDFMaterialDualBandCondensedRange, TestProperties)
{
    SCOPED_TRACE("Begin Test: Properties for a single band BSDF material.");

    using FenestrationCommon::Property;
    using FenestrationCommon::Side;

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
    const std::vector<double> correctWavelengths{0.3,
                                                 0.38,
                                                 0.46,
                                                 0.54,
                                                 0.62,
                                                 0.7,
                                                 0.78,
                                                 0.952,
                                                 1.124,
                                                 1.296,
                                                 1.468,
                                                 1.64,
                                                 1.812,
                                                 1.984,
                                                 2.156,
                                                 2.328,
                                                 2.5};
    EXPECT_EQ(wavelengths.size(), correctWavelengths.size());
    for(size_t i = 0u; i < correctWavelengths.size(); ++i)
    {
        EXPECT_NEAR(wavelengths[i], correctWavelengths[i], 1e-6);
    }

    // Test to make sure getBandProperties returns the correctly scaled values
    // for each of the calculated wavelength bands.
    auto bandProperties =
      m_Material->getBandProperties(Property::T, Side::Front, incomingDirection, outgoingDirection);
    std::vector<double> expectedBandProperties{0.121977,
                                               0.0416186,
                                               0.0416186,
                                               0.0416186,
                                               0.0416186,
                                               0.0416186,
                                               0.0416186,
                                               0.121977,
                                               0.121977,
                                               0.121977,
                                               0.121977,
                                               0.121977,
                                               0.121977,
                                               0.121977,
                                               0.121977,
                                               0.121977,
                                               0.121977};

    EXPECT_EQ(bandProperties.size(), expectedBandProperties.size());
    for(size_t i = 0u; i < expectedBandProperties.size(); ++i)
    {
        EXPECT_NEAR(expectedBandProperties[i], bandProperties[i], 1e-6);
    }
}