#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

class TestIndoorPermeableLayer : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

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

        auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer1 != nullptr);

        auto shadeLayerThickness = 0.0001;
        auto shadeLayerConductance = 160.0;
        auto PermeabilityFactor = 0.15;

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;

        EffectiveLayers::EffectiveLayerCommon effectiveLayer{
          windowWidth, windowHeight, shadeLayerThickness, PermeabilityFactor};

        constexpr auto frontEmissivity = 0.9;
        constexpr auto frontIRTransmittance = 0.0;
        constexpr auto backEmissivity = 0.9;
        constexpr auto backIRTransmittance = 0.0;

        auto layer2 = Tarcog::ISO15099::Layers::shading(shadeLayerThickness,
                                                        shadeLayerConductance,
                                                        effectiveLayer.getEffectiveOpenness(),
                                                        frontEmissivity,
                                                        frontIRTransmittance,
                                                        backEmissivity,
                                                        backIRTransmittance);

        ASSERT_TRUE(layer2 != nullptr);

        auto gapThickness = 0.0127;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap1 != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    }
};

TEST_F(TestIndoorPermeableLayer, Test1)
{
    SCOPED_TRACE("Begin Test: Indoor Permeable Layer");

    auto & tarcogSystem = GetSystem();

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////


    constexpr auto Tolerance = 1e-6;

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    std::vector correctTemperature = {259.702462, 260.108333, 280.800387, 280.800462};
    Helper::testVectors("Temperature - Uvalue run",
                        correctTemperature,
                        tarcogSystem.getTemperatures(aRun),
                        Tolerance);

    std::vector correctSolidConductivities{1.0, 136.003700};
    Helper::testVectors("Solid Conductivities - Uvalue run",
                        correctSolidConductivities,
                        tarcogSystem.getSolidEffectiveLayerConductivities(aRun),
                        Tolerance);

    std::vector correctGapConductivities{0.072131};
    Helper::testVectors("Gap Conductivities - Uvalue run",
                        correctGapConductivities,
                        tarcogSystem.getGapEffectiveLayerConductivities(aRun),
                        Tolerance);

    std::vector correctRadiosity = {255.089478, 273.129071, 344.556891, 359.690198};
    Helper::testVectors(
      "Radiosity - Uvalue run", correctRadiosity, tarcogSystem.getRadiosities(aRun), Tolerance);

    auto effectiveSystemConductivity{tarcogSystem.getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.076533, effectiveSystemConductivity, Tolerance);

    auto thickness{tarcogSystem.thickness(aRun)};
    EXPECT_NEAR(0.015848, thickness, Tolerance);

    // Number of iterations are different for different compilers. I have already explained that
    // in the one of the issues in github repository (Different calculations values between mac and
    // windows #146). auto numOfIter = tarcogSystem.getNumberOfIterations(aRun); EXPECT_EQ(25u,
    // numOfIter);

    auto Uvalue = tarcogSystem.getUValue();
    EXPECT_NEAR(3.414354, Uvalue, Tolerance);

    constexpr double Tsol{0.703296};
    auto SHGC = tarcogSystem.getSHGC(Tsol);
    EXPECT_NEAR(0.0, SHGC, Tolerance);

    auto relativeHeatGain = tarcogSystem.relativeHeatGain(Tsol);
    EXPECT_NEAR(26.563673, relativeHeatGain, Tolerance);
}
