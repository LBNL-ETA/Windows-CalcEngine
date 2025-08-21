#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestSpecularLayer_1042 : public testing::Test
{
    std::shared_ptr<CBSDFLayer> m_Layer;

protected:
    void SetUp() override
    {
        double thickness = 3.18e-3;   // [m]
        auto aMaterial =
          Material::nBandMaterial(SpectralSample::NFRC_1042(), thickness, MaterialType::Coated);
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Full);

        aMaterial->setBandWavelengths(StandardData::solarRadiationASTM_E891_87_Table1().getXArray());

        // make layer
        m_Layer = CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);
        m_Layer->setSourceData(StandardData::solarRadiationASTM_E891_87_Table1());
    }

public:
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    }
};

// C++
TEST_F(TestSpecularLayer_1042, TestSpecular1)
{
    SCOPED_TRACE("Begin Test: Specular layer - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    auto aResults = aLayer->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.39638769241019578, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.4012138036375823, RfDiff, 1e-6);

    constexpr double theta = 37;
    constexpr double phi = 59;

    const double tauHem = aResults.DirHem(Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.43259306056506958, tauHem, 1e-6);

    const double tauDir = aResults.DirDir(Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.43259306056506958, tauDir, 1e-6);
}
