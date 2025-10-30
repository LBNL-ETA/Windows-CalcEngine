#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

// This test is created to accommodate same test in WC so we can confirm the results (Shading
// material is White Venetian - NFRC 31100)
class TestShadeInVenetian : public testing::Test
{
protected:
    static std::unique_ptr<Tarcog::ISO15099::CSingleSystem> buildDoubleLayerSystem()
    {
        using namespace Tarcog::ISO15099;

        auto roomTemperature = 294.15;
        auto Indoor = Environments::indoor(roomTemperature);
        assert(Indoor != nullptr);

        auto airTemperature = 255.15;
        auto airSpeed = 5.5;
        auto tSky = 255.15;
        auto solarRadiation = 0.0;
        auto Outdoor = Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, SkyModel::AllSpecified);
        assert(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::CalculateH);

        auto emissivityFront = 0.52204944036998135;
        auto emissivityBack = 0.52204944036998102;
        auto transmittanceFront = 0.44074853220969445;
        auto transmittanceBack = 0.44074853220969445;

        auto materialThickness = 0.0001;
        auto materialConductance = 160.0;

        FenestrationCommon::Venetian::Geometry geometry{0.05, 0.07, 45, 0.00};
        const auto effectiveVenetian{
          EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry)};

        auto shadingLayer = Layers::shading(effectiveVenetian.thickness,
                                            materialConductance,
                                            effectiveVenetian.openness,
                                            emissivityFront,
                                            transmittanceFront,
                                            emissivityBack,
                                            transmittanceBack);
        assert(shadingLayer != nullptr);

        auto solidLayer = Layers::solid(0.003048, 1.0, 0.84, 0.0, 0.84, 0.0);
        auto gap = Layers::gap(0.0127);

        CIGU igu{1.0, 1.0};
        igu.addLayers({solidLayer, gap, shadingLayer});

        auto system = std::make_unique<CSingleSystem>(igu, Indoor, Outdoor);
        system->solve();
        return system;
    }

    static std::unique_ptr<Tarcog::ISO15099::CSingleSystem>
      buildSingleLayerSystemWithModifier(const Tarcog::ISO15099::ShadingModifier & modifier)
    {
        using namespace Tarcog::ISO15099;

        auto roomTemperature = 294.15;
        auto Indoor = Environments::indoor(roomTemperature);
        constexpr auto calculatedHcFromDoubleRun{2.6612782178108558};
        Indoor->setHCoeffModel(BoundaryConditionsCoeffModel::HcPrescribed,
                               modifier.hcRatio * calculatedHcFromDoubleRun);
        assert(Indoor != nullptr);

        auto airTemperature = 255.15;
        auto airSpeed = 5.5;
        auto tSky = 255.15;
        auto solarRadiation = 0.0;
        auto Outdoor = Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, SkyModel::AllSpecified);
        assert(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::CalculateH);

        auto solidLayer =
          Layers::solid(0.003048, 1.0, 0.84, 0.0, modifier.emissivityRatio * 0.84, 0.0);
        assert(solidLayer != nullptr);

        CIGU igu{1.0, 1.0};
        igu.addLayers({solidLayer});

        auto system = std::make_unique<CSingleSystem>(igu, Indoor, Outdoor);
        system->solve();
        return system;
    }
};

TEST_F(TestShadeInVenetian, Test1)
{
    SCOPED_TRACE("Begin Test: ShadeOutVenetian - U-value");

    constexpr auto Tolerance = 1e-6;

    const auto aDoubleSystem = buildDoubleLayerSystem();

    EXPECT_NEAR(4.586092, aDoubleSystem->getUValue(), Tolerance);

    const std::vector correctTemperatureDouble{261.258561, 261.803719, 282.651541, 282.922999};
    Helper::testVectors("Temperatures with shading",
                        correctTemperatureDouble,
                        aDoubleSystem->getTemperatures(),
                        Tolerance);

    const std::vector correctRadiosityDouble{260.328693, 285.599331, 386.624101, 331.314667};
    Helper::testVectors("Radiosities with shading",
                        correctRadiosityDouble,
                        aDoubleSystem->getRadiosities(),
                        Tolerance);

    auto aModifier = aDoubleSystem->getShadingModifier(Tarcog::ISO15099::Environment::Indoor);

    EXPECT_NEAR(0.995714, aModifier.hcRatio, Tolerance);
    EXPECT_NEAR(0.701355, aModifier.emissivityRatio, Tolerance);

    const auto aSingleSystem = buildSingleLayerSystemWithModifier(aModifier);

    EXPECT_NEAR(4.586092, aSingleSystem->getUValue(), Tolerance);

    // Temperatures of the glass should perfectly match with temperatures of the glass from
    // double layer run
    const std::vector correctTemperatureSingle{261.258561, 261.803719};
    Helper::testVectors("Temperatures with shading modifier",
                        correctTemperatureSingle,
                        aSingleSystem->getTemperatures(),
                        Tolerance);

    // Radiosities should perfectly match first and last surface radiosity from the dual layer run
    const std::vector correctRadiositySingle{260.328693, 331.314667};
    Helper::testVectors("Radiosities with shading modifier",
                        correctRadiositySingle,
                        aSingleSystem->getRadiosities(),
                        Tolerance);
}
