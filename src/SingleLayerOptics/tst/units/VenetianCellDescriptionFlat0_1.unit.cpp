#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>

namespace
{
    SingleLayerOptics::CVenetianCellDescription createCellDescription()
    {
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 0;
        const auto curvatureRadius = 0;
        FenestrationCommon::VenetianGeometry geometry{
          slatWidth, slatSpacing, slatTiltAngle, curvatureRadius};
        const size_t numOfSlatSegments = 1;

        return {geometry, numOfSlatSegments};
    }
}   // namespace

TEST(TestVenetianDescriptionFlat0_1, BeamViewFactorsFrontIncoming18)
{
    auto aCell{createCellDescription()};

    const auto vfs{aCell.cellBeamViewFactors(18, FenestrationCommon::Side::Front)};

    EXPECT_EQ(1u, vfs.size());

    EXPECT_EQ(1u, vfs[0].enclosureIndex);
    EXPECT_EQ(0u, vfs[0].segmentIndex);
    EXPECT_NEAR(0.433226, vfs[0].value, 1e-6);
    EXPECT_NEAR(1.0, vfs[0].percentHit, 1e-6);
}

TEST(TestVenetianDescriptionFlat0_1, BeamViewFactorsBackIncoming18)
{
    auto aCell{createCellDescription()};

    const auto vfs{aCell.cellBeamViewFactors(18, FenestrationCommon::Side::Back)};

    EXPECT_EQ(1u, vfs.size());

    EXPECT_EQ(0u, vfs[0].enclosureIndex);
    EXPECT_EQ(0u, vfs[0].segmentIndex);
    EXPECT_NEAR(0.43322626164387507, vfs[0].value, 1e-6);
    EXPECT_NEAR(1.0, vfs[0].percentHit, 1e-6);
}