#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular layers only

class MultiPaneBSDF_102_103_callback : public testing::Test
{
public:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;
    std::atomic<size_t> callbackCalls{0};
    size_t lastCurrent{0};
    size_t totalWavelengths{0};

protected:
    void SetUp() override
    {
        auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        thickness = 5.715e-3;   // [m]
        auto aMaterial_103 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_103(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);
        auto Layer_103 = CBSDFLayerMaker::getSpecularLayer(aMaterial_103, aBSDF);

        // Define progress callback
        FenestrationCommon::ProgressCallback callback = [this](size_t current, size_t total) {
            ++callbackCalls;
            lastCurrent = current;
            totalWavelengths = total;
        };

        m_Layer = CMultiPaneBSDF::create({Layer_102, Layer_103}, std::nullopt, callback);
    }
};

TEST_F(MultiPaneBSDF_102_103_callback, SolarAndVisibleRange)
{
    EXPECT_EQ(callbackCalls.load(), 111u);
    EXPECT_EQ(totalWavelengths, 111u);
    EXPECT_LE(lastCurrent, totalWavelengths);
}