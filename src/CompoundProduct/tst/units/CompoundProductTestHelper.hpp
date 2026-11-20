#pragma once

#include <vector>

#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCECompoundProduct.hpp"

namespace CompoundProductTest
{
    [[nodiscard]] inline FenestrationCommon::CSeries series(const std::vector<double> & wavelengths,
                                                            const std::vector<double> & values)
    {
        FenestrationCommon::CSeries result;
        for(size_t index = 0; index < wavelengths.size(); ++index)
        {
            result.addProperty(wavelengths[index], values[index]);
        }
        return result;
    }

    inline void expectNear(const FenestrationCommon::CSeries & actual,
                           const std::vector<double> & expected,
                           const double tolerance)
    {
        ASSERT_EQ(actual.size(), expected.size());
        for(size_t index = 0; index < expected.size(); ++index)
        {
            EXPECT_NEAR(actual[index].value(), expected[index], tolerance)
              << "at wavelength " << actual[index].x();
        }
    }

    inline void expectNear(const FenestrationCommon::CSeries & actual,
                           const FenestrationCommon::CSeries & expected,
                           const double tolerance)
    {
        expectNear(actual, expected.getYArray(), tolerance);
    }

    inline void expectNear(const CompoundProduct::Interface & actual,
                           const CompoundProduct::Interface & expected,
                           const double tolerance)
    {
        expectNear(actual.transmittance, expected.transmittance, tolerance);
        expectNear(actual.reflectanceFront, expected.reflectanceFront, tolerance);
        expectNear(actual.reflectanceBack, expected.reflectanceBack, tolerance);
    }

    [[nodiscard]] inline bool hasWarning(const std::vector<CompoundProduct::Diagnostic> & diagnostics,
                                         const double wavelength,
                                         const CompoundProduct::Warning warning)
    {
        for(const auto & diagnostic : diagnostics)
        {
            if(diagnostic.wavelength == wavelength && diagnostic.warning == warning)
            {
                return true;
            }
        }
        return false;
    }
}   // namespace CompoundProductTest
