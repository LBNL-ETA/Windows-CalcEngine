#include <gtest/gtest.h>

#include <vector>

#include <WCETarcog.hpp>

class TestDeflectionE1300_TripleLayer2 : public testing::Test
{
private:
    double m_Width{1.0};
    double m_Height{1.0};

    std::vector<Deflection::LayerData> m_Layer{{0.00556}, {0.00742}, {0.00556}};
    std::vector<Deflection::GapData> m_Gap{{0.0127, 273.15 + 30, 101000},
                                           {0.0127, 273.15 + 30, 101000}};

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

TEST_F(TestDeflectionE1300_TripleLayer2, Deflection1)
{
    auto def{getDefObject()};

    const std::vector<double> loadTemperatures{22 + 273.15, 21 + 273.15};

    def.setIGUTilt(90);
    def.setInteriorPressure(101000);
    def.setExteriorPressure(101000);
    def.setAppliedLoad({0, 0, 0});

    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{6.278315e-07};
    EXPECT_NEAR(error.value(), correctError, 1e-12);

    const std::vector<double> correctDeflection{-0.735948e-3, -0.022103e-03, 0.785073e-3};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }
}