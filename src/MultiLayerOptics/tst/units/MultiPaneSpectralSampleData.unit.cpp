#include <memory>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>
#include <WCECommon.hpp>

using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

namespace
{
    void expectSeriesNear(const std::vector<double> & expected,
                          const CSeries & actual,
                          double tolerance = 1e-6)
    {
        EXPECT_EQ(actual.size(), expected.size());
        for(size_t i = 0; i < actual.size(); ++i)
        {
            EXPECT_NEAR(expected[i], actual[i].value(), tolerance);
        }
    }
}   // namespace

class TestMultiLayerOpticsMeasuredSampleData : public testing::Test
{
    std::shared_ptr<CMultiPaneSampleData> m_MultiLayerOptics;

protected:
    void SetUp() override
    {
        // This test (example) shows how to get multilayer sample data from two measurements.
        // Results are calculated at each wavelength

        // sampleMeasurements1
        const auto sampleMeasurements1 = CSpectralSampleData::create({
          {0.330, {0.0857, 0.0857, 0.0560, 0.2646}, {0.0, 0.0, 0.0, 0.0}},
          {0.335, {0.1280, 0.1280, 0.0623, 0.2664}, {0.0, 0.0, 0.0, 0.0}},
          {0.340, {0.1707, 0.1707, 0.0719, 0.2668}, {0.0, 0.0, 0.0, 0.0}},
          {0.345, {0.2125, 0.2125, 0.0840, 0.2680}, {0.0, 0.0, 0.0, 0.0}},
          {0.350, {0.2536, 0.2536, 0.0990, 0.2706}, {0.0, 0.0, 0.0, 0.0}},
          {0.355, {0.2953, 0.2953, 0.1165, 0.2735}, {0.0, 0.0, 0.0, 0.0}},
          {0.360, {0.3370, 0.3370, 0.1365, 0.2773}, {0.0, 0.0, 0.0, 0.0}},
          {0.365, {0.3774, 0.3774, 0.1579, 0.2809}, {0.0, 0.0, 0.0, 0.0}},
          {0.370, {0.4125, 0.4125, 0.1773, 0.2829}, {0.0, 0.0, 0.0, 0.0}},
          {0.375, {0.4414, 0.4414, 0.1931, 0.2836}, {0.0, 0.0, 0.0, 0.0}},
          {0.380, {0.4671, 0.4671, 0.2074, 0.2827}, {0.0, 0.0, 0.0, 0.0}},
          {0.385, {0.4953, 0.4953, 0.2244, 0.2814}, {0.0, 0.0, 0.0, 0.0}},
          {0.390, {0.5229, 0.5229, 0.2415, 0.2801}, {0.0, 0.0, 0.0, 0.0}},
          {0.395, {0.5455, 0.5455, 0.2553, 0.2781}, {0.0, 0.0, 0.0, 0.0}},
          {0.400, {0.5630, 0.5630, 0.2651, 0.2757}, {0.0, 0.0, 0.0, 0.0}},
        });

        // sampleMeasurements2
        const auto sampleMeasurements2 = CSpectralSampleData::create({
          {0.330, {0.1600, 0.1600, 0.0450, 0.0470}, {0.0, 0.0, 0.0, 0.0}},
          {0.335, {0.2940, 0.2940, 0.0490, 0.0500}, {0.0, 0.0, 0.0, 0.0}},
          {0.340, {0.4370, 0.4370, 0.0550, 0.0560}, {0.0, 0.0, 0.0, 0.0}},
          {0.345, {0.5660, 0.5660, 0.0620, 0.0620}, {0.0, 0.0, 0.0, 0.0}},
          {0.350, {0.6710, 0.6710, 0.0690, 0.0690}, {0.0, 0.0, 0.0, 0.0}},
          {0.355, {0.7440, 0.7440, 0.0740, 0.0740}, {0.0, 0.0, 0.0, 0.0}},
          {0.360, {0.7930, 0.7930, 0.0780, 0.0780}, {0.0, 0.0, 0.0, 0.0}},
          {0.365, {0.8220, 0.8220, 0.0800, 0.0800}, {0.0, 0.0, 0.0, 0.0}},
          {0.370, {0.8320, 0.8320, 0.0810, 0.0810}, {0.0, 0.0, 0.0, 0.0}},
          {0.375, {0.8190, 0.8190, 0.0800, 0.0800}, {0.0, 0.0, 0.0, 0.0}},
          {0.380, {0.8090, 0.8090, 0.0790, 0.0790}, {0.0, 0.0, 0.0, 0.0}},
          {0.385, {0.8290, 0.8290, 0.0800, 0.0800}, {0.0, 0.0, 0.0, 0.0}},
          {0.390, {0.8530, 0.8530, 0.0820, 0.0820}, {0.0, 0.0, 0.0, 0.0}},
          {0.395, {0.8680, 0.8680, 0.0830, 0.0830}, {0.0, 0.0, 0.0, 0.0}},
          {0.400, {0.8750, 0.8750, 0.0830, 0.0830}, {0.0, 0.0, 0.0, 0.0}},
        });


        m_MultiLayerOptics = std::make_shared<CMultiPaneSampleData>();
        m_MultiLayerOptics->addSample(sampleMeasurements1);
        m_MultiLayerOptics->addSample(sampleMeasurements2);
    }

public:
    std::shared_ptr<CMultiPaneSampleData> getMultiLayerOptics()
    {
        return m_MultiLayerOptics;
    }
};

