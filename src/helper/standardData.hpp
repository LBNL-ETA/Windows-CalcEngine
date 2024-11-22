#pragma once

#include <WCECommon.hpp>

namespace StandardData
{
    [[nodiscard]] std::vector<double> condensedSpectrumDefault();
    [[nodiscard]] FenestrationCommon::CSeries solarRadiationASTM_E891_87_Table1();

    namespace Photopic
    {
        [[nodiscard]] std::vector<double> wavelengthSetPhotopic();

        // Full CIE Illuminant D651 nm ssp table (used for PHOTOPIC properties)
        [[nodiscard]] FenestrationCommon::CSeries solarRadiation();
        [[nodiscard]] FenestrationCommon::CSeries detectorData();
    }   // namespace Photopic

}   // namespace StandardData