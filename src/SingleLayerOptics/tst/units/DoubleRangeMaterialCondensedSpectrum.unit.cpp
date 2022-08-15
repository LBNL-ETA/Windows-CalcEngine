#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"

using SingleLayerOptics::CMaterial;
using SingleLayerOptics::Material;

// Creation of double range material with provided ratio
class TestDoubleRangeMaterialRatioCondensedSpectrum : public testing::Test
{
private:
    std::shared_ptr<CMaterial> m_Material;

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

protected:
    virtual void SetUp()
    {
        // Solar range material
        const auto Tsol = 0.1;
        const auto Rfsol = 0.7;
        const auto Rbsol = 0.7;

        // Visible range
        const auto Tvis = 0.2;
        const auto Rfvis = 0.6;
        const auto Rbvis = 0.6;

        m_Material = Material::dualBandMaterial(Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        m_Material->setBandWavelengths(condensedSpectrum());
    }

public:
    std::shared_ptr<CMaterial> getMaterial()
    {
        return m_Material;
    };
};

TEST_F(TestDoubleRangeMaterialRatioCondensedSpectrum, TestMaterialProperties)
{
    SCOPED_TRACE("Begin Test: Phi angles creation.");

    using FenestrationCommon::Property;
    using FenestrationCommon::Side;

    std::shared_ptr<CMaterial> aMaterial = getMaterial();

    double T = aMaterial->getProperty(Property::T, Side::Front);

    // Test for solar range first
    EXPECT_NEAR(0.1, T, 1e-6);

    double R = aMaterial->getProperty(Property::R, Side::Front);

    EXPECT_NEAR(0.7, R, 1e-6);

    const auto wavelengths{aMaterial->getBandWavelengths()};
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

    // Properties at four wavelengths should have been created
    size_t size = correctWavelengths.size();

    const auto Transmittances = aMaterial->getBandProperties(Property::T, Side::Front);

    EXPECT_EQ(size, Transmittances.size());

    std::vector<double> correctResults{0.000399,
                                       0.2,
                                       0.2,
                                       0.2,
                                       0.2,
                                       0.2,
                                       0.2,
                                       0.000399,
                                       0.000399,
                                       0.000399,
                                       0.000399,
                                       0.000399,
                                       0.000399,
                                       0.000399,
                                       0.000399,
                                       0.000399,
                                       0.000399};

    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], Transmittances[i], 1e-6);
    }

    const auto Reflectances = aMaterial->getBandProperties(Property::R, Side::Front);

    EXPECT_EQ(size, Reflectances.size());

    correctResults = {0.799601,
                      0.6,
                      0.6,
                      0.6,
                      0.6,
                      0.6,
                      0.6,
                      0.799601,
                      0.799601,
                      0.799601,
                      0.799601,
                      0.799601,
                      0.799601,
                      0.799601,
                      0.799601,
                      0.799601,
                      0.799601};

    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], Reflectances[i], 1e-6);
    }
}
