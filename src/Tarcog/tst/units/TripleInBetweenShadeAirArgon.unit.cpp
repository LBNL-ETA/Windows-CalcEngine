#include <memory>
#include <gtest/gtest.h>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

class TestInBetweenShadeAirArgon : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;             // Kelvins
        auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        auto roomTemperature = 295.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////

        // Solid layers
        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer1 != nullptr);

        auto layer3 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer3 != nullptr);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto Atop = 0.1;
        auto Abot = 0.1;
        auto Aleft = 0.1;
        auto Aright = 0.1;
        auto PermeabilityFactor = 0.2;

        EffectiveLayers::ShadeOpenness openness{Aleft, Aright, Atop, Abot};

        double windowWidth = 1;
        double windowHeight = 1;

        EffectiveLayers::EffectiveLayerCommon effectiveLayer{
          windowWidth, windowHeight, shadeLayerThickness, PermeabilityFactor, openness};

        auto layer2 = Tarcog::ISO15099::Layers::shading(
          shadeLayerThickness, shadeLayerConductance, effectiveLayer.getEffectiveOpenness());

        ASSERT_TRUE(layer2 != nullptr);

        // gap layers

        // Create coefficients for Air
        Gases::CIntCoeff AirCon{2.8733e-03, 7.76e-05, 0.0};
        Gases::CIntCoeff AirCp{1.002737e+03, 1.2324e-02, 0.0};
        Gases::CIntCoeff AirVisc{3.7233e-06, 4.94e-08, 0.0};

        Gases::CGasData AirData{"Air", 28.97, 1.4, AirCp, AirCon, AirVisc};

        // Create coefficients for Argon
        Gases::CIntCoeff ArgonCon{2.2848e-03, 5.1486e-05, 0.0};
        Gases::CIntCoeff ArgonCp{5.21929e+02, 0.0, 0.0};
        Gases::CIntCoeff ArgonVisc{3.3786e-06, 6.4514e-08, 0.0};

        Gases::CGasData ArgonData{"Argon", 39.948, 1.67, ArgonCp, ArgonCon, ArgonVisc};

        // Create gas mixture
        Gases::CGas Gas1({{0.1, AirData}, {0.9, ArgonData}});

        auto gapThickness = 0.0127;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness, Gas1);
        ASSERT_TRUE(gap1 != nullptr);

        auto gap2 = Tarcog::ISO15099::Layers::gap(gapThickness, Gas1);
        ASSERT_TRUE(gap2 != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2, gap2, layer3});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    };
};


TEST_F(TestInBetweenShadeAirArgon, Test1)
{
    SCOPED_TRACE("Begin Test: InBetween Shade - Air(10%)/Argon(90%)");

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    const std::vector correctTemperature = {
      257.741586, 258.174255, 271.844631, 271.848848, 284.281326, 284.713996};
    Helper::testVectors("Temperature", correctTemperature, aSystem->getTemperatures(), 1e-6);

    const std::vector correctRadiosity = {
      248.620197, 259.854589, 301.665988, 318.014856, 361.934161, 381.791407};
    Helper::testVectors("Radiosity", correctRadiosity, aSystem->getRadiosities(), 1e-6);

    auto numOfIter = GetSystem()->getNumberOfIterations();
    EXPECT_EQ(27u, numOfIter);
}
