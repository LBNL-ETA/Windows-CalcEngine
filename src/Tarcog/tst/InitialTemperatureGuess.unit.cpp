#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"


using namespace Tarcog;
using namespace FenestrationCommon;

class TestTemperatureInitialGuess : public testing::Test
{
private:
    std::shared_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto tSky = airTemperature;
        auto pressure = 101325.0;   // Pascals
        auto airSpeed = 5.5;        // meters per second
        auto solarRadiation = 0.0;

        auto Outdoor = Environments::outdoor(airTemperature,
                                             pressure,
                                             airSpeed,
                                             solarRadiation,
                                             tSky,
                                             SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        auto roomTemperature = 294.15;
        auto Indoor = Environments::indoor(roomTemperature, pressure);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;
        auto solidLayer1 = Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(solidLayer1 != nullptr);
        auto solidLayer2 = Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(solidLayer2 != nullptr);

        auto gapThickness = 0.012;
        auto gapPressure = 101325.0;
        auto gapLayer = Layers::gap(gapThickness, gapPressure);
        ASSERT_TRUE(gapLayer != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        CIGU aTarIGU(windowWidth, windowHeight);
        aTarIGU.addLayer(solidLayer1);
        aTarIGU.addLayer(gapLayer);
        aTarIGU.addLayer(solidLayer2);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<CSingleSystem>(aTarIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    std::shared_ptr<CIGUSolidLayer> getLayer1() const
    {
        return m_TarcogSystem->getSolidLayers()[0];
    }

    std::shared_ptr<CIGUSolidLayer> getLayer2() const
    {
        return m_TarcogSystem->getSolidLayers()[1];
    }
};

TEST_F(TestTemperatureInitialGuess, Test1)
{
    SCOPED_TRACE("Begin Test: Initial temperature and IR guess");

    auto layer = getLayer1();

    auto temperature = layer->getTemperature(Side::Front);
    auto J = layer->J(Side::Front);
    EXPECT_NEAR(256.282733081615, temperature, 1e-6);
    EXPECT_NEAR(244.589307222020, J, 1e-6);

    temperature = layer->getTemperature(Side::Back);
    J = layer->J(Side::Back);
    EXPECT_NEAR(262.756302643044, temperature, 1e-6);
    EXPECT_NEAR(270.254322031419, J, 1e-6);

    layer = getLayer2();
    temperature = layer->getTemperature(Side::Front);
    J = layer->J(Side::Front);
    EXPECT_NEAR(276.349099622422, temperature, 1e-6);
    EXPECT_NEAR(330.668096601357, J, 1e-6);

    temperature = layer->getTemperature(Side::Back);
    J = layer->J(Side::Back);
    EXPECT_NEAR(282.822669183851, temperature, 1e-6);
    EXPECT_NEAR(362.757956247504, J, 1e-6);
}
