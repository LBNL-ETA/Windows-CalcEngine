#include "WholeWindow.hpp"

namespace Tarcog::ISO15099
{
    WindowSingleVision::WindowSingleVision(WindowVision vision) : vision(std::move(vision))
    {}

    double WindowSingleVision::area() const
    {
        return vision.area();
    }

    double WindowSingleVision::uValue() const
    {
        return vision.uValue();
    }

    double WindowSingleVision::shgc() const
    {
        return vision.shgc();
    }

    double WindowSingleVision::vt() const
    {
        return vision.vt();
    }
}   // namespace Tarcog::ISO15099
