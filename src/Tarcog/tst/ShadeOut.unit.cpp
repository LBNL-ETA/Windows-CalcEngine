#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"


using namespace Tarcog;
using namespace FenestrationCommon;

class TestShadeOut : public testing::Test
{
private:
    std::shared_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto pressure = 101325.0;       // Pascals
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;   // Kelvins
        auto solarRadiation = 0.0;

        auto Outdoor = Environments::outdoor(airTemperature,
                                             pressure,
                                             airSpeed,
                                             solarRadiation,
                                             tSky,
                                             SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Environments::indoor(roomTemperature, pressure);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto emissivity = 0.832855582237;
        auto transmittance = 0.074604861438;

        auto shadeLayerThickness = 0.0006;
        auto shadeLayerConductance = 160.0;
        auto Atop = 0.0;
        auto Abot = 0.0;
        auto Aleft = 0.0;
        auto Aright = 0.0;
        auto Afront = 0.5;

        auto aSolidLayer1 = Layers::shadeWithOpenness(
          shadeLayerThickness,
          shadeLayerConductance,
          Atop, Abot, Aleft, Aright, Afront,
          emissivity, transmittance,
          emissivity, transmittance);

        ASSERT_TRUE(aSolidLayer1 != nullptr);

        auto solidLayerThickness = 0.0056134;   // [m]
        auto solidLayerConductance = 1.0;
        auto emissivity1 = 0.84;
        auto emissivity2 = 0.038798544556;
        transmittance = 0.0;

        auto aSolidLayer2 = Layers::solid( solidLayerThickness, solidLayerConductance, emissivity1,
        	transmittance, emissivity2, transmittance);
        ASSERT_TRUE(aSolidLayer2 != nullptr);

        auto gapThickness = 0.0127;
        auto gapPressure = 101325.0;
        auto aGapLayer = Layers::gap(gapThickness, gapPressure);
        ASSERT_TRUE(aGapLayer != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer1);
        aIGU.addLayer(aGapLayer);
        aIGU.addLayer(aSolidLayer2);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    std::shared_ptr<CSingleSystem> GetSystem() const
    {
        return m_TarcogSystem;
    };
};

TEST_F(TestShadeOut, Test1)
{
    SCOPED_TRACE("Begin Test: Single Clear - U-value");

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    auto Temperature = *aSystem->getTemperatures();
    std::vector<double> correctTemperature = {256.991924, 256.992140, 269.666330, 270.128394};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], 1e-5);
    }

    auto Radiosity = *aSystem->getRadiosities();
    std::vector<double> correctRadiosity = {249.993042, 250.921069, 291.999868, 419.703053};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], 1e-5);
    }
}
