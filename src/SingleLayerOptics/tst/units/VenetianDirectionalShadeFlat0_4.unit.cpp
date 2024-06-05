#include <memory>
#include <gtest/gtest.h>
#include <fstream>

#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

void printMatrixToCSV(int n,
                          const std::function<double(int, int)> & mat,
                          const std::string & filename,
                          int precision = 6)
    {
        std::ofstream file(filename);
        if(!file.is_open())
        {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return;
        }

        file << std::fixed << std::setprecision(precision);

        for(int i = 0; i < n; ++i)
        {
            for(int j = 0; j < n; ++j)
            {
                file << mat(i, j);
                if(j < n - 1)
                {
                    file << ",";
                }
            }
            file << "\n";
        }

        file.close();
    }

class TestVenetianDirectionalShadeFlat0_4 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    virtual void SetUp()
    {
        // create material
        const auto Tmat = 0.0;
        const auto Rfmat = 0.9;
        const auto Rbmat = 0.9;

        // make cell geometry
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 0;
        const auto curvatureRadius = 0;
        const size_t numOfSlatSegments = 1;

        // make layer
        m_Shade = CBSDFLayerMaker::getVenetianLayer(Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat),
                                                    BSDFHemisphere::create(BSDFBasis::Quarter),
                                                    slatWidth,
                                                    slatSpacing,
                                                    slatTiltAngle,
                                                    curvatureRadius,
                                                    numOfSlatSegments,
                                                    DistributionMethod::DirectionalDiffuse,
                                                    true);
    }

public:
    std::shared_ptr<CBSDFLayer> GetShade()
    {
        return m_Shade;
    };
};

TEST_F(TestVenetianDirectionalShadeFlat0_4, TestVenetian1)
{
    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    auto aResults = aShade->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.549687, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.165684, RfDiff, 1e-6);

    auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);
    printMatrixToCSV(aT.size(), aT, "Tf.csv");
}
