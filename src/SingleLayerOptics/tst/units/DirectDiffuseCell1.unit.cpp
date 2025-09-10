#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestDirectDiffuseCell1 : public testing::Test
{
    std::shared_ptr<CDirectionalDiffuseCell> m_Cell;

protected:
    void SetUp() override
    {
        // This is only direct part of the single band material
        constexpr auto Tfmat = 0.08;
        constexpr auto Tbmat = 0.08;
        constexpr auto Rfmat = 0.9;
        constexpr auto Rbmat = 0.9;
        const auto aMaterial = Material::singleBandMaterial(Tfmat, Tbmat, Rfmat, Rbmat);

        std::shared_ptr<ICellDescription> aCell = std::make_shared<CSpecularCellDescription>();

        m_Cell = std::make_shared<CDirectionalDiffuseCell>(aMaterial, aCell);
    }

public:
    std::shared_ptr<CDirectionalDiffuseCell> GetCell()
    {
        return m_Cell;
    }
};

namespace
{
    CBeamDirection direction(const double theta, const double phi)
    {
        return {theta, phi};
    }
}   // namespace

TEST_F(TestDirectDiffuseCell1, TestNormalIncidence)
{
    auto aCell = GetCell();
    const CBeamDirection Incoming{0, 0};
    const CBeamDirection Outgoing{0, 0};

    constexpr Side aSide{Side::Front};

    const double Tfdir_dir = aCell->T_dir_dir(aSide, Incoming);
    EXPECT_NEAR(0.0, Tfdir_dir, 1e-6);

    const double Tfdir_dif = aCell->T_dir_dif(aSide, Incoming, Outgoing);
    EXPECT_NEAR(0.08, Tfdir_dif, 1e-6);

    const double Rfdir_dif = aCell->R_dir_dif(aSide, Incoming, Outgoing);
    EXPECT_NEAR(0.9, Rfdir_dif, 1e-6);
}

TEST_F(TestDirectDiffuseCell1, TestNonNormalIncidence)
{
    auto aCell = GetCell();
    const CBeamDirection Incoming{10.0, 0};
    const CBeamDirection Outgoing{20.0, 0};

    constexpr Side aSide{Side::Front};

    const double Tfdir_dir = aCell->T_dir_dir(aSide, Incoming);
    EXPECT_NEAR(0.0, Tfdir_dir, 1e-6);

    const double Tfdir_dif = aCell->T_dir_dif(aSide, Incoming, Outgoing);
    EXPECT_NEAR(0.0, Tfdir_dif, 1e-6);

    const double Rfdir_dif = aCell->R_dir_dif(aSide, Incoming, Outgoing);
    EXPECT_NEAR(0.0, Rfdir_dif, 1e-6);
}
