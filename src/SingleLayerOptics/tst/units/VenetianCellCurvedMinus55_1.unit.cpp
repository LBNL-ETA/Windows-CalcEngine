#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianCellCurvedMinus55_1 : public testing::Test
{
private:
    std::shared_ptr<CVenetianCell> m_Cell;

protected:
    virtual void SetUp()
    {
        // create material
        const auto Tmat = 0.1;
        const auto Rfmat = 0.7;
        const auto Rbmat = 0.7;
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        const auto slatWidth = 0.076200;     // m
        const auto slatSpacing = 0.057150;   // m
        const auto slatTiltAngle = -55.000000;
        const auto curvatureRadius = 0.123967;
        const FenestrationCommon::Venetian::Geometry geometry{
          slatWidth, slatSpacing, slatTiltAngle, curvatureRadius};
        size_t numOfSlatSegments = 2;

        std::shared_ptr<CVenetianCellDescription> aCellDescription =
          std::make_shared<CVenetianCellDescription>(geometry, numOfSlatSegments);

        m_Cell = std::make_shared<CVenetianCell>(aMaterial, aCellDescription);
    }

public:
    std::shared_ptr<CVenetianCell> GetCell()
    {
        return m_Cell;
    };
};

TEST_F(TestVenetianCellCurvedMinus55_1, TestVenetian1)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Curved, -55 degrees slats - diffuse-diffuse).");

    std::shared_ptr<CVenetianCell> aCell = GetCell();

    // Front side
    Side aSide = Side::Front;
    double Tdif_dif = aCell->T_dif_dif(aSide);
    double Rdif_dif = aCell->R_dif_dif(aSide);

    EXPECT_NEAR(0.31432383716259166, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.43717927379126331, Rdif_dif, 1e-6);

    // Back side
    aSide = Side::Back;
    Tdif_dif = aCell->T_dif_dif(aSide);
    Rdif_dif = aCell->R_dif_dif(aSide);

    EXPECT_NEAR(0.31432383716259171, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.45819107731734438, Rdif_dif, 1e-6);
}

TEST_F(TestVenetianCellCurvedMinus55_1, TestVenetian2)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Curved, -55 degrees slats - direct-diffuse).");

    std::shared_ptr<CVenetianCell> aCell = GetCell();

    // Front side
    Side aSide = Side::Front;
    double Theta = 0;
    double Phi = 0;

    CBeamDirection aDirection = CBeamDirection(Theta, Phi);

    double Tdir_dir = aCell->T_dir_dir(aSide, aDirection);
    double Tdir_dif = aCell->T_dir_dif(aSide, aDirection);
    double Rdir_dif = aCell->R_dir_dif(aSide, aDirection);

    EXPECT_NEAR(0.00000000000000000, Tdir_dir, 1e-6);
    EXPECT_NEAR(0.19221286838976023, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.50885857000236578, Rdir_dif, 1e-6);

    // Back side
    aSide = Side::Back;
    Tdir_dir = aCell->T_dir_dir(aSide, aDirection);
    Tdir_dif = aCell->T_dir_dif(aSide, aDirection);
    Rdir_dif = aCell->R_dir_dif(aSide, aDirection);

    EXPECT_NEAR(0.00000000000000000, Tdir_dir, 1e-6);
    EXPECT_NEAR(0.19839281076530671, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.51950846145958018, Rdir_dif, 1e-6);
}
