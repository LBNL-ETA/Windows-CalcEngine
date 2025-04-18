#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

/// This example is compared with fortran version of WINDOW

class TestDoubleOutsidePerforatedShadeExterior_UValue : public testing::Test
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
        auto shadeLayerConductance = 0.3;

        // make cell geometry

        // 31006 represents material properties at the time of the creation of this unit test
        const auto thickness_31006{0.0006};
        const FenestrationCommon::Perforated::Geometry aGeometry{
          FenestrationCommon::Perforated::Type::Circular, 0.00169, 0.00169, 0.00116, 0.00116};

        const auto dl{0.0};
        const auto dr{0.0};
        const auto dtop{0.0};
        const auto dbot{0.0};

        EffectiveLayers::ShadeOpenness openness{dl, dr, dtop, dbot};

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;

        EffectiveLayers::EffectiveLayerPerforated effectiveLayerPerforated{
          windowWidth, windowHeight, thickness_31006, aGeometry, openness};

        const auto Ef = 0.752239525318;
        const auto Eb = 0.752239525318;
        const auto Tirf = 0.164178311825;
        const auto Tirb = 0.164178311825;

        auto aLayer1 =
          Tarcog::ISO15099::Layers::shading(effectiveLayerPerforated.effectiveThickness(),
                                            shadeLayerConductance,
                                            effectiveLayerPerforated.getEffectiveOpenness(),
                                            Ef,
                                            Tirf,
                                            Eb,
                                            Tirb);

        aLayer1->setSolarHeatGain(0.324484854937, solarRadiation);

        auto gapThickness = 0.0127;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;
        auto aLayer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aLayer2 != nullptr);

        aLayer2->setSolarHeatGain(0.034704498947, solarRadiation);


        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aLayer1, GapLayer1, aLayer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    }
};

TEST_F(TestDoubleOutsidePerforatedShadeExterior_UValue, Test1)
{
    SCOPED_TRACE("Begin Test: Outside perforated shade.");

    auto * const aSystem = GetSystem();

    const auto uval = aSystem->getUValue();

    // Fortran version of tarcog in WINDOW gives 3.215803
    EXPECT_NEAR(3.215808, uval, 1e-6);

    const auto effectiveLayerConductivities{
      aSystem->getSolidEffectiveLayerConductivities(Tarcog::ISO15099::System::Uvalue)};

    const std::vector<double> correctEffectConductivites{0.230300, 1};
    EXPECT_EQ(correctEffectConductivites.size(), effectiveLayerConductivities.size());
    for(size_t i = 0u; i < correctEffectConductivites.size(); ++i)
    {
        EXPECT_NEAR(correctEffectConductivites[i], effectiveLayerConductivities[i], 1e-6);
    }

    const auto heatflow =
      aSystem->getHeatFlow(Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor);

    // Fortran version of tracog in WINDOW gives 125.416325
    EXPECT_NEAR(125.416522, heatflow, 1e-6);
}
