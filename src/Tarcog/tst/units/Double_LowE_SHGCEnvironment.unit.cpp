#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

// This test is adopted to exactly match one of the test provided in WinCalc. If this test is compared
// to the WINDOW output, the results will not be identical. This is because WINDOW uses header
// from the optics file for the emissivity, while WinCalc will calculate emissivity if the data
// are available and in this case, calculated emissivity is different from what is presented in
// the header of the optics file.
class TestDouble_LowE_SHGCEnvironment : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 305.15;   // Kelvins
        auto airSpeed = 2.75;           // meters per second
        auto tSky = 305.15;             // Kelvins
        auto solarRadiation = 783.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 297.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayer2191Thickness = 0.003;   // [m]
        auto solidLayer2191Conductance = 1.0;   // [W/m2K]
        auto emissivityFront2191 = 0.068507126596106735;
        auto tirFront2191 = 0.0;
        auto emissivityBack2191 = 0.84479203668787961;
        auto solidLayer102Thickness = 0.003048;   // [m]
        auto solidLayer102Conductance = 1.0;      // [W/m2K]

        auto aSolidLayer1 = Tarcog::ISO15099::Layers::solid(solidLayer2191Thickness,
                                                            solidLayer2191Conductance,
                                                            emissivityFront2191,
                                                            tirFront2191,
                                                            emissivityBack2191,
                                                            tirFront2191);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayer102Thickness, solidLayer102Conductance);

        auto gapThickness = 0.0127;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->setAbsorptances({0.12766749469201363, 0.056008435686909087});
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CSystem> GetSystem() const
    {
        return m_TarcogSystem;
    };
};

TEST_F(TestDouble_LowE_SHGCEnvironment, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Surface temperatures");

    constexpr auto Tolerance = 1e-6;

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    auto Temperature = aSystem->getTemperatures(aRun);
    std::vector correctTemperature = {303.7294, 303.663604, 300.349928, 300.283080};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    auto SolidLayerConductivities = aSystem->getSolidEffectiveLayerConductivities(aRun);
    std::vector<double> correctSolidConductivities{1, 1};
    EXPECT_EQ(SolidLayerConductivities.size(), correctSolidConductivities.size());

    for(auto i = 0u; i < SolidLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctSolidConductivities[i], SolidLayerConductivities[i], Tolerance);
    }

    auto GapLayerConductivities = aSystem->getGapEffectiveLayerConductivities(aRun);
    std::vector<double> correctGapConductivities{0.084056};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    auto Radiosity = aSystem->getRadiosities(aRun);
    std::vector<double> correctRadiosity = {490.979148, 479.325622, 464.261514, 457.951125};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());
    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    auto effectiveSystemConductivity{aSystem->getEffectiveSystemConductivity(aRun)};
    EXPECT_NEAR(0.119309, effectiveSystemConductivity, Tolerance);

    auto thickness{aSystem->thickness(aRun)};
    EXPECT_NEAR(0.018748, thickness, Tolerance);

    auto numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(1u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    aRun = Tarcog::ISO15099::System::SHGC;

    Temperature = aSystem->getTemperatures(aRun);
    correctTemperature = {309.949976, 310.022538, 306.305781, 306.160325};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    SolidLayerConductivities = aSystem->getSolidEffectiveLayerConductivities(aRun);
    correctSolidConductivities = {1, 1};
    EXPECT_EQ(SolidLayerConductivities.size(), correctSolidConductivities.size());

    for(auto i = 0u; i < SolidLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctSolidConductivities[i], SolidLayerConductivities[i], Tolerance);
    }

    GapLayerConductivities = aSystem->getGapEffectiveLayerConductivities(aRun);
    correctGapConductivities = {0.088139};
    EXPECT_EQ(GapLayerConductivities.size(), correctGapConductivities.size());

    for(auto i = 0u; i < GapLayerConductivities.size(); ++i)
    {
        EXPECT_NEAR(correctGapConductivities[i], GapLayerConductivities[i], Tolerance);
    }

    Radiosity = aSystem->getRadiosities(aRun);
    correctRadiosity = {493.771471, 520.462911, 502.511582, 489.168383};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    effectiveSystemConductivity = aSystem->getEffectiveSystemConductivity(aRun);
    EXPECT_NEAR(0.344565, effectiveSystemConductivity, Tolerance);

    thickness = aSystem->thickness(aRun);
    EXPECT_NEAR(0.018748, thickness, Tolerance);

    numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(1, int(numOfIter));

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    auto Uvalue = aSystem->getUValue();
    EXPECT_NEAR(Uvalue, 2.741473, Tolerance);

    constexpr auto Totsol{0.58621139541797063};
    auto SHGC = aSystem->getSHGC(Totsol);
    EXPECT_NEAR(SHGC, 0.647153, Tolerance);

    auto relativeHeatGain = aSystem->relativeHeatGain(0.703296);
    EXPECT_NEAR(relativeHeatGain, 575.532702, Tolerance);
}
