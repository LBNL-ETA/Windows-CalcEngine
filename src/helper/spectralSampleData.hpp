#pragma once

#include <WCESpectralAveraging.hpp>

namespace SpectralSample
{
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_102();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_103();

    std::shared_ptr<SpectralAveraging::CSpectralSampleData> WhiteVenetianBlindSlat_31100();

    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_VE348();
}