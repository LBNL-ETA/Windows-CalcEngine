/// commonFrames.hpp

//////////////////////////////////////////////////////////////////////
/// Contains commonly used frame in WINDOW 7.8 database.
/// Data are taken from the database so that final test results
/// could be compared with WINDOW results
//////////////////////////////////////////////////////////////////////

#pragma once

#include <WCETarcog.hpp>

namespace Frame
{
    Tarcog::ISO15099::FrameData sampleSill();
    Tarcog::ISO15099::FrameData sampleHead();
    Tarcog::ISO15099::FrameData sampleJamb();
    Tarcog::ISO15099::FrameData genericFrameClass1();
    Tarcog::ISO15099::FrameData genericDividerAluminumHollow();
}   // namespace Frames