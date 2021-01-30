#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCESpectralAveraging.hpp"
#include "WCECommon.hpp"


using namespace SingleLayerOptics;
using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestNFRC102ScatteringLayer1_PHOTOPIC : public testing::Test
{
private:
    CScatteringLayer m_Layer;

    std::vector<double> loadWavelengths() const
    {
        std::vector<double> aWavelengths{
          0.380, 0.385, 0.390, 0.395, 0.400, 0.405, 0.410, 0.415, 0.420, 0.425, 0.430, 0.435,
          0.440, 0.445, 0.450, 0.455, 0.460, 0.465, 0.470, 0.475, 0.480, 0.485, 0.490, 0.495,
          0.500, 0.505, 0.510, 0.515, 0.520, 0.525, 0.530, 0.535, 0.540, 0.545, 0.550, 0.555,
          0.560, 0.565, 0.570, 0.575, 0.580, 0.585, 0.590, 0.595, 0.600, 0.605, 0.610, 0.615,
          0.620, 0.625, 0.630, 0.635, 0.640, 0.645, 0.650, 0.655, 0.660, 0.665, 0.670, 0.675,
          0.680, 0.685, 0.690, 0.695, 0.700, 0.705, 0.710, 0.715, 0.720, 0.725, 0.730, 0.735,
          0.740, 0.745, 0.750, 0.755, 0.760, 0.765, 0.770, 0.775, 0.780};

        return aWavelengths;
    }

    CSeries loadDetectorData() const
    {
        return CSeries(
          {{0.380, 0.0000}, {0.385, 0.0001}, {0.390, 0.0001}, {0.395, 0.0002}, {0.400, 0.0004},
           {0.405, 0.0006}, {0.410, 0.0012}, {0.415, 0.0022}, {0.420, 0.0040}, {0.425, 0.0073},
           {0.430, 0.0116}, {0.435, 0.0168}, {0.440, 0.0230}, {0.445, 0.0298}, {0.450, 0.0380},
           {0.455, 0.0480}, {0.460, 0.0600}, {0.465, 0.0739}, {0.470, 0.0910}, {0.475, 0.1126},
           {0.480, 0.1390}, {0.485, 0.1693}, {0.490, 0.2080}, {0.495, 0.2586}, {0.500, 0.3230},
           {0.505, 0.4073}, {0.510, 0.5030}, {0.515, 0.6082}, {0.520, 0.7100}, {0.525, 0.7932},
           {0.530, 0.8620}, {0.535, 0.9149}, {0.540, 0.9540}, {0.545, 0.9803}, {0.550, 0.9950},
           {0.555, 1.0000}, {0.560, 0.9950}, {0.565, 0.9786}, {0.570, 0.9520}, {0.575, 0.9154},
           {0.580, 0.8700}, {0.585, 0.8163}, {0.590, 0.7570}, {0.595, 0.6949}, {0.600, 0.6310},
           {0.605, 0.5668}, {0.610, 0.5030}, {0.615, 0.4412}, {0.620, 0.3810}, {0.625, 0.3210},
           {0.630, 0.2650}, {0.635, 0.2170}, {0.640, 0.1750}, {0.645, 0.1382}, {0.650, 0.1070},
           {0.655, 0.0816}, {0.660, 0.0610}, {0.665, 0.0446}, {0.670, 0.0320}, {0.675, 0.0232},
           {0.680, 0.0170}, {0.685, 0.0119}, {0.690, 0.0082}, {0.695, 0.0057}, {0.700, 0.0041},
           {0.705, 0.0029}, {0.710, 0.0021}, {0.715, 0.0015}, {0.720, 0.0010}, {0.725, 0.0007},
           {0.730, 0.0005}, {0.735, 0.0004}, {0.740, 0.0002}, {0.745, 0.0002}, {0.750, 0.0001},
           {0.755, 0.0001}, {0.760, 0.0001}, {0.765, 0.0000}, {0.770, 0.0000}, {0.775, 0.0000},
           {0.780, 0.0000}});
    }

    CSeries loadSolarRadiationFile()
    {
        return CSeries(
          {{0.3000, 0.0341},   {0.3010, 0.3601},   {0.3020, 0.6862},   {0.3030, 1.0122},
           {0.3040, 1.3383},   {0.3050, 1.6643},   {0.3060, 1.9903},   {0.3070, 2.3164},
           {0.3080, 2.6424},   {0.3090, 2.9685},   {0.3100, 3.2945},   {0.3110, 4.9887},
           {0.3120, 6.6828},   {0.3130, 8.3770},   {0.3140, 10.0711},  {0.3150, 11.7652},
           {0.3160, 13.4594},  {0.3170, 15.1535},  {0.3180, 16.8477},  {0.3190, 18.5418},
           {0.3200, 20.2360},  {0.3210, 21.9177},  {0.3220, 23.5995},  {0.3230, 25.2812},
           {0.3240, 26.9630},  {0.3250, 28.6447},  {0.3260, 30.3265},  {0.3270, 32.0082},
           {0.3280, 33.6900},  {0.3290, 35.3717},  {0.3300, 37.0535},  {0.3310, 37.3430},
           {0.3320, 37.6326},  {0.3330, 37.9221},  {0.3340, 38.2116},  {0.3350, 38.5011},
           {0.3360, 38.7907},  {0.3370, 39.0802},  {0.3380, 39.3697},  {0.3390, 39.6593},
           {0.3400, 39.9488},  {0.3410, 40.4451},  {0.3420, 40.9414},  {0.3430, 41.4377},
           {0.3440, 41.9340},  {0.3450, 42.4302},  {0.3460, 42.9265},  {0.3470, 43.4228},
           {0.3480, 43.9191},  {0.3490, 44.4154},  {0.3500, 44.9117},  {0.3510, 45.0844},
           {0.3520, 45.2570},  {0.3530, 45.4297},  {0.3540, 45.6023},  {0.3550, 45.7750},
           {0.3560, 45.9477},  {0.3570, 46.1203},  {0.3580, 46.2930},  {0.3590, 46.4656},
           {0.3600, 46.6383},  {0.3610, 47.1834},  {0.3620, 47.7285},  {0.3630, 48.2735},
           {0.3640, 48.8186},  {0.3650, 49.3637},  {0.3660, 49.9088},  {0.3670, 50.4539},
           {0.3680, 50.9989},  {0.3690, 51.5440},  {0.3700, 52.0891},  {0.3710, 51.8777},
           {0.3720, 51.6664},  {0.3730, 51.4550},  {0.3740, 51.2437},  {0.3750, 51.0323},
           {0.3760, 50.8209},  {0.3770, 50.6096},  {0.3780, 50.3982},  {0.3790, 50.1869},
           {0.3800, 49.9755},  {0.3810, 50.4428},  {0.3820, 50.9100},  {0.3830, 51.3773},
           {0.3840, 51.8446},  {0.3850, 52.3118},  {0.3860, 52.7791},  {0.3870, 53.2464},
           {0.3880, 53.7137},  {0.3890, 54.1809},  {0.3900, 54.6482},  {0.3910, 57.4589},
           {0.3920, 60.2695},  {0.3930, 63.0802},  {0.3940, 65.8909},  {0.3950, 68.7015},
           {0.3960, 71.5122},  {0.3970, 74.3229},  {0.3980, 77.1336},  {0.3990, 79.9442},
           {0.4000, 82.7549},  {0.4010, 83.6280},  {0.4020, 84.5011},  {0.4030, 85.3742},
           {0.4040, 86.2473},  {0.4050, 87.1204},  {0.4060, 87.9936},  {0.4070, 88.8667},
           {0.4080, 89.7398},  {0.4090, 90.6129},  {0.4100, 91.4860},  {0.4110, 91.6806},
           {0.4120, 91.8752},  {0.4130, 92.0697},  {0.4140, 92.2643},  {0.4150, 92.4589},
           {0.4160, 92.6535},  {0.4170, 92.8481},  {0.4180, 93.0426},  {0.4190, 93.2372},
           {0.4200, 93.4318},  {0.4210, 92.7568},  {0.4220, 92.0819},  {0.4230, 91.4069},
           {0.4240, 90.7320},  {0.4250, 90.0570},  {0.4260, 89.3821},  {0.4270, 88.7071},
           {0.4280, 88.0322},  {0.4290, 87.3572},  {0.4300, 86.6823},  {0.4310, 88.5006},
           {0.4320, 90.3188},  {0.4330, 92.1371},  {0.4340, 93.9554},  {0.4350, 95.7736},
           {0.4360, 97.5919},  {0.4370, 99.4102},  {0.4380, 101.2280}, {0.4390, 103.0470},
           {0.4400, 104.8650}, {0.4410, 106.0790}, {0.4420, 107.2940}, {0.4430, 108.5080},
           {0.4440, 109.7220}, {0.4450, 110.9360}, {0.4460, 112.1510}, {0.4470, 113.3650},
           {0.4480, 114.5790}, {0.4490, 115.7940}, {0.4500, 117.0080}, {0.4510, 117.0880},
           {0.4520, 117.1690}, {0.4530, 117.2490}, {0.4540, 117.3300}, {0.4550, 117.4100},
           {0.4560, 117.4900}, {0.4570, 117.5710}, {0.4580, 117.6510}, {0.4590, 117.7320},
           {0.4600, 117.8120}, {0.4610, 117.5170}, {0.4620, 117.2220}, {0.4630, 116.9270},
           {0.4640, 116.6320}, {0.4650, 116.3360}, {0.4660, 116.0410}, {0.4670, 115.7460},
           {0.4680, 115.4510}, {0.4690, 115.1560}, {0.4700, 114.8610}, {0.4710, 114.9670},
           {0.4720, 115.0730}, {0.4730, 115.1800}, {0.4740, 115.2860}, {0.4750, 115.3920},
           {0.4760, 115.4980}, {0.4770, 115.6040}, {0.4780, 115.7110}, {0.4790, 115.8170},
           {0.4800, 115.9230}, {0.4810, 115.2120}, {0.4820, 114.5010}, {0.4830, 113.7890},
           {0.4840, 113.0780}, {0.4850, 112.3670}, {0.4860, 111.6560}, {0.4870, 110.9450},
           {0.4880, 110.2330}, {0.4890, 109.5220}, {0.4900, 108.8110}, {0.4910, 108.8650},
           {0.4920, 108.9200}, {0.4930, 108.9740}, {0.4940, 109.0280}, {0.4950, 109.0820},
           {0.4960, 109.1370}, {0.4970, 109.1910}, {0.4980, 109.2450}, {0.4990, 109.3000},
           {0.5000, 109.3540}, {0.5010, 109.1990}, {0.5020, 109.0440}, {0.5030, 108.8880},
           {0.5040, 108.7330}, {0.5050, 108.5780}, {0.5060, 108.4230}, {0.5070, 108.2680},
           {0.5080, 108.1120}, {0.5090, 107.9570}, {0.5100, 107.8020}, {0.5110, 107.5010},
           {0.5120, 107.2000}, {0.5130, 106.8980}, {0.5140, 106.5970}, {0.5150, 106.2960},
           {0.5160, 105.9950}, {0.5170, 105.6940}, {0.5180, 105.3920}, {0.5190, 105.0910},
           {0.5200, 104.7900}, {0.5210, 105.0800}, {0.5220, 105.3700}, {0.5230, 105.6600},
           {0.5240, 105.9500}, {0.5250, 106.2390}, {0.5260, 106.5290}, {0.5270, 106.8190},
           {0.5280, 107.1090}, {0.5290, 107.3990}, {0.5300, 107.6890}, {0.5310, 107.3610},
           {0.5320, 107.0320}, {0.5330, 106.7040}, {0.5340, 106.3750}, {0.5350, 106.0470},
           {0.5360, 105.7190}, {0.5370, 105.3900}, {0.5380, 105.0620}, {0.5390, 104.7330},
           {0.5400, 104.4050}, {0.5410, 104.3690}, {0.5420, 104.3330}, {0.5430, 104.2970},
           {0.5440, 104.2610}, {0.5450, 104.2250}, {0.5460, 104.1900}, {0.5470, 104.1540},
           {0.5480, 104.1180}, {0.5490, 104.0820}, {0.5500, 104.0460}, {0.5510, 103.6410},
           {0.5520, 103.2370}, {0.5530, 102.8320}, {0.5540, 102.4280}, {0.5550, 102.0230},
           {0.5560, 101.6180}, {0.5570, 101.2140}, {0.5580, 100.8090}, {0.5590, 100.4050},
           {0.5600, 100.0000}, {0.5610, 99.6334},  {0.5620, 99.2668},  {0.5630, 98.9003},
           {0.5640, 98.5337},  {0.5650, 98.1671},  {0.5660, 97.8005},  {0.5670, 97.4339},
           {0.5680, 97.0674},  {0.5690, 96.7008},  {0.5700, 96.3342},  {0.5710, 96.2796},
           {0.5720, 96.2250},  {0.5730, 96.1703},  {0.5740, 96.1157},  {0.5750, 96.0611},
           {0.5760, 96.0065},  {0.5770, 95.9519},  {0.5780, 95.8972},  {0.5790, 95.8426},
           {0.5800, 95.7880},  {0.5810, 95.0778},  {0.5820, 94.3675},  {0.5830, 93.6573},
           {0.5840, 92.9470},  {0.5850, 92.2368},  {0.5860, 91.5266},  {0.5870, 90.8163},
           {0.5880, 90.1061},  {0.5890, 89.3958},  {0.5900, 88.6856},  {0.5910, 88.8177},
           {0.5920, 88.9497},  {0.5930, 89.0818},  {0.5940, 89.2138},  {0.5950, 89.3459},
           {0.5960, 89.4780},  {0.5970, 89.6100},  {0.5980, 89.7421},  {0.5990, 89.8741},
           {0.6000, 90.0062},  {0.6010, 89.9655},  {0.6020, 89.9248},  {0.6030, 89.8841},
           {0.6040, 89.8434},  {0.6050, 89.8026},  {0.6060, 89.7619},  {0.6070, 89.7212},
           {0.6080, 89.6805},  {0.6090, 89.6398},  {0.6100, 89.5991},  {0.6110, 89.4091},
           {0.6120, 89.2190},  {0.6130, 89.0290},  {0.6140, 88.8389},  {0.6150, 88.6489},
           {0.6160, 88.4589},  {0.6170, 88.2688},  {0.6180, 88.0788},  {0.6190, 87.8887},
           {0.6200, 87.6987},  {0.6210, 87.2577},  {0.6220, 86.8167},  {0.6230, 86.3757},
           {0.6240, 85.9347},  {0.6250, 85.4936},  {0.6260, 85.0526},  {0.6270, 84.6116},
           {0.6280, 84.1706},  {0.6290, 83.7296},  {0.6300, 83.2886},  {0.6310, 83.3297},
           {0.6320, 83.3707},  {0.6330, 83.4118},  {0.6340, 83.4528},  {0.6350, 83.4939},
           {0.6360, 83.5350},  {0.6370, 83.5760},  {0.6380, 83.6171},  {0.6390, 83.6581},
           {0.6400, 83.6992},  {0.6410, 83.3320},  {0.6420, 82.9647},  {0.6430, 82.5975},
           {0.6440, 82.2302},  {0.6450, 81.8630},  {0.6460, 81.4958},  {0.6470, 81.1285},
           {0.6480, 80.7613},  {0.6490, 80.3940},  {0.6500, 80.0268},  {0.6510, 80.0456},
           {0.6520, 80.0644},  {0.6530, 80.0831},  {0.6540, 80.1019},  {0.6550, 80.1207},
           {0.6560, 80.1395},  {0.6570, 80.1583},  {0.6580, 80.1770},  {0.6590, 80.1958},
           {0.6600, 80.2146},  {0.6610, 80.4209},  {0.6620, 80.6272},  {0.6630, 80.8336},
           {0.6640, 81.0399},  {0.6650, 81.2462},  {0.6660, 81.4525},  {0.6670, 81.6588},
           {0.6680, 81.8652},  {0.6690, 82.0715},  {0.6700, 82.2778},  {0.6710, 81.8784},
           {0.6720, 81.4791},  {0.6730, 81.0797},  {0.6740, 80.6804},  {0.6750, 80.2810},
           {0.6760, 79.8816},  {0.6770, 79.4823},  {0.6780, 79.0829},  {0.6790, 78.6836},
           {0.6800, 78.2842},  {0.6810, 77.4279},  {0.6820, 76.5716},  {0.6830, 75.7153},
           {0.6840, 74.8590},  {0.6850, 74.0027},  {0.6860, 73.1465},  {0.6870, 72.2902},
           {0.6880, 71.4339},  {0.6890, 70.5776},  {0.6900, 69.7213},  {0.6910, 69.9101},
           {0.6920, 70.0989},  {0.6930, 70.2876},  {0.6940, 70.4764},  {0.6950, 70.6652},
           {0.6960, 70.8540},  {0.6970, 71.0428},  {0.6980, 71.2315},  {0.6990, 71.4203},
           {0.7000, 71.6091},  {0.7010, 71.8831},  {0.7020, 72.1571},  {0.7030, 72.4311},
           {0.7040, 72.7051},  {0.7050, 72.9790},  {0.7060, 73.2530},  {0.7070, 73.5270},
           {0.7080, 73.8010},  {0.7090, 74.0750},  {0.7100, 74.3490},  {0.7110, 73.0745},
           {0.7120, 71.8000},  {0.7130, 70.5255},  {0.7140, 69.2510},  {0.7150, 67.9765},
           {0.7160, 66.7020},  {0.7170, 65.4275},  {0.7180, 64.1530},  {0.7190, 62.8785},
           {0.7200, 61.6040},  {0.7210, 62.4322},  {0.7220, 63.2603},  {0.7230, 64.0885},
           {0.7240, 64.9166},  {0.7250, 65.7448},  {0.7260, 66.5730},  {0.7270, 67.4011},
           {0.7280, 68.2293},  {0.7290, 69.0574},  {0.7300, 69.8856},  {0.7310, 70.4057},
           {0.7320, 70.9259},  {0.7330, 71.4460},  {0.7340, 71.9662},  {0.7350, 72.4863},
           {0.7360, 73.0064},  {0.7370, 73.5266},  {0.7380, 74.0467},  {0.7390, 74.5669},
           {0.7400, 75.0870},  {0.7410, 73.9376},  {0.7420, 72.7881},  {0.7430, 71.6387},
           {0.7440, 70.4893},  {0.7450, 69.3398},  {0.7460, 68.1904},  {0.7470, 67.0410},
           {0.7480, 65.8916},  {0.7490, 64.7421},  {0.7500, 63.5927},  {0.7510, 61.8752},
           {0.7520, 60.1578},  {0.7530, 58.4403},  {0.7540, 56.7229},  {0.7550, 55.0054},
           {0.7560, 53.2880},  {0.7570, 51.5705},  {0.7580, 49.8531},  {0.7590, 48.1356},
           {0.7600, 46.4182},  {0.7610, 48.4569},  {0.7620, 50.4956},  {0.7630, 52.5344},
           {0.7640, 54.5731},  {0.7650, 56.6118},  {0.7660, 58.6505},  {0.7670, 60.6892},
           {0.7680, 62.7280},  {0.7690, 64.7667},  {0.7700, 66.8054},  {0.7710, 66.4631},
           {0.7720, 66.1209},  {0.7730, 65.7786},  {0.7740, 65.4364},  {0.7750, 65.0941},
           {0.7760, 64.7518},  {0.7770, 64.4096},  {0.7780, 64.0673},  {0.7790, 63.7251},
           {0.7800, 63.3828},  {0.7810, 63.4749},  {0.7820, 63.5670},  {0.7830, 63.6592},
           {0.7840, 63.7513},  {0.7850, 63.8434},  {0.7860, 63.9355},  {0.7870, 64.0276},
           {0.7880, 64.1198},  {0.7890, 64.2119},  {0.7900, 64.3040},  {0.7910, 63.8188},
           {0.7920, 63.3336},  {0.7930, 62.8484},  {0.7940, 62.3632},  {0.7950, 61.8779},
           {0.7960, 61.3927},  {0.7970, 60.9075},  {0.7980, 60.4223},  {0.7990, 59.9371},
           {0.8000, 59.4519},  {0.8010, 58.7026},  {0.8020, 57.9533},  {0.8030, 57.2040},
           {0.8040, 56.4547},  {0.8050, 55.7054},  {0.8060, 54.9562},  {0.8070, 54.2069},
           {0.8080, 53.4576},  {0.8090, 52.7083},  {0.8100, 51.9590},  {0.8110, 52.5072},
           {0.8120, 53.0553},  {0.8130, 53.6035},  {0.8140, 54.1516},  {0.8150, 54.6998},
           {0.8160, 55.2480},  {0.8170, 55.7961},  {0.8180, 56.3443},  {0.8190, 56.8924},
           {0.8200, 57.4406},  {0.8210, 57.7278},  {0.8220, 58.0150},  {0.8230, 58.3022},
           {0.8240, 58.5894},  {0.8250, 58.8765},  {0.8260, 59.1637},  {0.8270, 59.4509},
           {0.8280, 59.7381},  {0.8290, 60.0253},  {0.8300, 60.3125}});
    }

    std::shared_ptr<CSpectralSampleData> loadSampleData_NFRC_102()
    {
        return CSpectralSampleData::create(
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
    }

protected:
    void SetUp() override
    {
        const auto thickness = 3.048e-3;   // [m]
        const auto aMaterial = Material::nBandMaterial(loadSampleData_NFRC_102(),
                                                       loadDetectorData(),
                                                       thickness,
                                                       MaterialType::Monolithic,
                                                       WavelengthRange::Visible);

        m_Layer = CScatteringLayer::createSpecularLayer(aMaterial);
        CSeries solarRadiation{loadSolarRadiationFile()};
        m_Layer.setSourceData(solarRadiation);
        m_Layer.setWavelengths(loadWavelengths());
    }

public:
    CScatteringLayer & getLayer()
    {
        return m_Layer;
    };
};

TEST_F(TestNFRC102ScatteringLayer1_PHOTOPIC, Test1)
{
    SCOPED_TRACE("Begin Test: NFRC 102 scattering layer - 0 deg incident.");

    const double minLambda = 0.38;
    const double maxLambda = 0.78;

    auto aLayer = getLayer();

    Side aSide = Side::Front;

    double T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDirect);
    EXPECT_NEAR(0.899260, T_dir_dir, 1e-6);

    double R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDirect);
    EXPECT_NEAR(0.082563, R_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDiffuse);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDiffuse);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.821306, T_dif_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.158094, R_dif_dif, 1e-6);

    double A_dir = aLayer.getAbsorptance(aSide, ScatteringSimple::Direct);
    EXPECT_NEAR(0.018177, A_dir, 1e-6);

    double A_dif = aLayer.getAbsorptance(aSide, ScatteringSimple::Diffuse);
    EXPECT_NEAR(0.020599, A_dif, 1e-6);
}