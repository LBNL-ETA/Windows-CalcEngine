#include <gtest/gtest.h>

#include <WCETarcog.hpp>

TEST(TestErrorHandlingSolidLayer, SolidLayerThickness)
{
    SCOPED_TRACE("Begin Test: Solid layer negative thickness.");

    EXPECT_THROW(
        Tarcog::ISO15099::CIGUSolidLayer(-1.0, 1.0, nullptr, nullptr),
        std::runtime_error
    );
}

TEST(TestErrorHandlingSolidLayer, SolidLayerConductivity)
{
    SCOPED_TRACE("Begin Test: Solid layer negative conductivity.");

    EXPECT_THROW(
        Tarcog::ISO15099::CIGUSolidLayer(1.0, -1.0, nullptr, nullptr),
        std::runtime_error
    );
}

TEST(TestErrorHandlingSolidLayer, SolidLayerFrontEmissivity)
{
    SCOPED_TRACE("Begin Test: Solid layer negative front emissivity.");

    EXPECT_THROW(
        Tarcog::ISO15099::CIGUSolidLayer(1.0, 1.0, -1.0, 0.0, 0.0, 0.0),
        std::runtime_error
    );
}

TEST(TestErrorHandlingSolidLayer, SolidLayerFrontIRTransmittance)
{
    SCOPED_TRACE("Begin Test: Solid layer negative front IR transmittance.");

    EXPECT_THROW(
        Tarcog::ISO15099::CIGUSolidLayer(1.0, 1.0, 1.0, -1.0, 0.0, 0.0),
        std::runtime_error
    );
}