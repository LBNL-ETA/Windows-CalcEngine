// commonFrames.cpp

#include "commonFrames.hpp"

namespace Frame
{
    Tarcog::ISO15099::FrameData sampleSill()
    {
        return {.UValue = 2.017913,
                .EdgeUValue = 2.339592,
                .ProjectedFrameDimension = 0.04287518,
                .WettedLength = 0.05633283,
                .Absorptance = 0.3};
    }

    Tarcog::ISO15099::FrameData sampleHead()
    {
        return {.UValue = 2.024318,
                .EdgeUValue = 2.34355,
                .ProjectedFrameDimension = 0.04287518,
                .WettedLength = 0.05674332,
                .Absorptance = 0.3};
    }

    Tarcog::ISO15099::FrameData sampleJamb()
    {
        return {.UValue = 1.943306,
                .EdgeUValue = 2.358972,
                .ProjectedFrameDimension = 0.04287518,
                .WettedLength = 0.04122826,
                .Absorptance = 0.3};
    }

    Tarcog::ISO15099::FrameData genericFrameClass1()
    {
        constexpr double projectedFrameDimension{0.05715};

        //! This is class 1 generic frame (as defined in WINDOW database)
        return {.UValue = 5.68,
                .EdgeUValue = 0.0,
                .ProjectedFrameDimension = projectedFrameDimension,
                .WettedLength = projectedFrameDimension,
                .Absorptance = 0.9,
                .Class = Tarcog::ISO15099::GenericFrame{{2.33, -0.01, 0.138, 0, 0}}};
    }

    Tarcog::ISO15099::FrameData genericDividerAluminumHollow()
    {
        constexpr double projectedFrameDimension{0.01588};

        // Data pulled from WINDOW database, record 1
        return {.UValue = 0.18,
                .EdgeUValue = 3.0,
                .ProjectedFrameDimension = projectedFrameDimension,
                .WettedLength = projectedFrameDimension,
                .Absorptance = 0.9,
                .Class = Tarcog::ISO15099::GenericDivider{
                  .EdgePoly = {1.19, 0.0, 0.73, 0.009},
                  .BodyPoly = {5.56, 0.0004, -0.00003, 0.042, -0.003}}};
    }

    Tarcog::ISO15099::FrameData sillTRR97()
    {
        return {.UValue = 2.48396,
                .EdgeUValue = 2.35195,
                .ProjectedFrameDimension = 0.050014,
                .WettedLength = 0.085128,
                .Absorptance = 0.3};
    }

    Tarcog::ISO15099::FrameData headTRR97()
    {
        return {.UValue = 2.50035,
                .EdgeUValue = 2.36047,
                .ProjectedFrameDimension = 0.050014,
                .WettedLength = 0.085128,
                .Absorptance = 0.3};
    }

    Tarcog::ISO15099::FrameData jambTRR97()
    {
        return {.UValue = 2.49937,
                .EdgeUValue = 2.35603,
                .ProjectedFrameDimension = 0.050014,
                .WettedLength = 0.085128,
                .Absorptance = 0.3};
    }

    Tarcog::ISO15099::FrameData sampleDivider()
    {
        return {.UValue = 1.91314399242401,
                .EdgeUValue = 1.60704600811005,
                .ProjectedFrameDimension = 0.0253999996185303,
                .WettedLength = 0.0253999996185303,
                .Absorptance = 0.3};
    }
}   // namespace Frame