#include <gtest/gtest.h>

#include <vector>

#include <WCETarcog.hpp>

class TestDeflectionE1300_TripleLayer1 : public testing::Test
{
private:
    double m_Width{1.0};
    double m_Height{2.5};

    std::vector<Deflection::LayerData> m_Layer{{0.00256}, {0.00742}, {0.00556}};
    std::vector<Deflection::GapData> m_Gap{{0.0127, 273.15 + 21}, {0.0127, 273.15 + 21}};

protected:
    void SetUp() override
    {}

public:
    [[nodiscard]] Deflection::DeflectionE1300 getDefObject() const
    {
        Deflection::DeflectionE1300 deflection(m_Width, m_Height, m_Layer, m_Gap);
        return deflection;
    }
};

TEST_F(TestDeflectionE1300_TripleLayer1, Deflection1)
{
    auto def{getDefObject()};

    const std::vector<double> loadTemperatures{22 + 273.15, 21 + 273.15};

    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};
    const auto pressureDifference{res.pressureDifference};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{-0.000318};
    EXPECT_NEAR(error.value(), correctError, 1e-6);

    const std::vector<double> correctDeflection{0.093280e-3, -0.002718e-3, -0.002649e-3};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }

    const std::vector<double> correctPressureDifference{0.857941, -0.608492, -0.249450};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctPressureDifference[i], pressureDifference[i], 1e-6);
    }
}