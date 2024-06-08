#include <memory>
#include <gtest/gtest.h>

#include "WCESpectralAveraging.hpp"
#include "WCEMultiLayerOptics.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular and venetian layers

class MultiPaneBSDF_102_VenetianDirectional_n_Band_Material : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

    CSeries loadSolarRadiationFile()
    {
        // Full ASTM E891-87 Table 1 (Solar radiation)
        CSeries aSolarRadiation(
          {{0.3000, 0.0},    {0.3050, 3.4},    {0.3100, 15.6},   {0.3150, 41.1},   {0.3200, 71.2},
           {0.3250, 100.2},  {0.3300, 152.4},  {0.3350, 155.6},  {0.3400, 179.4},  {0.3450, 186.7},
           {0.3500, 212.0},  {0.3600, 240.5},  {0.3700, 324.0},  {0.3800, 362.4},  {0.3900, 381.7},
           {0.4000, 556.0},  {0.4100, 656.3},  {0.4200, 690.8},  {0.4300, 641.9},  {0.4400, 798.5},
           {0.4500, 956.6},  {0.4600, 990.0},  {0.4700, 998.0},  {0.4800, 1046.1}, {0.4900, 1005.1},
           {0.5000, 1026.7}, {0.5100, 1066.7}, {0.5200, 1011.5}, {0.5300, 1084.9}, {0.5400, 1082.4},
           {0.5500, 1102.2}, {0.5700, 1087.4}, {0.5900, 1024.3}, {0.6100, 1088.8}, {0.6300, 1062.1},
           {0.6500, 1061.7}, {0.6700, 1046.2}, {0.6900, 859.2},  {0.7100, 1002.4}, {0.7180, 816.9},
           {0.7244, 842.8},  {0.7400, 971.0},  {0.7525, 956.3},  {0.7575, 942.2},  {0.7625, 524.8},
           {0.7675, 830.7},  {0.7800, 908.9},  {0.8000, 873.4},  {0.8160, 712.0},  {0.8237, 660.2},
           {0.8315, 765.5},  {0.8400, 799.8},  {0.8600, 815.2},  {0.8800, 778.3},  {0.9050, 630.4},
           {0.9150, 565.2},  {0.9250, 586.4},  {0.9300, 348.1},  {0.9370, 224.2},  {0.9480, 271.4},
           {0.9650, 451.2},  {0.9800, 549.7},  {0.9935, 630.1},  {1.0400, 582.9},  {1.0700, 539.7},
           {1.1000, 366.2},  {1.1200, 98.1},   {1.1300, 169.5},  {1.1370, 118.7},  {1.1610, 301.9},
           {1.1800, 406.8},  {1.2000, 375.2},  {1.2350, 423.6},  {1.2900, 365.7},  {1.3200, 223.4},
           {1.3500, 30.1},   {1.3950, 1.4},    {1.4425, 51.6},   {1.4625, 97.0},   {1.4770, 97.3},
           {1.4970, 167.1},  {1.5200, 239.3},  {1.5390, 248.8},  {1.5580, 249.3},  {1.5780, 222.3},
           {1.5920, 227.3},  {1.6100, 210.5},  {1.6300, 224.7},  {1.6460, 215.9},  {1.6780, 202.8},
           {1.7400, 158.2},  {1.8000, 28.6},   {1.8600, 1.8},    {1.9200, 1.1},    {1.9600, 19.7},
           {1.9850, 84.9},   {2.0050, 25.0},   {2.0350, 92.5},   {2.0650, 56.3},   {2.1000, 82.7},
           {2.1480, 76.2},   {2.1980, 66.4},   {2.2700, 65.0},   {2.3600, 57.6},   {2.4500, 19.8},
           {2.4940, 17.0},   {2.5370, 3.0},    {2.9410, 4.0},    {2.9730, 7.0},    {3.0050, 6.0},
           {3.0560, 3.0},    {3.1320, 5.0},    {3.1560, 18.0},   {3.2040, 1.2},    {3.2450, 3.0},
           {3.3170, 12.0},   {3.3440, 3.0},    {3.4500, 12.2},   {3.5730, 11.0},   {3.7650, 9.0},
           {4.0450, 6.9}

          });

        return aSolarRadiation;
    }

    std::shared_ptr<CSpectralSampleData> loadSampleData_NFRC_102()
    {
        auto aMeasurements_102 = CSpectralSampleData::create(
          {{0.300, 0.0020, 0.0470, 0.0480}, {0.305, 0.0030, 0.0470, 0.0480},
           {0.310, 0.0090, 0.0470, 0.0480}, {0.315, 0.0350, 0.0470, 0.0480},
           {0.320, 0.1000, 0.0470, 0.0480}, {0.325, 0.2180, 0.0490, 0.0500},
           {0.330, 0.3560, 0.0530, 0.0540}, {0.335, 0.4980, 0.0600, 0.0610},
           {0.340, 0.6160, 0.0670, 0.0670}, {0.345, 0.7090, 0.0730, 0.0740},
           {0.350, 0.7740, 0.0780, 0.0790}, {0.355, 0.8180, 0.0820, 0.0820},
           {0.360, 0.8470, 0.0840, 0.0840}, {0.365, 0.8630, 0.0850, 0.0850},
           {0.370, 0.8690, 0.0850, 0.0860}, {0.375, 0.8610, 0.0850, 0.0850},
           {0.380, 0.8560, 0.0840, 0.0840}, {0.385, 0.8660, 0.0850, 0.0850},
           {0.390, 0.8810, 0.0860, 0.0860}, {0.395, 0.8890, 0.0860, 0.0860},
           {0.400, 0.8930, 0.0860, 0.0860}, {0.410, 0.8930, 0.0860, 0.0860},
           {0.420, 0.8920, 0.0860, 0.0860}, {0.430, 0.8920, 0.0850, 0.0850},
           {0.440, 0.8920, 0.0850, 0.0850}, {0.450, 0.8960, 0.0850, 0.0850},
           {0.460, 0.9000, 0.0850, 0.0850}, {0.470, 0.9020, 0.0840, 0.0840},
           {0.480, 0.9030, 0.0840, 0.0840}, {0.490, 0.9040, 0.0850, 0.0850},
           {0.500, 0.9050, 0.0840, 0.0840}, {0.510, 0.9050, 0.0840, 0.0840},
           {0.520, 0.9050, 0.0840, 0.0840}, {0.530, 0.9040, 0.0840, 0.0840},
           {0.540, 0.9040, 0.0830, 0.0830}, {0.550, 0.9030, 0.0830, 0.0830},
           {0.560, 0.9020, 0.0830, 0.0830}, {0.570, 0.9000, 0.0820, 0.0820},
           {0.580, 0.8980, 0.0820, 0.0820}, {0.590, 0.8960, 0.0810, 0.0810},
           {0.600, 0.8930, 0.0810, 0.0810}, {0.610, 0.8900, 0.0810, 0.0810},
           {0.620, 0.8860, 0.0800, 0.0800}, {0.630, 0.8830, 0.0800, 0.0800},
           {0.640, 0.8790, 0.0790, 0.0790}, {0.650, 0.8750, 0.0790, 0.0790},
           {0.660, 0.8720, 0.0790, 0.0790}, {0.670, 0.8680, 0.0780, 0.0780},
           {0.680, 0.8630, 0.0780, 0.0780}, {0.690, 0.8590, 0.0770, 0.0770},
           {0.700, 0.8540, 0.0760, 0.0770}, {0.710, 0.8500, 0.0760, 0.0760},
           {0.720, 0.8450, 0.0750, 0.0760}, {0.730, 0.8400, 0.0750, 0.0750},
           {0.740, 0.8350, 0.0750, 0.0750}, {0.750, 0.8310, 0.0740, 0.0740},
           {0.760, 0.8260, 0.0740, 0.0740}, {0.770, 0.8210, 0.0740, 0.0740},
           {0.780, 0.8160, 0.0730, 0.0730}, {0.790, 0.8120, 0.0730, 0.0730},
           {0.800, 0.8080, 0.0720, 0.0720}, {0.810, 0.8030, 0.0720, 0.0720},
           {0.820, 0.8000, 0.0720, 0.0720}, {0.830, 0.7960, 0.0710, 0.0710},
           {0.840, 0.7930, 0.0700, 0.0710}, {0.850, 0.7880, 0.0700, 0.0710},
           {0.860, 0.7860, 0.0700, 0.0700}, {0.870, 0.7820, 0.0740, 0.0740},
           {0.880, 0.7800, 0.0720, 0.0720}, {0.890, 0.7770, 0.0730, 0.0740},
           {0.900, 0.7760, 0.0720, 0.0720}, {0.910, 0.7730, 0.0720, 0.0720},
           {0.920, 0.7710, 0.0710, 0.0710}, {0.930, 0.7700, 0.0700, 0.0700},
           {0.940, 0.7680, 0.0690, 0.0690}, {0.950, 0.7660, 0.0680, 0.0680},
           {0.960, 0.7660, 0.0670, 0.0680}, {0.970, 0.7640, 0.0680, 0.0680},
           {0.980, 0.7630, 0.0680, 0.0680}, {0.990, 0.7620, 0.0670, 0.0670},
           {1.000, 0.7620, 0.0660, 0.0670}, {1.050, 0.7600, 0.0660, 0.0660},
           {1.100, 0.7590, 0.0660, 0.0660}, {1.150, 0.7610, 0.0660, 0.0660},
           {1.200, 0.7650, 0.0660, 0.0660}, {1.250, 0.7700, 0.0650, 0.0650},
           {1.300, 0.7770, 0.0670, 0.0670}, {1.350, 0.7860, 0.0660, 0.0670},
           {1.400, 0.7950, 0.0670, 0.0680}, {1.450, 0.8080, 0.0670, 0.0670},
           {1.500, 0.8190, 0.0690, 0.0690}, {1.550, 0.8290, 0.0690, 0.0690},
           {1.600, 0.8360, 0.0700, 0.0700}, {1.650, 0.8400, 0.0700, 0.0700},
           {1.700, 0.8420, 0.0690, 0.0700}, {1.750, 0.8420, 0.0690, 0.0700},
           {1.800, 0.8410, 0.0700, 0.0700}, {1.850, 0.8400, 0.0690, 0.0690},
           {1.900, 0.8390, 0.0680, 0.0680}, {1.950, 0.8390, 0.0710, 0.0710},
           {2.000, 0.8390, 0.0690, 0.0690}, {2.050, 0.8400, 0.0680, 0.0680},
           {2.100, 0.8410, 0.0680, 0.0680}, {2.150, 0.8390, 0.0690, 0.0690},
           {2.200, 0.8300, 0.0700, 0.0700}, {2.250, 0.8300, 0.0700, 0.0700},
           {2.300, 0.8320, 0.0690, 0.0690}, {2.350, 0.8320, 0.0690, 0.0700},
           {2.400, 0.8320, 0.0700, 0.0700}, {2.450, 0.8260, 0.0690, 0.0690},
           {2.500, 0.8220, 0.0680, 0.0680}});

        return aMeasurements_102;
    }

    std::shared_ptr<CSpectralSampleData> loadSampleData_NFRC_31100()
    {
        auto aMeasurements_31100 = CSpectralSampleData::create(
          {{0.3, 0.0, 0.0703, 0.0703},   {0.305, 0.0, 0.07, 0.07},     {0.31, 0.0, 0.0692, 0.0692},
           {0.315, 0.0, 0.0684, 0.0684}, {0.32, 0.0, 0.0674, 0.0674},  {0.325, 0.0, 0.0663, 0.0663},
           {0.33, 0.0, 0.0653, 0.0653},  {0.335, 0.0, 0.0647, 0.0647}, {0.34, 0.0, 0.0642, 0.0642},
           {0.345, 0.0, 0.0651, 0.0651}, {0.35, 0.0, 0.067, 0.067},    {0.355, 0.0, 0.0704, 0.0704},
           {0.36, 0.0, 0.0751, 0.0751},  {0.365, 0.0, 0.0816, 0.0816}, {0.37, 0.0, 0.09, 0.09},
           {0.375, 0.0, 0.1022, 0.1022}, {0.38, 0.0, 0.1191, 0.1191},  {0.385, 0.0, 0.1455, 0.1455},
           {0.39, 0.0, 0.1897, 0.1897},  {0.395, 0.0, 0.2618, 0.2618}, {0.4, 0.0, 0.3615, 0.3615},
           {0.405, 0.0, 0.4777, 0.4777}, {0.41, 0.0, 0.5803, 0.5803},  {0.415, 0.0, 0.6437, 0.6437},
           {0.42, 0.0, 0.6726, 0.6726},  {0.425, 0.0, 0.684, 0.684},   {0.43, 0.0, 0.69, 0.69},
           {0.435, 0.0, 0.697, 0.697},   {0.44, 0.0, 0.7038, 0.7038},  {0.445, 0.0, 0.7093, 0.7093},
           {0.45, 0.0, 0.7133, 0.7133},  {0.455, 0.0, 0.7154, 0.7154}, {0.46, 0.0, 0.7152, 0.7152},
           {0.465, 0.0, 0.716, 0.716},   {0.47, 0.0, 0.716, 0.716},    {0.475, 0.0, 0.7166, 0.7166},
           {0.48, 0.0, 0.7166, 0.7166},  {0.485, 0.0, 0.7181, 0.7181}, {0.49, 0.0, 0.7199, 0.7199},
           {0.495, 0.0, 0.7225, 0.7225}, {0.5, 0.0, 0.7249, 0.7249},   {0.505, 0.0, 0.7291, 0.7291},
           {0.51, 0.0, 0.7327, 0.7327},  {0.515, 0.0, 0.7374, 0.7374}, {0.52, 0.0, 0.7408, 0.7408},
           {0.525, 0.0, 0.7457, 0.7457}, {0.53, 0.0, 0.7491, 0.7491},  {0.535, 0.0, 0.7524, 0.7524},
           {0.54, 0.0, 0.7548, 0.7548},  {0.545, 0.0, 0.7562, 0.7562}, {0.55, 0.0, 0.7577, 0.7577},
           {0.555, 0.0, 0.7584, 0.7584}, {0.56, 0.0, 0.7574, 0.7574},  {0.565, 0.0, 0.7568, 0.7568},
           {0.57, 0.0, 0.7553, 0.7553},  {0.575, 0.0, 0.7543, 0.7543}, {0.58, 0.0, 0.7529, 0.7529},
           {0.585, 0.0, 0.7511, 0.7511}, {0.59, 0.0, 0.7489, 0.7489},  {0.595, 0.0, 0.7474, 0.7474},
           {0.6, 0.0, 0.7458, 0.7458},   {0.605, 0.0, 0.7434, 0.7434}, {0.61, 0.0, 0.7419, 0.7419},
           {0.615, 0.0, 0.7399, 0.7399}, {0.62, 0.0, 0.7376, 0.7376},  {0.625, 0.0, 0.7353, 0.7353},
           {0.63, 0.0, 0.7339, 0.7339},  {0.635, 0.0, 0.7319, 0.7319}, {0.64, 0.0, 0.7297, 0.7297},
           {0.645, 0.0, 0.7277, 0.7277}, {0.65, 0.0, 0.7261, 0.7261},  {0.655, 0.0, 0.724, 0.724},
           {0.66, 0.0, 0.7221, 0.7221},  {0.665, 0.0, 0.7202, 0.7202}, {0.67, 0.0, 0.7186, 0.7186},
           {0.675, 0.0, 0.7154, 0.7154}, {0.68, 0.0, 0.7133, 0.7133},  {0.685, 0.0, 0.7124, 0.7124},
           {0.69, 0.0, 0.7105, 0.7105},  {0.695, 0.0, 0.7086, 0.7086}, {0.7, 0.0, 0.706, 0.706},
           {0.705, 0.0, 0.7048, 0.7048}, {0.71, 0.0, 0.7026, 0.7026},  {0.715, 0.0, 0.7004, 0.7004},
           {0.72, 0.0, 0.6986, 0.6986},  {0.725, 0.0, 0.6959, 0.6959}, {0.73, 0.0, 0.6944, 0.6944},
           {0.735, 0.0, 0.6916, 0.6916}, {0.74, 0.0, 0.69, 0.69},      {0.745, 0.0, 0.6879, 0.6879},
           {0.75, 0.0, 0.6855, 0.6855},  {0.755, 0.0, 0.6828, 0.6828}, {0.76, 0.0, 0.6809, 0.6809},
           {0.765, 0.0, 0.6781, 0.6781}, {0.77, 0.0, 0.6749, 0.6749},  {0.775, 0.0, 0.6731, 0.6731},
           {0.78, 0.0, 0.671, 0.671},    {0.785, 0.0, 0.6687, 0.6687}, {0.79, 0.0, 0.6659, 0.6659},
           {0.795, 0.0, 0.6633, 0.6633}, {0.8, 0.0, 0.6615, 0.6615},   {0.805, 0.0, 0.6588, 0.6588},
           {0.81, 0.0, 0.6569, 0.6569},  {0.815, 0.0, 0.6547, 0.6547}, {0.82, 0.0, 0.6531, 0.6531},
           {0.825, 0.0, 0.6508, 0.6508}, {0.83, 0.0, 0.6488, 0.6488},  {0.835, 0.0, 0.6478, 0.6478},
           {0.84, 0.0, 0.6471, 0.6471},  {0.845, 0.0, 0.645, 0.645},   {0.85, 0.0, 0.6451, 0.6451},
           {0.855, 0.0, 0.6433, 0.6433}, {0.86, 0.0, 0.6451, 0.6451},  {0.865, 0.0, 0.6461, 0.6461},
           {0.87, 0.0, 0.6474, 0.6474},  {0.875, 0.0, 0.6457, 0.6457}, {0.88, 0.0, 0.6444, 0.6444},
           {0.885, 0.0, 0.6442, 0.6442}, {0.89, 0.0, 0.6447, 0.6447},  {0.895, 0.0, 0.644, 0.644},
           {0.9, 0.0, 0.6455, 0.6455},   {0.905, 0.0, 0.6448, 0.6448}, {0.91, 0.0, 0.6462, 0.6462},
           {0.915, 0.0, 0.6466, 0.6466}, {0.92, 0.0, 0.6481, 0.6481},  {0.925, 0.0, 0.6496, 0.6496},
           {0.93, 0.0, 0.6502, 0.6502},  {0.935, 0.0, 0.6511, 0.6511}, {0.94, 0.0, 0.6524, 0.6524},
           {0.945, 0.0, 0.6535, 0.6535}, {0.95, 0.0, 0.6542, 0.6542},  {0.955, 0.0, 0.6559, 0.6559},
           {0.96, 0.0, 0.657, 0.657},    {0.965, 0.0, 0.6576, 0.6576}, {0.97, 0.0, 0.6585, 0.6585},
           {0.975, 0.0, 0.6599, 0.6599}, {0.98, 0.0, 0.6606, 0.6606},  {0.985, 0.0, 0.6621, 0.6621},
           {0.99, 0.0, 0.6634, 0.6634},  {0.995, 0.0, 0.6646, 0.6646}, {1.0, 0.0, 0.6658, 0.6658},
           {1.005, 0.0, 0.6664, 0.6664}, {1.01, 0.0, 0.667, 0.667},    {1.015, 0.0, 0.6684, 0.6684},
           {1.02, 0.0, 0.6686, 0.6686},  {1.025, 0.0, 0.6706, 0.6706}, {1.03, 0.0, 0.6711, 0.6711},
           {1.035, 0.0, 0.6719, 0.6719}, {1.04, 0.0, 0.673, 0.673},    {1.045, 0.0, 0.6743, 0.6743},
           {1.05, 0.0, 0.6747, 0.6747},  {1.055, 0.0, 0.6757, 0.6757}, {1.06, 0.0, 0.6767, 0.6767},
           {1.065, 0.0, 0.6776, 0.6776}, {1.07, 0.0, 0.678, 0.678},    {1.075, 0.0, 0.679, 0.679},
           {1.08, 0.0, 0.6789, 0.6789},  {1.085, 0.0, 0.6807, 0.6807}, {1.09, 0.0, 0.6809, 0.6809},
           {1.095, 0.0, 0.6825, 0.6825}, {1.1, 0.0, 0.6834, 0.6834},   {1.105, 0.0, 0.6837, 0.6837},
           {1.11, 0.0, 0.6842, 0.6842},  {1.115, 0.0, 0.6854, 0.6854}, {1.12, 0.0, 0.6849, 0.6849},
           {1.125, 0.0, 0.6857, 0.6857}, {1.13, 0.0, 0.6861, 0.6861},  {1.135, 0.0, 0.6862, 0.6862},
           {1.14, 0.0, 0.6874, 0.6874},  {1.145, 0.0, 0.6877, 0.6877}, {1.15, 0.0, 0.688, 0.688},
           {1.155, 0.0, 0.6887, 0.6887}, {1.16, 0.0, 0.6895, 0.6895},  {1.165, 0.0, 0.6895, 0.6895},
           {1.17, 0.0, 0.6906, 0.6906},  {1.175, 0.0, 0.6902, 0.6902}, {1.18, 0.0, 0.6893, 0.6893},
           {1.185, 0.0, 0.6897, 0.6897}, {1.19, 0.0, 0.6904, 0.6904},  {1.195, 0.0, 0.6908, 0.6908},
           {1.2, 0.0, 0.6918, 0.6918},   {1.205, 0.0, 0.6923, 0.6923}, {1.21, 0.0, 0.6931, 0.6931},
           {1.215, 0.0, 0.6936, 0.6936}, {1.22, 0.0, 0.695, 0.695},    {1.225, 0.0, 0.6958, 0.6958},
           {1.23, 0.0, 0.6969, 0.6969},  {1.235, 0.0, 0.6972, 0.6972}, {1.24, 0.0, 0.6981, 0.6981},
           {1.245, 0.0, 0.6984, 0.6984}, {1.25, 0.0, 0.6991, 0.6991},  {1.255, 0.0, 0.6994, 0.6994},
           {1.26, 0.0, 0.6999, 0.6999},  {1.265, 0.0, 0.7015, 0.7015}, {1.27, 0.0, 0.7012, 0.7012},
           {1.275, 0.0, 0.7018, 0.7018}, {1.28, 0.0, 0.7016, 0.7016},  {1.285, 0.0, 0.702, 0.702},
           {1.29, 0.0, 0.7028, 0.7028},  {1.295, 0.0, 0.7034, 0.7034}, {1.3, 0.0, 0.7033, 0.7033},
           {1.305, 0.0, 0.7036, 0.7036}, {1.31, 0.0, 0.7039, 0.7039},  {1.315, 0.0, 0.7043, 0.7043},
           {1.32, 0.0, 0.7049, 0.7049},  {1.325, 0.0, 0.7053, 0.7053}, {1.33, 0.0, 0.7051, 0.7051},
           {1.335, 0.0, 0.7054, 0.7054}, {1.34, 0.0, 0.7058, 0.7058},  {1.345, 0.0, 0.7056, 0.7056},
           {1.35, 0.0, 0.7055, 0.7055},  {1.355, 0.0, 0.7056, 0.7056}, {1.36, 0.0, 0.7059, 0.7059},
           {1.365, 0.0, 0.7062, 0.7062}, {1.37, 0.0, 0.7059, 0.7059},  {1.375, 0.0, 0.7064, 0.7064},
           {1.38, 0.0, 0.7067, 0.7067},  {1.385, 0.0, 0.707, 0.707},   {1.39, 0.0, 0.7073, 0.7073},
           {1.395, 0.0, 0.7082, 0.7082}, {1.4, 0.0, 0.708, 0.708},     {1.405, 0.0, 0.7082, 0.7082},
           {1.41, 0.0, 0.7083, 0.7083},  {1.415, 0.0, 0.7084, 0.7084}, {1.42, 0.0, 0.7083, 0.7083},
           {1.425, 0.0, 0.7095, 0.7095}, {1.43, 0.0, 0.7104, 0.7104},  {1.435, 0.0, 0.7112, 0.7112},
           {1.44, 0.0, 0.7115, 0.7115},  {1.445, 0.0, 0.7123, 0.7123}, {1.45, 0.0, 0.7131, 0.7131},
           {1.455, 0.0, 0.7137, 0.7137}, {1.46, 0.0, 0.7144, 0.7144},  {1.465, 0.0, 0.7149, 0.7149},
           {1.47, 0.0, 0.7153, 0.7153},  {1.475, 0.0, 0.7157, 0.7157}, {1.48, 0.0, 0.7159, 0.7159},
           {1.485, 0.0, 0.7166, 0.7166}, {1.49, 0.0, 0.7173, 0.7173},  {1.495, 0.0, 0.718, 0.718},
           {1.5, 0.0, 0.7185, 0.7185},   {1.505, 0.0, 0.7186, 0.7186}, {1.51, 0.0, 0.7192, 0.7192},
           {1.515, 0.0, 0.7203, 0.7203}, {1.52, 0.0, 0.7208, 0.7208},  {1.525, 0.0, 0.7221, 0.7221},
           {1.53, 0.0, 0.7223, 0.7223},  {1.535, 0.0, 0.7227, 0.7227}, {1.54, 0.0, 0.7235, 0.7235},
           {1.545, 0.0, 0.7243, 0.7243}, {1.55, 0.0, 0.725, 0.725},    {1.555, 0.0, 0.7244, 0.7244},
           {1.56, 0.0, 0.7253, 0.7253},  {1.565, 0.0, 0.7265, 0.7265}, {1.57, 0.0, 0.7262, 0.7262},
           {1.575, 0.0, 0.7262, 0.7262}, {1.58, 0.0, 0.727, 0.727},    {1.585, 0.0, 0.7274, 0.7274},
           {1.59, 0.0, 0.7284, 0.7284},  {1.595, 0.0, 0.7284, 0.7284}, {1.6, 0.0, 0.7285, 0.7285},
           {1.605, 0.0, 0.7289, 0.7289}, {1.61, 0.0, 0.7292, 0.7292},  {1.615, 0.0, 0.7294, 0.7294},
           {1.62, 0.0, 0.7287, 0.7287},  {1.625, 0.0, 0.7285, 0.7285}, {1.63, 0.0, 0.7279, 0.7279},
           {1.635, 0.0, 0.7266, 0.7266}, {1.64, 0.0, 0.7252, 0.7252},  {1.645, 0.0, 0.7229, 0.7229},
           {1.65, 0.0, 0.7178, 0.7178},  {1.655, 0.0, 0.7141, 0.7141}, {1.66, 0.0, 0.7101, 0.7101},
           {1.665, 0.0, 0.7081, 0.7081}, {1.67, 0.0, 0.7093, 0.7093},  {1.675, 0.0, 0.7108, 0.7108},
           {1.68, 0.0, 0.7109, 0.7109},  {1.685, 0.0, 0.7102, 0.7102}, {1.69, 0.0, 0.7102, 0.7102},
           {1.695, 0.0, 0.7113, 0.7113}, {1.7, 0.0, 0.7127, 0.7127},   {1.705, 0.0, 0.7139, 0.7139},
           {1.71, 0.0, 0.7154, 0.7154},  {1.715, 0.0, 0.7157, 0.7157}, {1.72, 0.0, 0.7166, 0.7166},
           {1.725, 0.0, 0.7159, 0.7159}, {1.73, 0.0, 0.7153, 0.7153},  {1.735, 0.0, 0.7171, 0.7171},
           {1.74, 0.0, 0.7178, 0.7178},  {1.745, 0.0, 0.7183, 0.7183}, {1.75, 0.0, 0.7206, 0.7206},
           {1.755, 0.0, 0.7213, 0.7213}, {1.76, 0.0, 0.7231, 0.7231},  {1.765, 0.0, 0.7251, 0.7251},
           {1.77, 0.0, 0.7266, 0.7266},  {1.775, 0.0, 0.7291, 0.7291}, {1.78, 0.0, 0.7303, 0.7303},
           {1.785, 0.0, 0.7315, 0.7315}, {1.79, 0.0, 0.7327, 0.7327},  {1.795, 0.0, 0.7332, 0.7332},
           {1.8, 0.0, 0.7339, 0.7339},   {1.805, 0.0, 0.7346, 0.7346}, {1.81, 0.0, 0.7345, 0.7345},
           {1.815, 0.0, 0.7359, 0.7359}, {1.82, 0.0, 0.735, 0.735},    {1.825, 0.0, 0.736, 0.736},
           {1.83, 0.0, 0.7372, 0.7372},  {1.835, 0.0, 0.7377, 0.7377}, {1.84, 0.0, 0.7391, 0.7391},
           {1.845, 0.0, 0.7382, 0.7382}, {1.85, 0.0, 0.7397, 0.7397},  {1.855, 0.0, 0.7411, 0.7411},
           {1.86, 0.0, 0.742, 0.742},    {1.865, 0.0, 0.7416, 0.7416}, {1.87, 0.0, 0.7415, 0.7415},
           {1.875, 0.0, 0.7425, 0.7425}, {1.88, 0.0, 0.7432, 0.7432},  {1.885, 0.0, 0.7417, 0.7417},
           {1.89, 0.0, 0.7415, 0.7415},  {1.895, 0.0, 0.7406, 0.7406}, {1.9, 0.0, 0.7399, 0.7399},
           {1.905, 0.0, 0.7389, 0.7389}, {1.91, 0.0, 0.7393, 0.7393},  {1.915, 0.0, 0.7395, 0.7395},
           {1.92, 0.0, 0.7401, 0.7401},  {1.925, 0.0, 0.7397, 0.7397}, {1.93, 0.0, 0.7408, 0.7408},
           {1.935, 0.0, 0.7417, 0.7417}, {1.94, 0.0, 0.7423, 0.7423},  {1.945, 0.0, 0.7423, 0.7423},
           {1.95, 0.0, 0.7422, 0.7422},  {1.955, 0.0, 0.7435, 0.7435}, {1.96, 0.0, 0.7422, 0.7422},
           {1.965, 0.0, 0.7432, 0.7432}, {1.97, 0.0, 0.7449, 0.7449},  {1.975, 0.0, 0.7479, 0.7479},
           {1.98, 0.0, 0.7477, 0.7477},  {1.985, 0.0, 0.7479, 0.7479}, {1.99, 0.0, 0.7481, 0.7481},
           {1.995, 0.0, 0.7493, 0.7493}, {2.0, 0.0, 0.7496, 0.7496},   {2.005, 0.0, 0.7501, 0.7501},
           {2.01, 0.0, 0.7503, 0.7503},  {2.015, 0.0, 0.7518, 0.7518}, {2.02, 0.0, 0.7517, 0.7517},
           {2.025, 0.0, 0.7523, 0.7523}, {2.03, 0.0, 0.7528, 0.7528},  {2.035, 0.0, 0.7531, 0.7531},
           {2.04, 0.0, 0.7531, 0.7531},  {2.045, 0.0, 0.7529, 0.7529}, {2.05, 0.0, 0.7539, 0.7539},
           {2.055, 0.0, 0.7564, 0.7564}, {2.06, 0.0, 0.7556, 0.7556},  {2.065, 0.0, 0.7544, 0.7544},
           {2.07, 0.0, 0.7537, 0.7537},  {2.075, 0.0, 0.7541, 0.7541}, {2.08, 0.0, 0.7517, 0.7517},
           {2.085, 0.0, 0.7532, 0.7532}, {2.09, 0.0, 0.7546, 0.7546},  {2.095, 0.0, 0.7541, 0.7541},
           {2.1, 0.0, 0.7539, 0.7539},   {2.105, 0.0, 0.7555, 0.7555}, {2.11, 0.0, 0.7536, 0.7536},
           {2.115, 0.0, 0.7544, 0.7544}, {2.12, 0.0, 0.7501, 0.7501},  {2.125, 0.0, 0.7442, 0.7442},
           {2.13, 0.0, 0.7399, 0.7399},  {2.135, 0.0, 0.7331, 0.7331}, {2.14, 0.0, 0.7283, 0.7283},
           {2.145, 0.0, 0.7282, 0.7282}, {2.15, 0.0, 0.731, 0.731},    {2.155, 0.0, 0.7431, 0.7431},
           {2.16, 0.0, 0.7431, 0.7431},  {2.165, 0.0, 0.7459, 0.7459}, {2.17, 0.0, 0.7519, 0.7519},
           {2.175, 0.0, 0.753, 0.753},   {2.18, 0.0, 0.7488, 0.7488},  {2.185, 0.0, 0.7548, 0.7548},
           {2.19, 0.0, 0.7505, 0.7505},  {2.195, 0.0, 0.7495, 0.7495}, {2.2, 0.0, 0.7456, 0.7456},
           {2.205, 0.0, 0.7457, 0.7457}, {2.21, 0.0, 0.7468, 0.7468},  {2.215, 0.0, 0.744, 0.744},
           {2.22, 0.0, 0.7412, 0.7412},  {2.225, 0.0, 0.7417, 0.7417}, {2.23, 0.0, 0.7395, 0.7395},
           {2.235, 0.0, 0.7341, 0.7341}, {2.24, 0.0, 0.7227, 0.7227},  {2.245, 0.0, 0.7105, 0.7105},
           {2.25, 0.0, 0.6971, 0.6971},  {2.255, 0.0, 0.676, 0.676},   {2.26, 0.0, 0.6582, 0.6582},
           {2.265, 0.0, 0.646, 0.646},   {2.27, 0.0, 0.6434, 0.6434},  {2.275, 0.0, 0.6433, 0.6433},
           {2.28, 0.0, 0.6464, 0.6464},  {2.285, 0.0, 0.6496, 0.6496}, {2.29, 0.0, 0.6549, 0.6549},
           {2.295, 0.0, 0.6597, 0.6597}, {2.3, 0.0, 0.6562, 0.6562},   {2.305, 0.0, 0.6573, 0.6573},
           {2.31, 0.0, 0.6587, 0.6587},  {2.315, 0.0, 0.66, 0.66},     {2.32, 0.0, 0.6659, 0.6659},
           {2.325, 0.0, 0.6758, 0.6758}, {2.33, 0.0, 0.676, 0.676},    {2.335, 0.0, 0.6785, 0.6785},
           {2.34, 0.0, 0.68, 0.68},      {2.345, 0.0, 0.681, 0.681},   {2.35, 0.0, 0.6778, 0.6778},
           {2.355, 0.0, 0.6796, 0.6796}, {2.36, 0.0, 0.683, 0.683},    {2.365, 0.0, 0.6793, 0.6793},
           {2.37, 0.0, 0.6823, 0.6823},  {2.375, 0.0, 0.6864, 0.6864}, {2.38, 0.0, 0.6836, 0.6836},
           {2.385, 0.0, 0.6786, 0.6786}, {2.39, 0.0, 0.6835, 0.6835},  {2.395, 0.0, 0.6795, 0.6795},
           {2.4, 0.0, 0.6834, 0.6834},   {2.405, 0.0, 0.689, 0.689},   {2.41, 0.0, 0.686, 0.686},
           {2.415, 0.0, 0.6899, 0.6899}, {2.42, 0.0, 0.6884, 0.6884},  {2.425, 0.0, 0.6912, 0.6912},
           {2.43, 0.0, 0.6936, 0.6936},  {2.435, 0.0, 0.6945, 0.6945}, {2.44, 0.0, 0.6927, 0.6927},
           {2.445, 0.0, 0.6792, 0.6792}, {2.45, 0.0, 0.6733, 0.6733},  {2.455, 0.0, 0.6649, 0.6649},
           {2.46, 0.0, 0.6712, 0.6712},  {2.465, 0.0, 0.6756, 0.6756}, {2.47, 0.0, 0.6799, 0.6799},
           {2.475, 0.0, 0.6874, 0.6874}, {2.48, 0.0, 0.6999, 0.6999},  {2.485, 0.0, 0.6993, 0.6993},
           {2.49, 0.0, 0.7081, 0.7081},  {2.495, 0.0, 0.7157, 0.7157}, {2.5, 0.0, 0.719, 0.719}});

        return aMeasurements_31100;
    }

