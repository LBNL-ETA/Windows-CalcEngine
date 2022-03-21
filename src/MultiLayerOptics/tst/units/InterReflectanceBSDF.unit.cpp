#include <memory>
#include <gtest/gtest.h>

#include "WCEMultiLayerOptics.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using FenestrationCommon::SquareMatrix;
using SingleLayerOptics::CBSDFDefinition;
using SingleLayerOptics::BSDFDirection;
using SingleLayerOptics::CBSDFDirections;

// Example that tests interreflectance between two adjacent layers. This procedure will be used to
// calculate other multilayer properties
class TestInterReflectanceBSDF : public testing::Test
{
private:
    FenestrationCommon::SquareMatrix m_InterReflectance;

protected:
    virtual void SetUp()
    {
        // Create lambda matrix
        std::vector<CBSDFDefinition> aDefinitions;
        aDefinitions.emplace_back(0, 1);
        aDefinitions.emplace_back(15, 1);
        aDefinitions.emplace_back(30, 1);
        aDefinitions.emplace_back(45, 1);
        aDefinitions.emplace_back(60, 1);
        aDefinitions.emplace_back(75, 1);
        aDefinitions.emplace_back(86.25, 1);

        const auto aDirections{CBSDFDirections(aDefinitions, BSDFDirection::Incoming)};
        const auto aLambdas{aDirections.lambdaMatrix()};

        SquareMatrix Rb{{1.438618083, 0, 0, 0, 0, 0, 0},
                        {0, 0.189397664, 0, 0, 0, 0, 0},
                        {0, 0, 0.112189021, 0, 0, 0, 0},
                        {0, 0, 0, 0.114376511, 0, 0, 0},
                        {0, 0, 0, 0, 0.207336671, 0, 0},
                        {0, 0, 0, 0, 0, 0.951907739, 0},
                        {0, 0, 0, 0, 0, 0, 15.28298172}};

        SquareMatrix Rf{{1.438618083, 0, 0, 0, 0, 0, 0},
                        {0, 0.189397664, 0, 0, 0, 0, 0},
                        {0, 0, 0.112189021, 0, 0, 0, 0},
                        {0, 0, 0, 0.114376511, 0, 0, 0},
                        {0, 0, 0, 0, 0.207336671, 0, 0},
                        {0, 0, 0, 0, 0, 0.951907739, 0},
                        {0, 0, 0, 0, 0, 0, 15.28298172}};

        m_InterReflectance = MultiLayerOptics::interReflectance(aLambdas, Rb, Rf);
    }

public:
    SquareMatrix getInterReflectance()
    {
        return m_InterReflectance;
    };
};

TEST_F(TestInterReflectanceBSDF, TestBSDFInterreflectance)
{
    SCOPED_TRACE("Begin Test: Simple BSDF interreflectance.");

    const auto results{getInterReflectance()};

    const size_t matrixSize{results.size()};

    // Test matrix
    constexpr size_t size{7u};

    EXPECT_EQ(size, matrixSize);

    const SquareMatrix correctResults{{1.005964363, 0, 0, 0, 0, 0, 0},
                                      {0, 1.005964363, 0, 0, 0, 0, 0},
                                      {0, 0, 1.006280195, 0, 0, 0, 0},
                                      {0, 0, 0, 1.008724458, 0, 0, 0},
                                      {0, 0, 0, 0, 1.021780268, 0, 0},
                                      {0, 0, 0, 0, 0, 1.176150952, 0},
                                      {0, 0, 0, 0, 0, 0, 3.022280250}};

    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; j < size; ++j)
        {
            EXPECT_NEAR(correctResults(i, j), results(i, j), 1e-6);
        }
    }
}
