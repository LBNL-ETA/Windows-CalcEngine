#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>


SingleLayerOptics::CVenetianCellDescription createCellDescription()
    {
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0;
        FenestrationCommon::VenetianGeometry geometry{
          slatWidth, slatSpacing, slatTiltAngle, curvatureRadius};
        const size_t numOfSlatSegments = 1;

        return {geometry, numOfSlatSegments};
    }

TEST(TestVenetianCellFlat0_5, BeamViewFactorsFront)
{
    auto aCell{createCellDescription()};

    const auto vfs{aCell.cellBeamViewFactors(18, FenestrationCommon::Side::Front)};

    EXPECT_EQ(1u, vfs.size());

    EXPECT_EQ(1u, vfs[0].enclosureIndex);
    EXPECT_EQ(0u, vfs[0].segmentIndex);
    EXPECT_NEAR(1.0, vfs[0].value, 1e-6);
    EXPECT_NEAR(0.800547, vfs[0].percentHit, 1e-6);
}

TEST(TestVenetianCellFlat0_5, BeamViewFactorsBack)
{
    auto aCell{createCellDescription()};

    const auto vfs{aCell.cellBeamViewFactors(-18, FenestrationCommon::Side::Back)};

    EXPECT_EQ(1u, vfs.size());

    EXPECT_EQ(0u, vfs[0].enclosureIndex);
    EXPECT_EQ(0u, vfs[0].segmentIndex);
    EXPECT_NEAR(1.0, vfs[0].value, 1e-6);
    EXPECT_NEAR(0.800547, vfs[0].percentHit, 1e-6);
}