#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianCellFlat0_4 : public testing::Test
{
private:
    CVenetianCell m_Cell{createMaterial(), createCellDescription()};

    static std::shared_ptr<CMaterial> createMaterial()
    {
        const auto Tmat = 0.0;
        const auto Rfmat = 0.9;
        const auto Rbmat = 0.9;
        return Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);
    }

protected:
    static std::shared_ptr<CVenetianCellDescription> createCellDescription()
    {
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 0;
        const auto curvatureRadius = 0;
        FenestrationCommon::VenetianGeometry geometry{
          slatWidth, slatSpacing, slatTiltAngle, curvatureRadius};
        const size_t numOfSlatSegments = 2;

        return std::make_shared<CVenetianCellDescription>(geometry, numOfSlatSegments);
    }

public:
    CVenetianCell & GetCell()
    {
        return m_Cell;
    };
};

TEST_F(TestVenetianCellFlat0_4, DiffiseDiffuse)
{
    auto & aCell{GetCell()};

    double Tdif_dif{aCell.T_dif_dif(FenestrationCommon::Side::Front)};
    double Rdif_dif{aCell.R_dif_dif(FenestrationCommon::Side::Front)};

    EXPECT_NEAR(0.58027235180123338, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.29851552698664535, Rdif_dif, 1e-6);
}

TEST_F(TestVenetianCellFlat0_4, DirectDirect)
{
    auto & aCell{GetCell()};

    constexpr auto Theta{18.0};
    constexpr auto Phi{270.0};

    const auto Tdir_dir{aCell.T_dir_dir(FenestrationCommon::Side::Front, {Theta, Phi})};

    EXPECT_NEAR(0.566774, Tdir_dir, 1e-6);
}

TEST_F(TestVenetianCellFlat0_4, DirectionalViewFactors)
{
    auto aCell{createCellDescription()};

    auto Theta{18.0};
    auto Phi{270.0};
    CBeamDirection incomingDirection = CBeamDirection(Theta, Phi);

    const auto Tdir_hem{aCell->viewFactors(FenestrationCommon::Side::Front,
                                           SingleLayerOptics::BSDFDirection::Incoming,
                                           incomingDirection)};

    const std::vector<std::vector<double>> correct{
      {0, 0, 0, 0.321232, 0.216613, 0.216613},
      {0.348612, 0, 0, 0.151388, 0.197224, 0.302776},
      {0.151388, 0, 0, 0.348612, 0.302776, 0.197224},
      {0.333333, 0.100925, 0.232408, 0, 0.232408, 0.100925},
      {0.151388, 0.197224, 0.302776, 0.348612, 0, 0},
      {0.348612, 0.302776, 0.197224, 0.151388, 0, 0}};

    for(size_t i = 0u; i < 6u; ++i)
    {
        for(size_t j = 0u; j < 6u; ++j)
        {
            EXPECT_NEAR(correct[i][j], Tdir_hem(i, j), 1e-6);
        }
    }
}

TEST_F(TestVenetianCellFlat0_4, DirectionalDiffuse)
{
    auto & aCell{GetCell()};

    // Front side
    Side aSide = Side::Front;
    auto Theta{18.0};
    auto Phi{270.0};
    CBeamDirection incomingDirection = CBeamDirection(Theta, Phi);

    Theta = 36;
    Phi = 90;
    CBeamDirection outgoingDirection = CBeamDirection(Theta, Phi);

    double Tdir_dif = aCell.T_dir_dif(aSide, incomingDirection, outgoingDirection);
    double Rdir_dif = aCell.R_dir_dif(aSide, incomingDirection, outgoingDirection);

    EXPECT_NEAR(0.113067, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.113067, Rdir_dif, 1e-6);
}

TEST_F(TestVenetianCellFlat0_4, DirectionalDiffuseSharpIncomingAngle)
{
    auto & aCell{GetCell()};

    // Front side
    Side aSide = Side::Front;
    // CBeamDirection (Theta, Phi)
    CBeamDirection incomingDirection = CBeamDirection({76.5, 270});

    CBeamDirection outgoingDirection = CBeamDirection({76.5, 90});

    double Tdir_dif = aCell.T_dir_dif(aSide, incomingDirection, outgoingDirection);
    double Rdir_dif = aCell.R_dir_dif(aSide, incomingDirection, outgoingDirection);

    EXPECT_NEAR(0.052600, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.486232, Rdir_dif, 1e-6);
}

TEST_F(TestVenetianCellFlat0_4, DirectionalDiffuseCriticalCase)
{
    auto & aCell{GetCell()};

    // Front side
    Side aSide = Side::Front;
    // CBeamDirection (Theta, Phi)
    CBeamDirection incomingDirection = CBeamDirection({54, 240});   // incoming #30
    CBeamDirection outgoingDirection = CBeamDirection({54, 300});   // outgoing #26

    double Tdir_dif = aCell.T_dir_dif(aSide, incomingDirection, outgoingDirection);
    double Rdir_dif = aCell.R_dir_dif(aSide, incomingDirection, outgoingDirection);

    EXPECT_NEAR(0.114084, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.128390, Rdir_dif, 1e-6);
}

// This case is symmetrical to the DirectionalDiffuseCriticalCase and they should give identical
// results. The only difference is that the incoming and outgoing directions are symmetrical to
// the y-axe and therefore should produce the identical results.
TEST_F(TestVenetianCellFlat0_4, DirectionalDiffuseCriticalCaseSymmetricalResults)
{
    auto & aCell{GetCell()};

    // Front side
    Side aSide = Side::Front;
    // CBeamDirection (Theta, Phi)

    CBeamDirection incomingDirection = CBeamDirection({54, 120});   // incoming #26
    CBeamDirection outgoingDirection = CBeamDirection({54, 60});    // outgoing #30

    double Tdir_dif = aCell.T_dir_dif(aSide, incomingDirection, outgoingDirection);
    double Rdir_dif = aCell.R_dir_dif(aSide, incomingDirection, outgoingDirection);

    EXPECT_NEAR(0.114084, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.128390, Rdir_dif, 1e-6);
}