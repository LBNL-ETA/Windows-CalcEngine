#pragma once

#include <optional>

#include <WCECommon.hpp>

namespace DeflectionData
{
    Table::Table2D<std::optional<double>> getWNData();

    Table::Table2D<std::optional<double>> getVNData();    
}
