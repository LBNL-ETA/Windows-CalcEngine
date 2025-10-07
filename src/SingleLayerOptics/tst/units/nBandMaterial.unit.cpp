#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestnBandMaterial : public testing::Test
{
    std::shared_ptr<CMaterial> m_Material;

protected:
    void SetUp() override
    {
        // Create material from samples
        constexpr auto thickness = 3.048e-3;   // [m]
        m_Material =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
    }

public:
    CMaterial & getMaterial() const
    {
        return *m_Material;
    }
};

TEST_F(TestnBandMaterial, TestProperties)
{
    const auto & mat{getMaterial()};
    constexpr size_t wlIndex{5u};
    const auto value{mat.getBandProperty(Property::T, Side::Front, wlIndex)};
    constexpr auto correct{0.218};

    EXPECT_NEAR(value, correct, 1e-6);
}
