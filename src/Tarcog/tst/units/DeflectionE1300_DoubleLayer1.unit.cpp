#include <gtest/gtest.h>

#include <vector>

#include <WCETarcog.hpp>

class TestDeflectionE1300_DoubleLayer1 : public testing::Test
{
private:
    double m_Width{1.0};
    double m_Height{1.0};

    std::vector<Deflection::LayerData> m_Layer{{0.003048}, {0.00742}};
    std::vector<Deflection::GapData> m_Gap{{0.0127, 273.15 + 30}};

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

TEST_F(TestDeflectionE1300_DoubleLayer1, Deflection1)
{
    auto def{getDefObject()};

    const std::vector<double> loadTemperatures{268};

    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{9.924406e-08};
    EXPECT_NEAR(error.value(), correctError, 1e-9);

    const std::vector<double> correctDeflection{-2.469101e-3, 0.259526e-3};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }
}