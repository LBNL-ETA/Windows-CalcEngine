#include <memory>
#include <gtest/gtest.h>

#include "WCESpectralAveraging.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

class TestSpecularLayer_1042 : public testing::Test
{
private:
    std::shared_ptr<CBSDFLayer> m_Layer;

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


    std::shared_ptr<CSpectralSampleData> loadSampleData_NFRC_1042()
    {
        return CSpectralSampleData::create(
          {{0.300, 0.0006, 0.0518, 0.2713},  {0.305, 0.0006, 0.0509, 0.2624},
           {0.310, 0.0017, 0.0508, 0.2576},  {0.315, 0.0072, 0.0508, 0.2569},
           {0.320, 0.0219, 0.0511, 0.2579},  {0.325, 0.0486, 0.0525, 0.2607},
           {0.330, 0.0857, 0.0560, 0.2646},  {0.335, 0.1280, 0.0623, 0.2664},
           {0.340, 0.1707, 0.0719, 0.2668},  {0.345, 0.2125, 0.0840, 0.2680},
           {0.350, 0.2536, 0.0990, 0.2706},  {0.355, 0.2953, 0.1165, 0.2735},
           {0.360, 0.3370, 0.1365, 0.2773},  {0.365, 0.3774, 0.1579, 0.2809},
           {0.370, 0.4125, 0.1773, 0.2829},  {0.375, 0.4414, 0.1931, 0.2836},
           {0.380, 0.4671, 0.2074, 0.2827},  {0.385, 0.4953, 0.2244, 0.2814},
           {0.390, 0.5229, 0.2415, 0.2801},  {0.395, 0.5455, 0.2553, 0.2781},
           {0.400, 0.5630, 0.2651, 0.2757},  {0.405, 0.5764, 0.2718, 0.2728},
           {0.410, 0.5870, 0.2759, 0.2695},  {0.415, 0.5955, 0.2783, 0.2657},
           {0.420, 0.6025, 0.2798, 0.2623},  {0.425, 0.6098, 0.2809, 0.2584},
           {0.430, 0.6166, 0.2812, 0.2544},  {0.435, 0.6226, 0.2805, 0.2504},
           {0.440, 0.6286, 0.2794, 0.2462},  {0.445, 0.6346, 0.2780, 0.2419},
           {0.450, 0.6412, 0.2766, 0.2374},  {0.455, 0.6481, 0.2751, 0.2327},
           {0.460, 0.6542, 0.2730, 0.2281},  {0.465, 0.6595, 0.2702, 0.2235},
           {0.470, 0.6643, 0.2672, 0.2187},  {0.475, 0.6689, 0.2640, 0.2141},
           {0.480, 0.6734, 0.2607, 0.2095},  {0.485, 0.6784, 0.2571, 0.2046},
           {0.490, 0.6837, 0.2532, 0.1996},  {0.495, 0.6882, 0.2492, 0.1944},
           {0.500, 0.6928, 0.2449, 0.1892},  {0.505, 0.6968, 0.2406, 0.1840},
           {0.510, 0.7004, 0.2365, 0.1792},  {0.515, 0.7036, 0.2324, 0.1744},
           {0.520, 0.7067, 0.2284, 0.1697},  {0.525, 0.7099, 0.2244, 0.1650},
           {0.530, 0.7127, 0.2206, 0.1606},  {0.535, 0.7156, 0.2167, 0.1563},
           {0.540, 0.7179, 0.2130, 0.1521},  {0.545, 0.7200, 0.2094, 0.1481},
           {0.550, 0.7224, 0.2061, 0.1444},  {0.555, 0.7244, 0.2029, 0.1409},
           {0.560, 0.7259, 0.1997, 0.1375},  {0.565, 0.7265, 0.1967, 0.1344},
           {0.570, 0.7267, 0.1940, 0.1317},  {0.575, 0.7268, 0.1914, 0.1292},
           {0.580, 0.7267, 0.1889, 0.1270},  {0.585, 0.7257, 0.1868, 0.1252},
           {0.590, 0.7249, 0.1849, 0.1237},  {0.595, 0.7237, 0.1835, 0.1224},
           {0.600, 0.7228, 0.1825, 0.1217},  {0.605, 0.7211, 0.1815, 0.1211},
           {0.610, 0.7187, 0.1806, 0.1207},  {0.615, 0.7164, 0.1802, 0.1209},
           {0.620, 0.7138, 0.1801, 0.1214},  {0.625, 0.7110, 0.1802, 0.1223},
           {0.630, 0.7078, 0.1807, 0.1236},  {0.635, 0.7037, 0.1813, 0.1251},
           {0.640, 0.6999, 0.1823, 0.1269},  {0.645, 0.6958, 0.1837, 0.1292},
           {0.650, 0.6916, 0.1849, 0.1317},  {0.655, 0.6870, 0.1865, 0.1345},
           {0.660, 0.6823, 0.1885, 0.1377},  {0.665, 0.6773, 0.1906, 0.1411},
           {0.670, 0.6723, 0.1931, 0.1451},  {0.675, 0.6670, 0.1957, 0.1491},
           {0.680, 0.6614, 0.1985, 0.1533},  {0.685, 0.6551, 0.2015, 0.1580},
           {0.690, 0.6492, 0.2047, 0.1630},  {0.695, 0.6432, 0.2080, 0.1682},
           {0.700, 0.6366, 0.2115, 0.1733},  {0.705, 0.6299, 0.2150, 0.1790},
           {0.710, 0.6231, 0.2186, 0.1847},  {0.715, 0.6163, 0.2223, 0.1908},
           {0.720, 0.6093, 0.2259, 0.1965},  {0.725, 0.6023, 0.2299, 0.2030},
           {0.730, 0.5953, 0.2338, 0.2094},  {0.735, 0.5877, 0.2381, 0.2159},
           {0.740, 0.5808, 0.2422, 0.2228},  {0.745, 0.5732, 0.2466, 0.2299},
           {0.750, 0.5655, 0.2502, 0.2360},  {0.755, 0.5580, 0.2542, 0.2430},
           {0.760, 0.5506, 0.2585, 0.2501},  {0.765, 0.5433, 0.2631, 0.2574},
           {0.770, 0.5355, 0.2671, 0.2643},  {0.775, 0.5280, 0.2717, 0.2720},
           {0.780, 0.5207, 0.2763, 0.2794},  {0.785, 0.5137, 0.2806, 0.2872},
           {0.790, 0.5057, 0.2854, 0.2947},  {0.795, 0.4980, 0.2898, 0.3023},
           {0.800, 0.4910, 0.2945, 0.3095},  {0.805, 0.4843, 0.2990, 0.3179},
           {0.810, 0.4781, 0.3035, 0.3262},  {0.815, 0.4710, 0.3085, 0.3347},
           {0.820, 0.4633, 0.3132, 0.3422},  {0.825, 0.4567, 0.3172, 0.3502},
           {0.830, 0.4496, 0.3217, 0.3570},  {0.835, 0.4409, 0.3260, 0.3654},
           {0.840, 0.4340, 0.3296, 0.3724},  {0.845, 0.4272, 0.3327, 0.3783},
           {0.850, 0.4210, 0.3355, 0.3841},  {0.855, 0.4152, 0.3400, 0.3919},
           {0.860, 0.4092, 0.3443, 0.3992},  {0.865, 0.4034, 0.3483, 0.4062},
           {0.870, 0.3976, 0.3524, 0.4132},  {0.875, 0.3920, 0.3560, 0.4198},
           {0.880, 0.3864, 0.3595, 0.4263},  {0.885, 0.3811, 0.3632, 0.4327},
           {0.890, 0.3757, 0.3665, 0.4390},  {0.895, 0.3704, 0.3704, 0.4455},
           {0.900, 0.3650, 0.3740, 0.4519},  {0.905, 0.3599, 0.3768, 0.4575},
           {0.910, 0.3547, 0.3796, 0.4629},  {0.915, 0.3498, 0.3823, 0.4684},
           {0.920, 0.3448, 0.3854, 0.4739},  {0.925, 0.3397, 0.3885, 0.4793},
           {0.930, 0.3351, 0.3914, 0.4849},  {0.935, 0.3303, 0.3946, 0.4906},
           {0.940, 0.3256, 0.3980, 0.4962},  {0.945, 0.3212, 0.4011, 0.5020},
           {0.950, 0.3166, 0.4045, 0.5079},  {0.955, 0.3122, 0.4077, 0.5136},
           {0.960, 0.3078, 0.4110, 0.5192},  {0.965, 0.3036, 0.4146, 0.5249},
           {0.970, 0.2995, 0.4177, 0.5305},  {0.975, 0.2953, 0.4212, 0.5363},
           {0.980, 0.2914, 0.4247, 0.5424},  {0.985, 0.2875, 0.4281, 0.5478},
           {0.990, 0.2836, 0.4318, 0.5537},  {0.995, 0.2799, 0.4352, 0.5593},
           {1.000, 0.2760, 0.4388, 0.5649},  {1.005, 0.2723, 0.4414, 0.5693},
           {1.010, 0.2688, 0.4441, 0.5738},  {1.015, 0.2653, 0.4468, 0.5784},
           {1.020, 0.2616, 0.4494, 0.5829},  {1.025, 0.2586, 0.4524, 0.5876},
           {1.030, 0.2550, 0.4549, 0.5918},  {1.035, 0.2520, 0.4579, 0.5964},
           {1.040, 0.2487, 0.4606, 0.6007},  {1.045, 0.2455, 0.4634, 0.6050},
           {1.050, 0.2424, 0.4660, 0.6092},  {1.055, 0.2393, 0.4689, 0.6137},
           {1.060, 0.2363, 0.4713, 0.6179},  {1.065, 0.2334, 0.4740, 0.6215},
           {1.070, 0.2303, 0.4765, 0.6258},  {1.075, 0.2276, 0.4791, 0.6297},
           {1.080, 0.2247, 0.4816, 0.6336},  {1.085, 0.2221, 0.4842, 0.6375},
           {1.090, 0.2194, 0.4867, 0.6412},  {1.095, 0.2170, 0.4893, 0.6449},
           {1.100, 0.2142, 0.4915, 0.6483},  {1.105, 0.2118, 0.4941, 0.6518},
           {1.110, 0.2092, 0.4961, 0.6551},  {1.115, 0.2065, 0.4985, 0.6583},
           {1.120, 0.2043, 0.5006, 0.6615},  {1.125, 0.2019, 0.5028, 0.6647},
           {1.130, 0.1997, 0.5053, 0.6680},  {1.135, 0.1975, 0.5075, 0.6712},
           {1.140, 0.1953, 0.5098, 0.6742},  {1.145, 0.1931, 0.5119, 0.6773},
           {1.150, 0.1910, 0.5141, 0.6801},  {1.155, 0.1887, 0.5163, 0.6833},
           {1.160, 0.1869, 0.5187, 0.6863},  {1.165, 0.1851, 0.5209, 0.6894},
           {1.170, 0.1827, 0.5230, 0.6921},  {1.175, 0.1808, 0.5253, 0.6950},
           {1.180, 0.1788, 0.5273, 0.6977},  {1.185, 0.1772, 0.5295, 0.7005},
           {1.190, 0.1751, 0.5321, 0.7032},  {1.195, 0.1735, 0.5341, 0.7060},
           {1.200, 0.1715, 0.5363, 0.7088},  {1.205, 0.1697, 0.5385, 0.7112},
           {1.210, 0.1678, 0.5405, 0.7139},  {1.215, 0.1663, 0.5426, 0.7161},
           {1.220, 0.1647, 0.5448, 0.7185},  {1.225, 0.1631, 0.5470, 0.7209},
           {1.230, 0.1611, 0.5492, 0.7236},  {1.235, 0.1598, 0.5519, 0.7263},
           {1.240, 0.1583, 0.5540, 0.7286},  {1.245, 0.1568, 0.5562, 0.7311},
           {1.250, 0.1551, 0.5584, 0.7334},  {1.255, 0.1537, 0.5607, 0.7354},
           {1.260, 0.1522, 0.5627, 0.7374},  {1.265, 0.1507, 0.5649, 0.7396},
           {1.270, 0.1494, 0.5671, 0.7416},  {1.275, 0.1480, 0.5691, 0.7436},
           {1.280, 0.1468, 0.5716, 0.7458},  {1.285, 0.1451, 0.5738, 0.7477},
           {1.290, 0.1440, 0.5761, 0.7500},  {1.295, 0.1428, 0.5785, 0.7517},
           {1.300, 0.1416, 0.5809, 0.7539},  {1.305, 0.1403, 0.5831, 0.7558},
           {1.310, 0.1392, 0.5857, 0.7580},  {1.315, 0.1378, 0.5874, 0.7592},
           {1.320, 0.1367, 0.5897, 0.7611},  {1.325, 0.1355, 0.5921, 0.7629},
           {1.330, 0.1342, 0.5946, 0.7648},  {1.335, 0.1332, 0.5970, 0.7669},
           {1.340, 0.1319, 0.5995, 0.7686},  {1.345, 0.1309, 0.6016, 0.7705},
           {1.350, 0.1300, 0.6040, 0.7720},  {1.355, 0.1290, 0.6066, 0.7740},
           {1.360, 0.1279, 0.6089, 0.7753},  {1.365, 0.1268, 0.6110, 0.7767},
           {1.370, 0.1255, 0.6133, 0.7786},  {1.375, 0.1245, 0.6154, 0.7797},
           {1.380, 0.1235, 0.6168, 0.7805},  {1.385, 0.1225, 0.6187, 0.7818},
           {1.390, 0.1215, 0.6201, 0.7829},  {1.395, 0.1206, 0.6219, 0.7847},
           {1.400, 0.1197, 0.6240, 0.7865},  {1.405, 0.1185, 0.6262, 0.7884},
           {1.410, 0.1178, 0.6285, 0.7900},  {1.415, 0.1166, 0.6313, 0.7921},
           {1.420, 0.1157, 0.6338, 0.7934},  {1.425, 0.1149, 0.6367, 0.7951},
           {1.430, 0.1143, 0.6394, 0.7965},  {1.435, 0.1136, 0.6423, 0.7983},
           {1.440, 0.1127, 0.6449, 0.7993},  {1.445, 0.1117, 0.6474, 0.8005},
           {1.450, 0.1109, 0.6501, 0.8021},  {1.455, 0.1101, 0.6529, 0.8038},
           {1.460, 0.1093, 0.6554, 0.8050},  {1.465, 0.1086, 0.6578, 0.8062},
           {1.470, 0.1077, 0.6602, 0.8072},  {1.475, 0.1069, 0.6627, 0.8088},
           {1.480, 0.1061, 0.6652, 0.8099},  {1.485, 0.1053, 0.6673, 0.8111},
           {1.490, 0.1047, 0.6696, 0.8123},  {1.495, 0.1040, 0.6718, 0.8134},
           {1.500, 0.1032, 0.6741, 0.8147},  {1.505, 0.1024, 0.6763, 0.8159},
           {1.510, 0.1016, 0.6783, 0.8171},  {1.515, 0.1010, 0.6804, 0.8182},
           {1.520, 0.1002, 0.6824, 0.8192},  {1.525, 0.0997, 0.6844, 0.8204},
           {1.530, 0.0989, 0.6864, 0.8215},  {1.535, 0.0982, 0.6885, 0.8225},
           {1.540, 0.0974, 0.6904, 0.8235},  {1.545, 0.0968, 0.6921, 0.8244},
           {1.550, 0.0960, 0.6940, 0.8258},  {1.555, 0.0953, 0.6959, 0.8269},
           {1.560, 0.0946, 0.6976, 0.8277},  {1.565, 0.0939, 0.6992, 0.8285},
           {1.570, 0.0934, 0.7008, 0.8294},  {1.575, 0.0929, 0.7025, 0.8305},
           {1.580, 0.0920, 0.7040, 0.8315},  {1.585, 0.0912, 0.7056, 0.8324},
           {1.590, 0.0906, 0.7072, 0.8334},  {1.595, 0.0898, 0.7087, 0.8344},
           {1.600, 0.0891, 0.7100, 0.8353},  {1.605, 0.0886, 0.7112, 0.8362},
           {1.610, 0.0882, 0.7126, 0.8369},  {1.615, 0.0874, 0.7140, 0.8378},
           {1.620, 0.0867, 0.7151, 0.8387},  {1.625, 0.0862, 0.7163, 0.8395},
           {1.630, 0.0856, 0.7176, 0.8406},  {1.635, 0.0849, 0.7187, 0.8414},
           {1.640, 0.0842, 0.7196, 0.8421},  {1.645, 0.0836, 0.7207, 0.8429},
           {1.650, 0.0832, 0.7218, 0.8437},  {1.655, 0.0826, 0.7228, 0.8448},
           {1.660, 0.0820, 0.7236, 0.8453},  {1.665, 0.0814, 0.7245, 0.8460},
           {1.670, 0.0806, 0.7252, 0.8469},  {1.675, 0.0801, 0.7260, 0.8478},
           {1.680, 0.0796, 0.7267, 0.8486},  {1.685, 0.0790, 0.7275, 0.8493},
           {1.690, 0.0783, 0.7281, 0.8500},  {1.695, 0.0778, 0.7288, 0.8507},
           {1.700, 0.0773, 0.7295, 0.8516},  {1.705, 0.0768, 0.7301, 0.8525},
           {1.710, 0.0763, 0.7306, 0.8529},  {1.715, 0.0758, 0.7311, 0.8536},
           {1.720, 0.0753, 0.7323, 0.8555},  {1.725, 0.0746, 0.7328, 0.8561},
           {1.730, 0.0742, 0.7331, 0.8567},  {1.735, 0.0736, 0.7336, 0.8573},
           {1.740, 0.0732, 0.7339, 0.8579},  {1.745, 0.0727, 0.7344, 0.8585},
           {1.750, 0.0722, 0.7348, 0.8592},  {1.755, 0.0716, 0.7350, 0.8599},
           {1.760, 0.0711, 0.7353, 0.8605},  {1.765, 0.0707, 0.7354, 0.8610},
           {1.770, 0.0703, 0.7358, 0.8617},  {1.775, 0.0698, 0.7362, 0.8623},
           {1.780, 0.0693, 0.7363, 0.8628},  {1.785, 0.0688, 0.7363, 0.8632},
           {1.790, 0.0683, 0.7365, 0.8638},  {1.795, 0.0678, 0.7375, 0.8653},
           {1.800, 0.0674, 0.7371, 0.8651},  {1.805, 0.0668, 0.7372, 0.8655},
           {1.810, 0.0664, 0.7373, 0.8659},  {1.815, 0.0660, 0.7373, 0.8665},
           {1.820, 0.0656, 0.7376, 0.8671},  {1.825, 0.0652, 0.7379, 0.8676},
           {1.830, 0.0647, 0.7382, 0.8682},  {1.835, 0.0643, 0.7385, 0.8687},
           {1.840, 0.0637, 0.7386, 0.8692},  {1.845, 0.0632, 0.7387, 0.8698},
           {1.850, 0.0629, 0.7390, 0.8700},  {1.855, 0.0625, 0.7392, 0.8704},
           {1.860, 0.0622, 0.7412, 0.8730},  {1.865, 0.0619, 0.7413, 0.8737},
           {1.870, 0.0614, 0.7415, 0.8742},  {1.875, 0.0609, 0.7417, 0.8746},
           {1.880, 0.0605, 0.7422, 0.8751},  {1.885, 0.0602, 0.7425, 0.8755},
           {1.890, 0.0599, 0.7427, 0.8759},  {1.895, 0.0596, 0.7429, 0.8766},
           {1.900, 0.0593, 0.7431, 0.8770},  {1.905, 0.0589, 0.7432, 0.8775},
           {1.910, 0.0583, 0.7433, 0.8779},  {1.915, 0.0579, 0.7436, 0.8784},
           {1.920, 0.0576, 0.7437, 0.8787},  {1.925, 0.0573, 0.7438, 0.8790},
           {1.930, 0.0571, 0.7441, 0.8796},  {1.935, 0.0567, 0.7443, 0.8800},
           {1.940, 0.0562, 0.7444, 0.8802},  {1.945, 0.0559, 0.7446, 0.8806},
           {1.950, 0.0557, 0.7448, 0.8811},  {1.955, 0.0554, 0.7448, 0.8813},
           {1.960, 0.0551, 0.7449, 0.8815},  {1.965, 0.0547, 0.7453, 0.8820},
           {1.970, 0.0545, 0.7455, 0.8824},  {1.975, 0.0542, 0.7457, 0.8829},
           {1.980, 0.0536, 0.7459, 0.8833},  {1.985, 0.0534, 0.7460, 0.8837},
           {1.990, 0.0532, 0.7461, 0.8839},  {1.995, 0.0527, 0.7465, 0.8844},
           {2.000, 0.0524, 0.7468, 0.8849},  {2.005, 0.0520, 0.7471, 0.8854},
           {2.010, 0.0517, 0.7477, 0.8859},  {2.015, 0.0516, 0.7481, 0.8864},
           {2.020, 0.0513, 0.7484, 0.8868},  {2.025, 0.0510, 0.7489, 0.8873},
           {2.030, 0.0508, 0.7496, 0.8880},  {2.035, 0.0504, 0.7501, 0.8886},
           {2.040, 0.0502, 0.7507, 0.8892},  {2.045, 0.0499, 0.7512, 0.8896},
           {2.050, 0.0496, 0.7515, 0.8901},  {2.055, 0.0492, 0.7521, 0.8908},
           {2.060, 0.0491, 0.7527, 0.8912},  {2.065, 0.0489, 0.7531, 0.8914},
           {2.070, 0.0485, 0.7533, 0.8919},  {2.075, 0.0483, 0.7536, 0.8922},
           {2.080, 0.0480, 0.7539, 0.8926},  {2.085, 0.0476, 0.7542, 0.8931},
           {2.090, 0.0474, 0.7545, 0.8934},  {2.095, 0.0472, 0.7547, 0.8937},
           {2.100, 0.0468, 0.7549, 0.8942},  {2.105, 0.0467, 0.7552, 0.8947},
           {2.110, 0.0464, 0.7555, 0.8952},  {2.115, 0.0461, 0.7556, 0.8957},
           {2.120, 0.0459, 0.7559, 0.8965},  {2.125, 0.0457, 0.7545, 0.8951},
           {2.130, 0.0455, 0.7548, 0.8958},  {2.135, 0.0451, 0.7548, 0.8965},
           {2.140, 0.0448, 0.7551, 0.8975},  {2.145, 0.0446, 0.7554, 0.8987},
           {2.150, 0.0446, 0.7549, 0.8989},  {2.155, 0.0440, 0.7543, 0.8994},
           {2.160, 0.0440, 0.7536, 0.8998},  {2.165, 0.0438, 0.7527, 0.9004},
           {2.170, 0.0433, 0.7505, 0.9003},  {2.175, 0.0430, 0.7481, 0.9005},
           {2.180, 0.0426, 0.7455, 0.9012},  {2.185, 0.0424, 0.7414, 0.9004},
           {2.190, 0.0421, 0.7383, 0.9008},  {2.195, 0.0418, 0.7353, 0.9011},
           {2.200, 0.0414, 0.7322, 0.9008},  {2.205, 0.0409, 0.7299, 0.9007},
           {2.210, 0.0409, 0.7279, 0.9000},  {2.215, 0.0408, 0.7266, 0.8994},
           {2.220, 0.0403, 0.7267, 0.9001},  {2.225, 0.0402, 0.7268, 0.9003},
           {2.230, 0.0399, 0.7275, 0.9005},  {2.235, 0.0400, 0.7288, 0.9012},
           {2.240, 0.0399, 0.7301, 0.9018},  {2.245, 0.0396, 0.7317, 0.9025},
           {2.250, 0.0395, 0.7330, 0.9025},  {2.255, 0.0391, 0.7345, 0.9030},
           {2.260, 0.0391, 0.7365, 0.9043},  {2.265, 0.0391, 0.7384, 0.9050},
           {2.270, 0.0389, 0.7402, 0.9061},  {2.275, 0.0387, 0.7417, 0.9065},
           {2.280, 0.0385, 0.7437, 0.9079},  {2.285, 0.0383, 0.7447, 0.9081},
           {2.290, 0.0380, 0.7461, 0.9088},  {2.295, 0.0379, 0.7470, 0.9088},
           {2.300, 0.0379, 0.7484, 0.9094},  {2.305, 0.0377, 0.7495, 0.9100},
           {2.310, 0.0376, 0.7516, 0.9121},  {2.315, 0.0373, 0.7521, 0.9120},
           {2.320, 0.0372, 0.7529, 0.9126},  {2.325, 0.0370, 0.7537, 0.9132},
           {2.330, 0.0368, 0.7544, 0.9135},  {2.335, 0.0365, 0.7557, 0.9147},
           {2.340, 0.0365, 0.7560, 0.9148},  {2.345, 0.0360, 0.7567, 0.9153},
           {2.350, 0.0358, 0.7564, 0.9151},  {2.355, 0.0358, 0.7566, 0.9154},
           {2.360, 0.0355, 0.7577, 0.9165},  {2.365, 0.0356, 0.7569, 0.9156},
           {2.370, 0.0355, 0.7569, 0.9163},  {2.375, 0.0349, 0.7568, 0.9163},
           {2.380, 0.0350, 0.7562, 0.9164},  {2.385, 0.0348, 0.7552, 0.9156},
           {2.390, 0.0347, 0.7555, 0.9172},  {2.395, 0.0348, 0.7546, 0.9164},
           {2.400, 0.0345, 0.7550, 0.9175},  {2.405, 0.0347, 0.7539, 0.9179},
           {2.410, 0.0338, 0.7540, 0.9188},  {2.415, 0.0337, 0.7517, 0.9175},
           {2.420, 0.0336, 0.7524, 0.9198},  {2.425, 0.0337, 0.7505, 0.9198},
           {2.430, 0.0326, 0.7500, 0.9203},  {2.435, 0.0329, 0.7476, 0.9202},
           {2.440, 0.0333, 0.7451, 0.9207},  {2.445, 0.0323, 0.7436, 0.9204},
           {2.450, 0.0324, 0.7437, 0.9210},  {2.455, 0.0330, 0.7417, 0.9210},
           {2.460, 0.0339, 0.7409, 0.9199},  {2.465, 0.0306, 0.7374, 0.9198},
           {2.470, 0.0291, 0.7366, 0.9207},  {2.475, 0.0295, 0.7359, 0.9229},
           {2.480, 0.0343, 0.7301, 0.9167},  {2.485, 0.0282, 0.7327, 0.9163},
           {2.490, 0.0301, 0.7354, 0.9226},  {2.495, 0.0286, 0.7317, 0.9226},
           {2.500, 0.0330, 0.7297, 0.9266},  {3.000, 0.0028, 0.0546, 0.9347},
           {3.500, 0.0020, 0.0405, 0.9444},  {4.000, 0.0025, 0.0424, 0.9472},
           {4.500, 0.0113, 0.0337, 0.9504},  {5.000, 0.0003, 0.0308, 0.9546},
           {5.500, 0.0005, 0.0278, 0.9548},  {6.000, 0.0006, 0.0242, 0.9466},
           {6.500, 0.0002, 0.0206, 0.9486},  {7.000, 0.0004, 0.0127, 0.9572},
           {7.500, 0.0005, 0.0055, 0.9590},  {8.000, 0.0003, 0.0049, 0.9608},
           {8.500, 0.0003, 0.1089, 0.9619},  {9.000, 0.0005, 0.2089, 0.9623},
           {9.500, 0.0006, 0.2911, 0.9637},  {10.000, 0.0006, 0.2345, 0.9638},
           {10.500, 0.0001, 0.1906, 0.9621}, {11.000, 0.0004, 0.1455, 0.9612},
           {11.500, 0.0003, 0.1011, 0.9617}, {12.000, 0.0002, 0.0708, 0.9615},
           {12.500, 0.0008, 0.0697, 0.9618}, {13.000, 0.0003, 0.0803, 0.9623},
           {13.500, 0.0001, 0.0756, 0.9627}, {14.000, 0.0002, 0.0662, 0.9627},
           {14.500, 0.0002, 0.0596, 0.9640}, {15.000, 0.0009, 0.0540, 0.9626},
           {15.500, 0.0009, 0.0494, 0.9642}, {16.000, 0.0003, 0.0440, 0.9649},
           {16.500, 0.0002, 0.0387, 0.9646}, {17.000, 0.0001, 0.0337, 0.9646},
           {17.500, 0.0004, 0.0269, 0.9644}, {18.000, 0.0005, 0.0221, 0.9641},
           {18.500, 0.0007, 0.0281, 0.9645}, {19.000, 0.0005, 0.0733, 0.9654},
           {19.500, 0.0003, 0.1379, 0.9660}, {20.000, 0.0005, 0.1903, 0.9657},
           {20.500, 0.0007, 0.2237, 0.9653}, {21.000, 0.0001, 0.2412, 0.9640},
           {21.500, 0.0005, 0.2451, 0.9647}, {22.000, 0.0006, 0.2385, 0.9659},
           {22.500, 0.0006, 0.2257, 0.9670}, {23.000, 0.0006, 0.2125, 0.9669},
           {23.500, 0.0010, 0.1998, 0.9661}, {24.000, 0.0007, 0.1873, 0.9703},
           {24.500, 0.0008, 0.1762, 0.9648}, {25.000, 0.0008, 0.1693, 0.9678},
           {26.000, 0.0007, 0.1551, 0.9665}, {27.000, 0.0008, 0.1448, 0.9682},
           {28.000, 0.0003, 0.1385, 0.9693}, {29.000, 0.0004, 0.1314, 0.9683},
           {30.000, 0.0010, 0.1275, 0.9700}, {32.000, 0.0004, 0.1229, 0.9701},
           {34.000, 0.0006, 0.1210, 0.9704}, {36.000, 0.0001, 0.1219, 0.9736},
           {38.000, 0.0006, 0.1221, 0.9705}, {40.000, 0.0003, 0.1237, 0.9757}});
    }

protected:
    virtual void SetUp()
    {
        double thickness = 3.18e-3;   // [m]
        auto aMaterial =
          Material::nBandMaterial(loadSampleData_NFRC_1042(), thickness, MaterialType::Coated);
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Full);

        aMaterial->setBandWavelengths(loadSolarRadiationFile().getXArray());

        // make layer
        m_Layer = CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);
        CSeries solarRadiation{loadSolarRadiationFile()};
        m_Layer->setSourceData(solarRadiation);
    }

public:
    std::shared_ptr<CBSDFLayer> getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestSpecularLayer_1042, TestSpecular1)
{
    SCOPED_TRACE("Begin Test: Specular layer - BSDF.");

    std::shared_ptr<CBSDFLayer> aLayer = getLayer();

    auto aResults = aLayer->getResults();

    const double tauDiff = aResults.DiffDiff(Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.38969308651817641, tauDiff, 1e-6);

    const double RfDiff = aResults.DiffDiff(Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.39697550370449242, RfDiff, 1e-6);

    const double theta = 37;
    const double phi = 59;

    const double tauHem = aResults.DirHem(Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.42756321138709952, tauHem, 1e-6);

    const double tauDir = aResults.DirDir(Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.42756321138709952, tauDir, 1e-6);
}
