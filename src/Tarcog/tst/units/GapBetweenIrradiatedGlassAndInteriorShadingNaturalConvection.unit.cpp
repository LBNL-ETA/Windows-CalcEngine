#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using Tarcog::ISO15099::CIGUSolidLayer;
using Tarcog::ISO15099::CIGUGapLayer;
using Tarcog::ISO15099::CSingleSystem;

class TestGapBetweenIrradiatedGlassAndInteriorShadingNaturalConvection : public testing::Test
{
private:
    std::unique_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////

        auto outdoorAirTemperature = 298.15;   // Kelvins
        auto outdoorAirSpeed = 2.75;                // meters per second
        auto tSky = outdoorAirTemperature;                 // Kelvins
        auto solarRadiation = 1000.0;
        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          outdoorAirTemperature, outdoorAirSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 298.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        
        auto windowWidth = 1.0;
        auto windowHeight = 1.0;

        const auto solidLayerThickness = 0.003048;   // [m]
        const auto solidLayerConductance = 1.0;
        auto solidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        solidLayer->setSolarHeatGain(0.04, solarRadiation);
        ASSERT_TRUE(solidLayer != nullptr);

        auto gapThickness = 0.05;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gapLayer != nullptr);

        auto shadeLayerConductance = 0.12;
        // make cell geometry
        const auto thickness_31111{0.00023};
        const auto x = 0.00169;        // m
        const auto y = 0.00169;        // m
        const auto radius = 0.00058;   // m
        const auto CellDimension{
          ThermalPermeability::Perforated::diameterToXYDimension(2 * radius)};
        const auto frontOpenness{ThermalPermeability::Perforated::openness(
          ThermalPermeability::Perforated::Geometry::Circular,
          x,
          y,
          CellDimension.x,
          CellDimension.y)};
        const auto dl{0.0};
        const auto dr{0.0};
        const auto dtop{0.0};
        const auto dbot{0.0};
        EffectiveLayers::ShadeOpenness openness{frontOpenness, dl, dr, dtop, dbot};
        EffectiveLayers::EffectiveLayerPerforated effectiveLayerPerforated{
          windowWidth, windowHeight, thickness_31111, openness};
        EffectiveLayers::EffectiveOpenness effOpenness{
          effectiveLayerPerforated.getEffectiveOpenness()};
        const auto effectiveThickness{effectiveLayerPerforated.effectiveThickness()};
        auto Ef = 0.640892;
        auto Eb = 0.623812;
        auto Tirf = 0.257367;
        auto Tirb = 0.257367;
        auto shadeLayer = Tarcog::ISO15099::Layers::shading(
          effectiveThickness, shadeLayerConductance, effOpenness, Ef, Tirf, Eb, Tirb);
        shadeLayer->setSolarHeatGain(0.35, solarRadiation);
        ASSERT_TRUE(shadeLayer != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({solidLayer, gapLayer, shadeLayer});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        
        m_TarcogSystem = std::make_unique<CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] CIGUSolidLayer * GetFirstLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[0].get();
    };

    [[nodiscard]] CIGUGapLayer * GetGap() const
    {
        return m_TarcogSystem->getGapLayers()[0].get();
    };

    [[nodiscard]] CIGUSolidLayer * GetSecondLayer() const
    {
        return m_TarcogSystem->getSolidLayers()[1].get();
    };
};

TEST_F(TestGapBetweenIrradiatedGlassAndInteriorShadingNaturalConvection, GapLayerSurfaceIRFlow)
{
    SCOPED_TRACE("Begin Test: Test gap layer surface temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Front);
    auto backIRRadiationFlow = aLayer->J(FenestrationCommon::Side::Back);
    EXPECT_NEAR(503.2725415073902, frontIRRadiationFlow, 1e-4);
    EXPECT_NEAR(571.43594632791894, backIRRadiationFlow, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassAndInteriorShadingNaturalConvection, GainEnergy)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gain Energy");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR(-43.832687161547518, gainEnergy, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassAndInteriorShadingNaturalConvection, FirstLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Solid Temperatures");

    auto aLayer = GetFirstLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(304.60903517150138, frontTemperature, 1e-4);
    EXPECT_NEAR(304.94605922753459, backTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassAndInteriorShadingNaturalConvection, GapTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Gap Temperatures");

    auto aLayer = GetGap();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    auto layerTemperature = aLayer->averageLayerTemperature();
    auto averageTemperature = aLayer->averageSurfaceTemperature();
    EXPECT_NEAR(304.94605922753459, frontTemperature, 1e-4);
    EXPECT_NEAR(324.90604783261381, backTemperature, 1e-4);
    EXPECT_NEAR(309.99149481933722, layerTemperature, 1e-4);
    EXPECT_NEAR(314.92605353007423, averageTemperature, 1e-4);
}

TEST_F(TestGapBetweenIrradiatedGlassAndInteriorShadingNaturalConvection, SecondLayerSurfaceTemperatures)
{
    SCOPED_TRACE("Begin Test: Test Forced Ventilated Gap Layer At Edge - Shade Temperatures");

    auto aLayer = GetSecondLayer();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision

    ASSERT_TRUE(aLayer != nullptr);
    auto frontTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Front);
    auto backTemperature = aLayer->surfaceTemperature(FenestrationCommon::Side::Back);
    EXPECT_NEAR(324.90604783261381, frontTemperature, 1e-4);
    EXPECT_NEAR(324.83549933079962, backTemperature, 1e-4);
}