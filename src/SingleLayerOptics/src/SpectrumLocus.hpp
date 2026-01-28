#pragma once

#include "WCECommon.hpp"

namespace SingleLayerOptics
{
    /// CIE 1964 10-degree observer spectrum locus chromaticity coordinates
    /// Data from 360nm to 830nm in 5nm increments (wavelengths in microns)
    /// Source: http://cvision.ucsd.edu (same as LBNL Standards .loc files)
    [[nodiscard]] FenestrationCommon::CSeries CIE_1964_Locus_X();
    [[nodiscard]] FenestrationCommon::CSeries CIE_1964_Locus_Y();

}   // namespace SingleLayerOptics
