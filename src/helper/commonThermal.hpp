#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Definitions of common boundary conditions, IGUs, and windows
/// to be used across tests.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>

#include <WCETarcog.hpp>

// Standard NFRC boundary conditions as used in WINDOW
namespace Environment::NFRC
{
    namespace Winter
    {
        std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor();
        std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor();
    }
    namespace Summer
    {
        std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor();
        std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor();
    }
}   // namespace Environment

namespace IGU::NFRC
{
    // Default double clear air from the database (NFRC 102 - NFRC 102 with 12.7 mm Air gap)
    Tarcog::ISO15099::CIGU doubleClearAir();
}