// commonThermal.cpp

#include "commonThermal.hpp"

namespace Environment::NFRC::Winter
{
    std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor()
    {
        constexpr auto airTemperature = 255.15;   // Kelvins
        constexpr auto airSpeed = 5.5;            // meters per second
        constexpr auto tSky = 255.15;             // Kelvins
        constexpr auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        return Outdoor;
    }

    std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor()
    {
        constexpr auto roomTemperature = 294.15;
        return Tarcog::ISO15099::Environments::indoor(roomTemperature);
    }
}   // namespace Environment::NFRC::Winter

namespace Environment::NFRC::Summer
{
    std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor()
    {
        constexpr auto airTemperature = 305.15;   // Kelvins
        constexpr auto airSpeed = 2.75;           // meters per second
        constexpr auto tSky = 305.15;             // Kelvins
        constexpr auto solarRadiation = 783.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        return Outdoor;
    }

    std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor()
    {
        constexpr auto roomTemperature = 297.15;
        return Tarcog::ISO15099::Environments::indoor(roomTemperature);
    }
}   // namespace Environment::NFRC::Summer

namespace IGU::NFRC
{
    Tarcog::ISO15099::CIGU doubleClearAir()
    {
        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness = 0.005715;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        constexpr auto solarAbsorptance1 = 0.166707709432;
        constexpr auto solarAbsorptance2 = 0.112737670541;

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer1->setSolarAbsorptance(solarAbsorptance1);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer2->setSolarAbsorptance(solarAbsorptance2);

        constexpr auto gapThickness = 0.012;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);

        constexpr auto windowWidth = 1.2;
        constexpr auto windowHeight = 1.5;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        return aIGU;
    }
}