TEST_F(TestMultiLayerOpticsMeasuredSampleData, TestDoublePaneResults)
{
    SCOPED_TRACE(
      "Begin Test: Test simple double pane calculations (T, Rf, Rb and equivalent absorptances).");

    auto MultiLayerOptics = getMultiLayerOptics();

    const std::vector correctT = {0.013877,
                                  0.038130,
                                  0.075707,
                                  0.122307,
                                  0.173403,
                                  0.224242,
                                  0.273149,
                                  0.317354,
                                  0.351249,
                                  0.369899,
                                  0.386516,
                                  0.420060,
                                  0.456519,
                                  0.484682,
                                  0.504162};

    expectSeriesNear(correctT, MultiLayerOptics->properties(Property ::T, Side ::Front));

    const std::vector correctRf = {0.056334,
                                   0.063113,
                                   0.073526,
                                   0.086847,
                                   0.103522,
                                   0.123086,
                                   0.145554,
                                   0.169556,
                                   0.191406,
                                   0.209049,
                                   0.225030,
                                   0.244478,
                                   0.264448,
                                   0.280582,
                                   0.292025};

    expectSeriesNear(correctRf, MultiLayerOptics->properties(Property::R, Side::Front));

    const std::vector correctRb = {0.053855387,
                                   0.073331105,
                                   0.107709312,
                                   0.149306086,
                                   0.193153331,
                                   0.228519414,
                                   0.256234943,
                                   0.274162863,
                                   0.281422839,
                                   0.274643900,
                                   0.268248318,
                                   0.277843470,
                                   0.290594317,
                                   0.297477857,
                                   0.299026161};

    expectSeriesNear(correctRb, MultiLayerOptics->properties(Property ::R, Side::Back));

    const std::vector correctAbs = {0.929788,
                                    0.898757,
                                    0.850767,
                                    0.790846,
                                    0.723075,
                                    0.652672,
                                    0.581297,
                                    0.513089,
                                    0.457345,
                                    0.421053,
                                    0.388454,
                                    0.335462,
                                    0.279033,
                                    0.234737,
                                    0.203814};

    expectSeriesNear(correctAbs, MultiLayerOptics->properties(Property::Abs, Side::Front));
}


TEST_F(TestMultiLayerOpticsMeasuredSampleData, TestDoublePaneAbsorptances)
{
    SCOPED_TRACE("Begin Test: Test layer absroptances.");

    auto MultiLayerOptics = getMultiLayerOptics();

    const std::vector correctAbsF1 = {0.860835761,
                                      0.813548561,
                                      0.762759679,
                                      0.710460061,
                                      0.655884150,
                                      0.597817313,
                                      0.536862648,
                                      0.475253771,
                                      0.420616132,
                                      0.375436233,
                                      0.334943510,
                                      0.289351814,
                                      0.244245498,
                                      0.207375484,
                                      0.179613906};

    expectSeriesNear(correctAbsF1, MultiLayerOptics->getLayerAbsorptances(1, Side::Front));

    const std::vector correctAbsF2 = {0.068952518,
                                      0.085208275,
                                      0.088007015,
                                      0.080385689,
                                      0.067190541,
                                      0.054854806,
                                      0.044434083,
                                      0.037835438,
                                      0.036729145,
                                      0.045616344,
                                      0.053510261,
                                      0.046110336,
                                      0.034787506,
                                      0.027361056,
                                      0.024199766};

    expectSeriesNear(correctAbsF2, MultiLayerOptics->getLayerAbsorptances(2, Side::Front));
}
