#pragma once

#include <initializer_list>
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

    //! Values written in braces. Without this overload a list with one value is ambiguous for
    //! some compilers, because both a vector and a series can be created from it.
    inline void expectNear(const FenestrationCommon::CSeries & actual,
                           const std::initializer_list<double> expected,
                           const double tolerance)
    {
        expectNear(actual, std::vector<double>{expected}, tolerance);
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
        expectNear(actual.tf, expected.tf, tolerance);
        expectNear(actual.tb, expected.tb, tolerance);
        expectNear(actual.rf, expected.rf, tolerance);
        expectNear(actual.rb, expected.rb, tolerance);
    }

    inline void expectNear(const CompoundProduct::ExternalProperties & actual,
                           const CompoundProduct::ExternalProperties & expected,
                           const double tolerance)
    {
        expectNear(actual.Tf, expected.Tf, tolerance);
        expectNear(actual.Tb, expected.Tb, tolerance);
        expectNear(actual.Rf, expected.Rf, tolerance);
        expectNear(actual.Rb, expected.Rb, tolerance);
    }

    [[nodiscard]] inline bool
      hasWarning(const std::vector<CompoundProduct::Diagnostic> & diagnostics,
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
