#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"

#include "csvHandlers.hpp"
#include "matrixTesting.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat0_1 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Shade;

protected:
    void SetUp() override
    {
        // create material
        const auto Tmat = 0.1;
        const auto Rfmat = 0.7;
        const auto Rbmat = 0.7;
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        // make cell geometry
        const auto slatWidth = 0.010;     // m
        const auto slatSpacing = 0.010;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0;
        const size_t numOfSlatSegments = 1;

        // Method
        DistributionMethod aDistribution = DistributionMethod::DirectionalDiffuse;

        // create BSDF
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        // make layer
        m_Shade = CBSDFLayerMaker::getVenetianLayer(aMaterial,
                                                    aBSDF,
                                                    slatWidth,
                                                    slatSpacing,
                                                    slatTiltAngle,
                                                    curvatureRadius,
                                                    numOfSlatSegments,
                                                    aDistribution,
                                                    true);
    }

public:
    [[nodiscard]] std::shared_ptr<CBSDFLayer> GetShade() const
    {
        return m_Shade;
    };
};

TEST_F(TestVenetianDirectionalShadeFlat0_1, TestVenetian1)
{
    SCOPED_TRACE("Begin Test: Venetian cell (Flat, 45 degrees slats) - solar properties.");

    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    BSDFIntegrator aResults = aShade->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.484803, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.340929, RfDiff, 1e-6);

    const double theta = 23;
    const double phi = 198;

    const double tauHem = aResults.DirHem(Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.464709, tauHem, 1e-6);

    const auto aT = aResults.getMatrix(Side::Front, PropertySimple::T);

    const auto correctT{
      Helper::readMatrixFromCSV(TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestVenetianDirectionalShadeFlat0_1_aT.csv")};

    Helper::compareMatrices(correctT, aT.getMatrix(), 1e-6);

    // Front reflectance
    const auto aRf = aResults.getMatrix(Side::Front, PropertySimple::R);

    const auto correctR{
      Helper::readMatrixFromCSV(TEST_DATA_DIR_SINGLE_LAYER_OPTICS "/data/TestVenetianDirectionalShadeFlat0_1_aRf.csv")};

    Helper::compareMatrices(correctR, aRf.getMatrix(), 1e-6);
}

TEST_F(TestVenetianDirectionalShadeFlat0_1, AtWavelength)
{
    std::shared_ptr<CBSDFLayer> aShade = GetShade();

    constexpr size_t wavelengthIndex{0u};
    auto aResults{aShade->getResultsAtWavelength(wavelengthIndex)};
    const auto correct{0.484803};
    const auto result{aResults.DiffDiff(Side::Front, PropertySimple::T)};

    EXPECT_NEAR(correct, result, 1e-6);
}