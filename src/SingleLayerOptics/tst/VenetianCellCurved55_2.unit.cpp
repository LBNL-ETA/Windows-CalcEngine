#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianCellCurved55_2 : public testing::Test
{
private:
    std::shared_ptr<CVenetianCell> m_Cell;

protected:
    virtual void SetUp()
    {
        // create material
		const auto Tmat = 0.1;
		const auto Rfmat = 0.3;
		const auto Rbmat = 0.7;
		const auto minLambda = 0.3;
		const auto maxLambda = 2.5;
        const auto aMaterial =
          Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda);

        // make cell geometry
		const auto slatWidth = 0.076200;     // m
		const auto slatSpacing = 0.057150;   // m
		const auto slatTiltAngle = 55.000000;
		const auto curvatureRadius = 0.123967;
        size_t numOfSlatSegments = 2;

        std::shared_ptr<CVenetianCellDescription> aCellDescription =
          std::make_shared<CVenetianCellDescription>(
            slatWidth, slatSpacing, slatTiltAngle, curvatureRadius, numOfSlatSegments);

        m_Cell = std::make_shared<CVenetianCell>(aMaterial, aCellDescription);
    }

public:
    std::shared_ptr<CVenetianCell> GetCell()
    {
        return m_Cell;
    };
};

TEST_F(TestVenetianCellCurved55_2, TestVenetian1)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Curved, -55 degrees slats - diffuse-diffuse).");

    std::shared_ptr<CVenetianCell> aCell = GetCell();

    // Front side
    Side aSide = Side::Front;
    double Tdif_dif = aCell->T_dif_dif(aSide);
    double Rdif_dif = aCell->R_dif_dif(aSide);

    EXPECT_NEAR(0.260287, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.196089, Rdif_dif, 1e-6);

    // Back side
    aSide = Side::Back;
    Tdif_dif = aCell->T_dif_dif(aSide);
    Rdif_dif = aCell->R_dif_dif(aSide);

    EXPECT_NEAR(0.260287, Tdif_dif, 1e-6);
    EXPECT_NEAR(0.399713, Rdif_dif, 1e-6);
}

TEST_F(TestVenetianCellCurved55_2, TestVenetian2)
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

    EXPECT_NEAR(0.000000, Tdir_dir, 1e-6);
    EXPECT_NEAR(0.113713, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.217891, Rdir_dif, 1e-6);

    // Back side
    aSide = Side::Back;
    Tdir_dir = aCell->T_dir_dir(aSide, aDirection);
    Tdir_dif = aCell->T_dir_dif(aSide, aDirection);
    Rdir_dif = aCell->R_dir_dif(aSide, aDirection);

    EXPECT_NEAR(0.000000, Tdir_dir, 1e-6);
    EXPECT_NEAR(0.131946, Tdir_dif, 1e-6);
    EXPECT_NEAR(0.469503, Rdir_dif, 1e-6);
}
