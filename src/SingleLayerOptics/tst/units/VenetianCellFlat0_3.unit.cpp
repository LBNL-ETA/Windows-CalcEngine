#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>

// This example corresponds to example 1 in the venetian blind validation document.

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianCellFlat0_3 : public testing::Test
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
        const size_t numOfSlatSegments = 1;

        return std::make_shared<CVenetianCellDescription>(geometry, numOfSlatSegments);
    }

public:
    CVenetianCell& GetCell()
    {
        return m_Cell;
    };
};

TEST_F(TestVenetianCellFlat0_3, DiffiseDiffuse)
{
    auto & aCell{GetCell()};

    double Tdif_dif{aCell.T_dif_dif(FenestrationCommon::Side::Front)};
    double Rdif_dif{aCell.R_dif_dif(FenestrationCommon::Side::Front)};

    EXPECT_NEAR(0.606061, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.272727, Rdif_dif, 1e-6);
}

TEST_F(TestVenetianCellFlat0_3, DirectDirect)
{
    auto & aCell{GetCell()};

    constexpr auto Theta{18.0};
    constexpr auto Phi{270.0};

    const auto Tdir_dir{aCell.T_dir_dir(FenestrationCommon::Side::Front, {Theta, Phi})};

    EXPECT_NEAR(0.566774, Tdir_dir, 1e-6);
}

TEST_F(TestVenetianCellFlat0_3, DirectionalViewFactors)
{
    auto aCell{createCellDescription()};

    auto Theta{18.0};
    auto Phi{270.0};
    CBeamDirection incomingDirection = CBeamDirection(Theta, Phi);

    const auto Tdir_hem{aCell->viewFactors(FenestrationCommon::Side::Front, incomingDirection)};

    //EXPECT_NEAR(0.566774, Tdir_hem, 1e-6);
}

TEST_F(TestVenetianCellFlat0_3, DirectionalDiffuse)
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

    EXPECT_NEAR(0.071485, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.071485, Rdir_dif, 1e-6);
}
