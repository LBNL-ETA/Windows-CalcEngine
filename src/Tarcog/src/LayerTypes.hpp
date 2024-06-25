#pragma once

#include <memory>

namespace Tarcog::ISO15099
{
    class CIGUSolidLayer;
    using SolidLayer = std::shared_ptr<CIGUSolidLayer>;
}