#include <gtest/gtest.h>

#include <vector>

#include <WCETarcog.hpp>

class TestDeflectionE1300_TripleLayer : public testing::Test
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

TEST_F(TestDeflectionE1300_TripleLayer, ZeroDeflection)
{
    const auto width{1.0};
    const auto height{1.0};
    const std::vector<Deflection::LayerData> layer{{0.00556}, {0.00742}, {0.00556}};
    const std::vector<Deflection::GapData> gap{{0.0127, 273.15 + 21}, {0.0127, 273.15 + 21}};
    Deflection::DeflectionE1300 def(width, height, layer, gap);

    const std::vector<double> loadTemperatures{21 + 273.15, 21 + 273.15};
    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};
    const auto panesLoad{res.paneLoad};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{-3.686496e-07};
    EXPECT_NEAR(error.value(), correctError, 1e-9);

    const std::vector<double> correctDeflection{0, 0, 0};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }

    const std::vector<double> correctPanesLoad{0, 0, 0};
    for(size_t i = 0u; i < correctPanesLoad.size(); ++i)
    {
        EXPECT_NEAR(correctPanesLoad[i], panesLoad[i], 1e-6);
    }
}

TEST_F(TestDeflectionE1300_TripleLayer, DeflectionSquaredWindow)
{
    const auto width{1.0};
    const auto height{1.0};
    const std::vector<Deflection::LayerData> layer{{0.00556}, {0.00742}, {0.00556}};
    const std::vector<Deflection::GapData> gap{{0.0127, 273.15 + 21}, {0.0127, 273.15 + 21}};
    Deflection::DeflectionE1300 def(width, height, layer, gap);

    const std::vector<double> loadTemperatures{22 + 273.15, 21 + 273.15};
    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};
    const auto panesLoad{res.paneLoad};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{5.582482e-05};
    EXPECT_NEAR(error.value(), correctError, 1e-9);

    const std::vector<double> correctDeflection{0.07465884e-03, -0.022613683e-03, -0.02094219e-03};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }

    const std::vector<double> correctPanesLoad{19.883320, -14.308254, -5.575066};
    for(size_t i = 0u; i < correctPanesLoad.size(); ++i)
    {
        EXPECT_NEAR(correctPanesLoad[i], panesLoad[i], 1e-6);
    }
}

TEST_F(TestDeflectionE1300_TripleLayer, DeflectionDifferentWidthAndHeight)
{
    const auto width{1.0};
    const auto height{2.5};
    const std::vector<Deflection::LayerData> layer{{0.00556}, {0.00742}, {0.00556}};
    const std::vector<Deflection::GapData> gap{{0.0127, 273.15 + 21}, {0.0127, 273.15 + 21}};
    Deflection::DeflectionE1300 def(width, height, layer, gap);

    const std::vector<double> loadTemperatures{22 + 273.15, 21 + 273.15};
    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};
    const auto panesLoad{res.paneLoad};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{-5.033322e-06};
    EXPECT_NEAR(error.value(), correctError, 1e-9);

    const std::vector<double> correctDeflection{0.072674289e-03, -0.02168655e-03, -0.02113032e-03};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }

    const std::vector<double> correctPanesLoad{6.844653, -4.854545, -1.990108};
    for(size_t i = 0u; i < correctPanesLoad.size(); ++i)
    {
        EXPECT_NEAR(correctPanesLoad[i], panesLoad[i], 1e-6);
    }
}

TEST_F(TestDeflectionE1300_TripleLayer, DeflectionDifferentInteriorAndExteriorPressure)
{
    const auto width{1.0};
    const auto height{2.5};
    const std::vector<Deflection::LayerData> layer{{0.00556}, {0.00742}, {0.00556}};
    const std::vector<Deflection::GapData> gap{{0.0127, 273.15 + 21}, {0.0127, 273.15 + 21}};
    Deflection::DeflectionE1300 def(width, height, layer, gap);

    const double exteriorPressure{102325};
    def.setExteriorPressure(exteriorPressure);

    const std::vector<double> loadTemperatures{22 + 273.15, 21 + 273.15};
    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};
    const auto panesLoad{res.paneLoad};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{-5.196203e-4};
    EXPECT_NEAR(error.value(), correctError, 1e-9);

    const std::vector<double> correctDeflection{-2.408286e-3, -2.317540e-3, -2.242010e-3};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }

    const std::vector<double> correctPanesLoad{-238.541713, -539.835780, -221.622507};
    for(size_t i = 0u; i < correctPanesLoad.size(); ++i)
    {
        EXPECT_NEAR(correctPanesLoad[i], panesLoad[i], 1e-6);
    }
}

TEST_F(TestDeflectionE1300_TripleLayer, DeflectionWithTiltAngle)
{
    const auto width{1.0};
    const auto height{2.5};
    const std::vector<Deflection::LayerData> layer{{0.00556}, {0.00742}, {0.00556}};
    const std::vector<Deflection::GapData> gap{{0.0127, 273.15 + 21}, {0.0127, 273.15 + 21}};
    Deflection::DeflectionE1300 def(width, height, layer, gap);

    const auto tiltAngle{45.0};
    def.setIGUTilt(tiltAngle);

    const std::vector<double> loadTemperatures{22 + 273.15, 21 + 273.15};
    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};
    const auto panesLoad{res.paneLoad};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{-2.177694e-4};
    EXPECT_NEAR(error.value(), correctError, 1e-9);

    const std::vector<double> correctDeflection{-0.261122e-3, -0.337247e-3, -0.302394e-3};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }

    const std::vector<double> correctPanesLoad{-24.606516, -75.533516, -28.496727};
    for(size_t i = 0u; i < correctPanesLoad.size(); ++i)
    {
        EXPECT_NEAR(correctPanesLoad[i], panesLoad[i], 1e-6);
    }
}

TEST_F(TestDeflectionE1300_TripleLayer, DeflectionWithAppliedLoad)
{
    const auto width{1.0};
    const auto height{2.5};
    const std::vector<Deflection::LayerData> layer{{0.00556}, {0.00742}, {0.00556}};
    const std::vector<Deflection::GapData> gap{{0.0127, 273.15 + 21}, {0.0127, 273.15 + 21}};
    Deflection::DeflectionE1300 def(width, height, layer, gap);

    const std::vector<double> appliedLoad{1500, 0, 0};
    def.setAppliedLoad(appliedLoad);

    const std::vector<double> loadTemperatures{22 + 273.15, 21 + 273.15};
    def.setLoadTemperatures(loadTemperatures);

    const auto res{def.results()};
    const auto error{res.error};
    const auto deflection{res.deflection};
    const auto panesLoad{res.paneLoad};

    ASSERT_EQ(error.has_value(), true);

    const auto correctError{-2.908520e-06};
    EXPECT_NEAR(error.value(), correctError, 1e-9);

    const std::vector<double> correctDeflection{-3.619900e-3, -3.420178e-3, -3.323497e-3};
    for(size_t i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], 1e-9);
    }

    const std::vector<double> correctPanesLoad{-361.828081, -806.503990, -331.667929};
    for(size_t i = 0u; i < correctPanesLoad.size(); ++i)
    {
        EXPECT_NEAR(correctPanesLoad[i], panesLoad[i], 1e-6);
    }
}