protected:
    virtual void SetUp()
    {
        const auto solarRadiation{loadSolarRadiationFile()};

        double thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          loadSampleData_NFRC_102(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        // Venetian blind material
        thickness = 0.0001;   // [m]
        auto aMaterialVenetian = SingleLayerOptics::Material::nBandMaterial(
          loadSampleData_NFRC_31100(), thickness, MaterialType::Monolithic);

        // make cell geometry
        const auto slatWidth = 0.005;     // m
        const auto slatSpacing = 0.007;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0.003;
        const size_t numOfSlatSegments = 5;

        std::shared_ptr<CBSDFLayer> Layer_Venetian =
          CBSDFLayerMaker::getVenetianLayer(aMaterialVenetian,
                                            aBSDF,
                                            slatWidth,
                                            slatSpacing,
                                            slatTiltAngle,
                                            curvatureRadius,
                                            numOfSlatSegments,
                                            DistributionMethod::DirectionalDiffuse,
                                            true);

        m_Layer = CMultiPaneBSDF::create({Layer_Venetian, Layer_102});

        const CalculationProperties input{loadSolarRadiationFile(),
                                          loadSolarRadiationFile().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_102_VenetianDirectional_n_Band_Material, TestBSDF1)
{
    SCOPED_TRACE("Begin Test: Specular and venetian directional IGU - BSDF.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    const double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.39040447912219783, tauDiff, 1e-6);

    const double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.081346841038713175, rhoDiff, 1e-6);

    const double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.47745840238573878, absDiff1, 1e-6);

    const double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.050790277453349929, absDiff2, 1e-6);

    const double theta = 0;
    const double phi = 0;

    const double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.48474773923246411, tauHem, 1e-6);

    const double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.4142871458079016, tauDir, 1e-6);

    const double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.075941438123243721, rhoHem, 1e-6);

    const double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.019480048289373025, rhoDir, 1e-6);

    const double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.38456381613967494, abs1, 1e-6);

    const double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.054747006504616808, abs2, 1e-6);
}
