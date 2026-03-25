#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

class MultiPaneBSDF_102_102_Condensed_LayerMatrices : public testing::Test
{
public:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        constexpr auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102a = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        auto aMaterial_102b = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto layer1 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102a, aBSDF);
        auto layer2 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102b, aBSDF);

        m_Layer = CMultiPaneBSDF::create({layer1, layer2});
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_102_102_Condensed_LayerMatrices, PerLayerTransmittanceMatrices)
{
    CMultiPaneBSDF & aLayer = getLayer();

    const CalculationProperties input{StandardData::solarRadiationASTM_E891_87_Table1(),
                                      StandardData::condensedSpectrumDefault()};
    aLayer.setCalculationProperties(input);

    // Get per-layer wavelength matrices for both layers (front transmittance)
    const auto layer1FrontT =
      aLayer.getLayerWavelengthMatrices(1, Side::Front, PropertySurface::T);
    const auto layer2FrontT =
      aLayer.getLayerWavelengthMatrices(2, Side::Front, PropertySurface::T);

    // Both layers are identical NFRC_102, so their raw single-layer matrices must match
    ASSERT_EQ(layer1FrontT.size(), 21u);
    ASSERT_EQ(layer2FrontT.size(), 21u);

    for(size_t idx = 0; idx < layer1FrontT.size(); ++idx)
    {
        EXPECT_NEAR(layer1FrontT[idx].x, layer2FrontT[idx].x, 1e-10);

        const auto & mat1 = layer1FrontT[idx].matrix;
        const auto & mat2 = layer2FrontT[idx].matrix;
        ASSERT_EQ(mat1.size(), mat2.size());

        // All elements should be identical for identical layers
        for(size_t row = 0; row < mat1.size(); ++row)
        {
            for(size_t col = 0; col < mat1.size(); ++col)
            {
                EXPECT_NEAR(mat1(row, col), mat2(row, col), 1e-10);
            }
        }
    }

    // For specular clear glass, direct-direct transmittance (0,0) should be positive
    for(const auto & [wl, mat] : layer1FrontT)
    {
        EXPECT_GT(mat(0, 0), 0.0);
    }
}

TEST_F(MultiPaneBSDF_102_102_Condensed_LayerMatrices, PerLayerReflectanceMatrices)
{
    CMultiPaneBSDF & aLayer = getLayer();

    const CalculationProperties input{StandardData::solarRadiationASTM_E891_87_Table1(),
                                      StandardData::condensedSpectrumDefault()};
    aLayer.setCalculationProperties(input);

    const auto layer1FrontR =
      aLayer.getLayerWavelengthMatrices(1, Side::Front, PropertySurface::R);
    const auto layer2FrontR =
      aLayer.getLayerWavelengthMatrices(2, Side::Front, PropertySurface::R);

    ASSERT_EQ(layer1FrontR.size(), 21u);
    ASSERT_EQ(layer2FrontR.size(), 21u);

    // Identical layers must produce identical reflectance matrices
    for(size_t idx = 0; idx < layer1FrontR.size(); ++idx)
    {
        EXPECT_NEAR(layer1FrontR[idx].x, layer2FrontR[idx].x, 1e-10);
        EXPECT_NEAR(
          layer1FrontR[idx].matrix(0, 0), layer2FrontR[idx].matrix(0, 0), 1e-10);
    }

    // Reflectance (0,0) should be positive for glass
    for(const auto & [wl, mat] : layer1FrontR)
    {
        EXPECT_GT(mat(0, 0), 0.0);
    }
}

TEST_F(MultiPaneBSDF_102_102_Condensed_LayerMatrices, SystemVsLayerTransmittance)
{
    CMultiPaneBSDF & aLayer = getLayer();

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    const CalculationProperties input{StandardData::solarRadiationASTM_E891_87_Table1(),
                                      StandardData::condensedSpectrumDefault()};
    aLayer.setCalculationProperties(input);

    // System wavelength matrices (regression check)
    const auto systemMatrices =
      aLayer.getWavelengthMatrices(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_EQ(systemMatrices.size(), 21u);

    // Per-layer matrices for both layers
    const auto layer1Matrices =
      aLayer.getLayerWavelengthMatrices(1, Side::Front, PropertySurface::T);
    const auto layer2Matrices =
      aLayer.getLayerWavelengthMatrices(2, Side::Front, PropertySurface::T);
    EXPECT_EQ(layer1Matrices.size(), 21u);
    EXPECT_EQ(layer2Matrices.size(), 21u);
}
