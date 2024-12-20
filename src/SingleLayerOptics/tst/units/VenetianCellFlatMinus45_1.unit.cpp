#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianCellFlatMinus45_1 : public testing::Test
{
private:
    // std::shared_ptr< CVenetianCell > m_Cell;
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
        const auto slatWidth = 0.010;     // m
        const auto slatSpacing = 0.010;   // m
        const auto slatTiltAngle = -45;
        const auto curvatureRadius = 0;
        const FenestrationCommon::Venetian::Geometry geometry{
          slatWidth, slatSpacing, slatTiltAngle, curvatureRadius};
        const size_t numOfSlatSegments = 2;

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

TEST_F(TestVenetianCellFlatMinus45_1, TestVenetian1)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Flat, -45 degrees slats) - diffuse-diffuse.");

    std::shared_ptr<CVenetianCell> aCell = GetCell();

    // Front side
    Side aSide = Side::Front;
    double Tdif_dif = aCell->T_dif_dif(aSide);
    double Rdif_dif = aCell->R_dif_dif(aSide);

    EXPECT_NEAR(0.47122586752693946, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.34565694288233745, Rdif_dif, 1e-6);

    // Back side
    aSide = Side::Back;
    Tdif_dif = aCell->T_dif_dif(aSide);
    Rdif_dif = aCell->R_dif_dif(aSide);

    EXPECT_NEAR(0.47122586752693946, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.34565694288233745, Rdif_dif, 1e-6);
}

TEST_F(TestVenetianCellFlatMinus45_1, TestVenetian2)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Flat, -45 degrees slats) - direct-diffuse.");

    std::shared_ptr<CVenetianCell> aCell = GetCell();

    // Front side
    Side aSide = Side::Front;
    double Theta = 0;
    double Phi = 0;
    CBeamDirection aDirection = CBeamDirection(Theta, Phi);

    double Tdir_dir = aCell->T_dir_dir(aSide, aDirection);
    double Tdir_dif = aCell->T_dir_dif(aSide, aDirection);
    double Rdir_dif = aCell->R_dir_dif(aSide, aDirection);

    EXPECT_NEAR(0.29289321881345237, Tdir_dir, 1e-6);
    EXPECT_NEAR(0.15853813605369516, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.35939548999199655, Rdir_dif, 1e-6);

    // Back side
    aSide = Side::Back;
    Tdir_dir = aCell->T_dir_dir(aSide, aDirection);
    Tdir_dif = aCell->T_dir_dif(aSide, aDirection);
    Rdir_dif = aCell->R_dir_dif(aSide, aDirection);

    EXPECT_NEAR(0.29289321881345237, Tdir_dir, 1e-6);
    EXPECT_NEAR(0.15853813605369510, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.35939548999199644, Rdir_dif, 1e-6);
}
