#include <memory>
#include <gtest/gtest.h>

#include "WCESpectralAveraging.hpp"
#include "WCEMultiLayerOptics.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

class MultiPaneBSDF_SingleBandMaterial_Woven_IR : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    virtual void SetUp()
    {
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        const auto TIR = 0.0;
        const auto RfIR = 0.1;
        const auto RbIR = 0.1;

        auto aMaterial = Material::singleBandMaterial(TIR, TIR, RfIR, RbIR);

        // make cell geometry
        const auto diameter = 0.002;   // m
        const auto spacing = 0.003;    // m

        // Perforated layer is created here
        const auto LayerWoven{CBSDFLayerMaker::getWovenLayer(aMaterial, aBSDF, diameter, spacing)};

        m_Layer = CMultiPaneBSDF::create({LayerWoven});

        std::vector<double> wl{5,  10, 15, 20, 25, 30, 35, 40, 45, 50,
                               55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
        const auto blackBodyTemperature = 300;
        const auto sourceData = SpectralAveraging::BlackBodySpectrum(wl, blackBodyTemperature);

        // Need to update correct solar radiation table. This one will not produce results since it
        // is out of range
        const CalculationProperties input{sourceData, wl};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_SingleBandMaterial_Woven_IR, TestWovenShade)
{
    const double minLambda = 5.0;
    const double maxLambda = 100.0;

    CMultiPaneBSDF & aLayer = getLayer();

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.037034, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.096297, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.866670, absDiff1, 1e-6);
}
