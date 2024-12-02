#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCEGases.hpp"
#include "WCETarcog.hpp"
#include "WCECommon.hpp"

class TestDoubleOutsideVenetianShade_SHGC : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

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
        auto shadeLayerConductance = 160.0;

        const auto matThickness{0.0001};   // m

        const FenestrationCommon::Venetian::Geometry geometry{
          0.0148, 0.0127, 0.0, 0.0331305656433105};

        const auto dl{0.0};
        const auto dr{0.0};
        const auto dtop{0.0};
        const auto dbot{0.0};

        EffectiveLayers::ShadeOpenness openness{dl, dr, dtop, dbot};

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;

        EffectiveLayers::EffectiveHorizontalVenetian effectiveVenetian{
          windowWidth, windowHeight, matThickness, geometry, openness};

        // From unit test
        auto Ef = 0.5564947806702053;
        auto Eb = 0.5564947806702053;
        auto Tirf = 0.42293224373137134;
        auto Tirb = 0.42293224373137134;

        auto aLayer1 = Tarcog::ISO15099::Layers::shading(effectiveVenetian.effectiveThickness(),
                                                         shadeLayerConductance,
                                                         effectiveVenetian.getEffectiveOpenness(),
                                                         Ef,
                                                         Tirf,
                                                         Eb,
                                                         Tirb);

        // From unit test
        aLayer1->setSolarHeatGain(0.030609361, solarRadiation);

        auto gapThickness = 0.0127;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;
        auto aLayer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aLayer2 != nullptr);

        // From unit test
        aLayer2->setSolarHeatGain(0.08669346, solarRadiation);


        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aLayer1, GapLayer1, aLayer2});

        // Alternative option of adding layers.
        // aIGU.addLayer(aLayer1);
        // aIGU.addLayer(GapLayer1);
        // aIGU.addLayer(aLayer2);
        // aIGU.addLayer(GapLayer2);
        // aIGU.addLayer(aLayer3);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::unique_ptr<Tarcog::ISO15099::CSystem>(
          new Tarcog::ISO15099::CSystem(aIGU, Indoor, Outdoor));
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    Tarcog::ISO15099::CSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    };
};

TEST_F(TestDoubleOutsideVenetianShade_SHGC, Test1)
{
    SCOPED_TRACE("Begin Test: Outside venetian shade.");

    auto aSystem = GetSystem();

    const auto uval = aSystem->getUValue();
    EXPECT_NEAR(3.352152, uval, 1e-6);

    const auto effectiveLayerConductivities{
      aSystem->getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue)};

    const std::vector<double> correctEffectConductivites{2.045130, 1};
    EXPECT_EQ(correctEffectConductivites.size(), effectiveLayerConductivities.size());
    for(size_t i = 0u; i < correctEffectConductivites.size(); ++i)
    {
        EXPECT_NEAR(correctEffectConductivites[i], effectiveLayerConductivities[i], 1e-6);
    }

    auto totSol{0.789689322};
    const auto shgc{aSystem->getSHGC(totSol)};
    EXPECT_NEAR(0.841574, shgc, 1e-6);

    const auto heatflow =
      aSystem->getHeatFlow(Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(-67.442678, heatflow, 1e-6);
}
