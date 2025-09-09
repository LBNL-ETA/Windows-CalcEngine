#pragma once

#include <WCESpectralAveraging.hpp>

namespace SpectralSample
{
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_102();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_103();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_913();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_1042();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_2191();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_2600();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_5439();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_6046();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_20243();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_21467();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_21515();

    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_31100();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_31111();

    std::shared_ptr<SpectralAveraging::CSpectralSampleData> NFRC_VE348();

    std::shared_ptr<SpectralAveraging::CSpectralSampleData> DirectDiffuse();
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> ILC2019_1();
}