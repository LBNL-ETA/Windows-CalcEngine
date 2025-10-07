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
    }   // namespace Winter
    namespace Summer
    {
        std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor();
        std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor();
    }   // namespace Summer
}   // namespace Environment::NFRC

namespace IGU::NFRC
{
    // --- Optics metadata bound to IGU type
    struct Optics
    {
        double tVis;
        double tSol;
    };

    // --- IGU preset that carries both geometry/material and optics
    struct Preset
    {
        Tarcog::ISO15099::CIGU igu;
        Optics optics;
    };

    // Default double clear air from the database (NFRC 102 - NFRC 102 with 12.7 mm Air gap)
    Preset doubleClearAir();
}   // namespace IGU::NFRC

namespace System
{
    // Construct CSystem from an IGU preset and two environments
    std::shared_ptr<Tarcog::ISO15099::CSystem>
      make(const IGU::NFRC::Preset & preset,
           const std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> & indoor,
           const std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> & outdoor);
}   // namespace System

namespace Window
{
    // Construct DualVision window given two systems and their optics
    Tarcog::ISO15099::DualVisionHorizontal
      makeDualVisionHorizontal(double width,
                               double height,
                               const std::shared_ptr<Tarcog::ISO15099::CSystem> & left,
                               const IGU::NFRC::Optics & leftOptics,
                               const std::shared_ptr<Tarcog::ISO15099::CSystem> & right,
                               const IGU::NFRC::Optics & rightOptics);

    Tarcog::ISO15099::DualVisionHorizontal
      withDefaultDualHorizontalFrames(const Tarcog::ISO15099::DualVisionHorizontal & window);
}   // namespace Window