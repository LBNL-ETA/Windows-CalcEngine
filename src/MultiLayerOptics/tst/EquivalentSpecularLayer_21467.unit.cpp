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

class EquivalentSpecularLayer_21467 : public testing::Test
{
private:
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

	CSeries loadSolarRadiationFile() const
    {
        // Full ASTM E891-87 Table 1 (Solar radiation)
        auto aSolarRadiation = CSeries(
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

    std::shared_ptr<CSpectralSampleData> loadSampleData_NFRC_21467() const
    {
        auto aMeasurement = CSpectralSampleData::create(
          {{0.3, 0.0, 0.073, 0.05},      {0.305, 0.0, 0.073, 0.048},   {0.31, 0.001, 0.072, 0.048},
           {0.315, 0.06, 0.075, 0.042},  {0.32, 0.202, 0.078, 0.042},  {0.325, 0.263, 0.085, 0.046},
           {0.33, 0.346, 0.104, 0.233},  {0.335, 0.435, 0.129, 0.296}, {0.34, 0.513, 0.162, 0.321},
           {0.345, 0.58, 0.189, 0.383},  {0.35, 0.635, 0.21, 0.324},   {0.355, 0.679, 0.225, 0.284},
           {0.36, 0.714, 0.229, 0.257},  {0.365, 0.741, 0.23, 0.24},   {0.37, 0.756, 0.223, 0.227},
           {0.375, 0.768, 0.214, 0.215}, {0.38, 0.776, 0.204, 0.208},  {0.385, 0.786, 0.198, 0.2},
           {0.39, 0.797, 0.195, 0.191},  {0.395, 0.807, 0.19, 0.185},  {0.4, 0.815, 0.185, 0.179},
           {0.405, 0.82, 0.18, 0.172},   {0.41, 0.824, 0.176, 0.167},  {0.415, 0.828, 0.172, 0.164},
           {0.42, 0.831, 0.167, 0.16},   {0.425, 0.834, 0.164, 0.16},  {0.43, 0.836, 0.161, 0.156},
           {0.435, 0.839, 0.157, 0.155}, {0.44, 0.841, 0.153, 0.152},  {0.445, 0.844, 0.149, 0.15},
           {0.45, 0.847, 0.146, 0.149},  {0.455, 0.851, 0.144, 0.146}, {0.46, 0.854, 0.141, 0.143},
           {0.465, 0.858, 0.139, 0.142}, {0.47, 0.861, 0.137, 0.139},  {0.475, 0.862, 0.134, 0.138},
           {0.48, 0.864, 0.131, 0.136},  {0.485, 0.865, 0.129, 0.135}, {0.49, 0.868, 0.127, 0.132},
           {0.495, 0.87, 0.128, 0.13},   {0.5, 0.872, 0.127, 0.128},   {0.505, 0.873, 0.126, 0.126},
           {0.51, 0.875, 0.125, 0.125},  {0.515, 0.875, 0.125, 0.123}, {0.52, 0.876, 0.124, 0.121},
           {0.525, 0.877, 0.123, 0.12},  {0.53, 0.878, 0.122, 0.118},  {0.535, 0.879, 0.12, 0.118},
           {0.54, 0.88, 0.118, 0.117},   {0.545, 0.88, 0.116, 0.116},  {0.55, 0.88, 0.114, 0.115},
           {0.555, 0.88, 0.112, 0.114},  {0.56, 0.881, 0.11, 0.113},   {0.565, 0.88, 0.109, 0.113},
           {0.57, 0.88, 0.108, 0.112},   {0.575, 0.878, 0.107, 0.112}, {0.58, 0.877, 0.107, 0.111},
           {0.585, 0.877, 0.107, 0.11},  {0.59, 0.876, 0.104, 0.109},  {0.595, 0.876, 0.103, 0.108},
           {0.6, 0.876, 0.104, 0.107},   {0.605, 0.875, 0.102, 0.107}, {0.61, 0.875, 0.102, 0.106},
           {0.615, 0.876, 0.103, 0.106}, {0.62, 0.877, 0.103, 0.103},  {0.625, 0.878, 0.104, 0.103},
           {0.63, 0.88, 0.105, 0.103},   {0.635, 0.881, 0.106, 0.102}, {0.64, 0.884, 0.108, 0.101},
           {0.645, 0.885, 0.109, 0.101}, {0.65, 0.887, 0.111, 0.1},    {0.655, 0.887, 0.113, 0.1},
           {0.66, 0.887, 0.113, 0.098},  {0.665, 0.887, 0.113, 0.098}, {0.67, 0.886, 0.114, 0.1},
           {0.675, 0.885, 0.115, 0.097}, {0.68, 0.885, 0.115, 0.097},  {0.685, 0.884, 0.116, 0.096},
           {0.69, 0.884, 0.116, 0.095},  {0.695, 0.884, 0.116, 0.096}, {0.7, 0.883, 0.117, 0.096},
           {0.705, 0.883, 0.117, 0.096}, {0.71, 0.883, 0.117, 0.096},  {0.715, 0.882, 0.118, 0.096},
           {0.72, 0.883, 0.117, 0.095},  {0.725, 0.883, 0.117, 0.097}, {0.73, 0.883, 0.117, 0.095},
           {0.735, 0.883, 0.117, 0.095}, {0.74, 0.883, 0.116, 0.093},  {0.745, 0.883, 0.117, 0.095},
           {0.75, 0.881, 0.116, 0.096},  {0.755, 0.881, 0.114, 0.096}, {0.76, 0.881, 0.115, 0.097},
           {0.765, 0.881, 0.113, 0.098}, {0.77, 0.88, 0.113, 0.097},   {0.775, 0.877, 0.112, 0.097},
           {0.78, 0.876, 0.112, 0.098},  {0.785, 0.875, 0.111, 0.098}, {0.79, 0.873, 0.11, 0.1},
           {0.795, 0.874, 0.107, 0.1},   {0.8, 0.872, 0.107, 0.1},     {0.805, 0.871, 0.107, 0.101},
           {0.81, 0.87, 0.106, 0.101},   {0.815, 0.872, 0.108, 0.101}, {0.82, 0.87, 0.107, 0.104},
           {0.825, 0.867, 0.104, 0.105}, {0.83, 0.866, 0.104, 0.103},  {0.835, 0.866, 0.104, 0.107},
           {0.84, 0.865, 0.1, 0.106},    {0.845, 0.867, 0.099, 0.106}, {0.85, 0.861, 0.093, 0.108},
           {0.855, 0.864, 0.099, 0.105}, {0.86, 0.865, 0.097, 0.11},   {0.865, 0.867, 0.101, 0.109},
           {0.87, 0.866, 0.098, 0.106},  {0.875, 0.864, 0.099, 0.102}, {0.88, 0.862, 0.096, 0.112},
           {0.885, 0.859, 0.098, 0.117}, {0.89, 0.859, 0.096, 0.111},  {0.895, 0.859, 0.096, 0.117},
           {0.9, 0.861, 0.098, 0.112},   {0.905, 0.858, 0.096, 0.114}, {0.91, 0.855, 0.096, 0.11},
           {0.915, 0.855, 0.095, 0.11},  {0.92, 0.854, 0.093, 0.108},  {0.925, 0.852, 0.093, 0.11},
           {0.93, 0.851, 0.092, 0.112},  {0.935, 0.851, 0.092, 0.11},  {0.94, 0.85, 0.091, 0.108},
           {0.945, 0.848, 0.09, 0.109},  {0.95, 0.849, 0.09, 0.108},   {0.955, 0.848, 0.091, 0.11},
           {0.96, 0.848, 0.091, 0.109},  {0.965, 0.848, 0.091, 0.11},  {0.97, 0.846, 0.09, 0.109},
           {0.975, 0.846, 0.09, 0.108},  {0.98, 0.846, 0.091, 0.108},  {0.985, 0.846, 0.092, 0.108},
           {0.99, 0.845, 0.091, 0.107},  {0.995, 0.845, 0.091, 0.107}, {1.0, 0.845, 0.091, 0.108},
           {1.005, 0.846, 0.091, 0.108}, {1.01, 0.846, 0.09, 0.108},   {1.015, 0.846, 0.091, 0.108},
           {1.02, 0.845, 0.09, 0.109},   {1.025, 0.845, 0.09, 0.109},  {1.03, 0.844, 0.089, 0.109},
           {1.035, 0.844, 0.088, 0.11},  {1.04, 0.844, 0.088, 0.109},  {1.045, 0.844, 0.089, 0.109},
           {1.05, 0.844, 0.087, 0.109},  {1.055, 0.844, 0.087, 0.109}, {1.06, 0.845, 0.088, 0.11},
           {1.065, 0.844, 0.089, 0.11},  {1.07, 0.843, 0.088, 0.109},  {1.075, 0.844, 0.088, 0.109},
           {1.08, 0.844, 0.089, 0.108},  {1.085, 0.843, 0.089, 0.109}, {1.09, 0.844, 0.089, 0.109},
           {1.095, 0.844, 0.089, 0.109}, {1.1, 0.844, 0.09, 0.109},    {1.105, 0.843, 0.09, 0.109},
           {1.11, 0.844, 0.09, 0.107},   {1.115, 0.843, 0.089, 0.106}, {1.12, 0.844, 0.09, 0.106},
           {1.125, 0.844, 0.09, 0.107},  {1.13, 0.844, 0.091, 0.106},  {1.135, 0.844, 0.09, 0.105},
           {1.14, 0.844, 0.091, 0.104},  {1.145, 0.844, 0.09, 0.103},  {1.15, 0.843, 0.089, 0.104},
           {1.155, 0.843, 0.089, 0.103}, {1.16, 0.844, 0.089, 0.104},  {1.165, 0.844, 0.091, 0.103},
           {1.17, 0.843, 0.09, 0.104},   {1.175, 0.844, 0.09, 0.103},  {1.18, 0.845, 0.089, 0.104},
           {1.185, 0.845, 0.089, 0.104}, {1.19, 0.845, 0.09, 0.103},   {1.195, 0.845, 0.089, 0.103},
           {1.2, 0.846, 0.089, 0.103},   {1.205, 0.846, 0.09, 0.104},  {1.21, 0.846, 0.09, 0.104},
           {1.215, 0.847, 0.091, 0.105}, {1.22, 0.847, 0.091, 0.105},  {1.225, 0.847, 0.091, 0.106},
           {1.23, 0.847, 0.091, 0.106},  {1.235, 0.847, 0.09, 0.106},  {1.24, 0.847, 0.09, 0.107},
           {1.245, 0.847, 0.09, 0.108},  {1.25, 0.847, 0.09, 0.107},   {1.255, 0.847, 0.09, 0.107},
           {1.26, 0.848, 0.09, 0.107},   {1.265, 0.849, 0.09, 0.107},  {1.27, 0.849, 0.09, 0.107},
           {1.275, 0.849, 0.091, 0.107}, {1.28, 0.85, 0.09, 0.107},    {1.285, 0.85, 0.09, 0.106},
           {1.29, 0.851, 0.09, 0.106},   {1.295, 0.85, 0.089, 0.107},  {1.3, 0.851, 0.089, 0.107},
           {1.305, 0.851, 0.089, 0.107}, {1.31, 0.851, 0.088, 0.107},  {1.315, 0.851, 0.088, 0.106},
           {1.32, 0.852, 0.088, 0.107},  {1.325, 0.852, 0.087, 0.107}, {1.33, 0.851, 0.086, 0.105},
           {1.335, 0.851, 0.086, 0.106}, {1.34, 0.851, 0.086, 0.105},  {1.345, 0.852, 0.086, 0.105},
           {1.35, 0.852, 0.086, 0.105},  {1.355, 0.852, 0.086, 0.106}, {1.36, 0.852, 0.085, 0.105},
           {1.365, 0.852, 0.086, 0.105}, {1.37, 0.853, 0.087, 0.106},  {1.375, 0.853, 0.087, 0.106},
           {1.38, 0.853, 0.087, 0.106},  {1.385, 0.853, 0.087, 0.106}, {1.39, 0.853, 0.087, 0.106},
           {1.395, 0.852, 0.087, 0.106}, {1.4, 0.852, 0.087, 0.107},   {1.405, 0.853, 0.087, 0.107},
           {1.41, 0.853, 0.088, 0.108},  {1.415, 0.853, 0.088, 0.108}, {1.42, 0.853, 0.088, 0.109},
           {1.425, 0.853, 0.088, 0.11},  {1.43, 0.853, 0.089, 0.111},  {1.435, 0.853, 0.089, 0.11},
           {1.44, 0.854, 0.09, 0.11},    {1.445, 0.854, 0.091, 0.112}, {1.45, 0.855, 0.091, 0.112},
           {1.455, 0.855, 0.091, 0.111}, {1.46, 0.855, 0.091, 0.111},  {1.465, 0.855, 0.09, 0.112},
           {1.47, 0.855, 0.09, 0.111},   {1.475, 0.855, 0.09, 0.111},  {1.48, 0.854, 0.09, 0.112},
           {1.485, 0.854, 0.089, 0.111}, {1.49, 0.854, 0.089, 0.111},  {1.495, 0.854, 0.089, 0.11},
           {1.5, 0.854, 0.089, 0.109},   {1.505, 0.854, 0.089, 0.109}, {1.51, 0.855, 0.09, 0.11},
           {1.515, 0.855, 0.09, 0.11},   {1.52, 0.855, 0.089, 0.111},  {1.525, 0.854, 0.09, 0.111},
           {1.53, 0.854, 0.09, 0.11},    {1.535, 0.854, 0.09, 0.11},   {1.54, 0.854, 0.09, 0.11},
           {1.545, 0.854, 0.09, 0.11},   {1.55, 0.854, 0.09, 0.11},    {1.555, 0.854, 0.09, 0.11},
           {1.56, 0.855, 0.089, 0.11},   {1.565, 0.855, 0.089, 0.109}, {1.57, 0.855, 0.088, 0.11},
           {1.575, 0.855, 0.088, 0.11},  {1.58, 0.856, 0.088, 0.109},  {1.585, 0.856, 0.087, 0.109},
           {1.59, 0.856, 0.086, 0.108},  {1.595, 0.856, 0.086, 0.108}, {1.6, 0.857, 0.085, 0.108},
           {1.605, 0.857, 0.086, 0.109}, {1.61, 0.856, 0.086, 0.109},  {1.615, 0.857, 0.085, 0.109},
           {1.62, 0.855, 0.086, 0.109},  {1.625, 0.854, 0.086, 0.109}, {1.63, 0.855, 0.087, 0.11},
           {1.635, 0.854, 0.087, 0.11},  {1.64, 0.854, 0.087, 0.11},   {1.645, 0.853, 0.087, 0.109},
           {1.65, 0.853, 0.087, 0.109},  {1.655, 0.853, 0.087, 0.111}, {1.66, 0.853, 0.087, 0.111},
           {1.665, 0.853, 0.087, 0.111}, {1.67, 0.853, 0.087, 0.111},  {1.675, 0.852, 0.087, 0.112},
           {1.68, 0.852, 0.087, 0.112},  {1.685, 0.852, 0.087, 0.112}, {1.69, 0.852, 0.086, 0.112},
           {1.695, 0.853, 0.086, 0.113}, {1.7, 0.852, 0.086, 0.112},   {1.705, 0.853, 0.087, 0.112},
           {1.71, 0.853, 0.087, 0.111},  {1.715, 0.853, 0.086, 0.11},  {1.72, 0.854, 0.086, 0.111},
           {1.725, 0.855, 0.086, 0.112}, {1.73, 0.855, 0.087, 0.111},  {1.735, 0.856, 0.087, 0.11},
           {1.74, 0.856, 0.087, 0.111},  {1.745, 0.856, 0.087, 0.11},  {1.75, 0.857, 0.086, 0.11},
           {1.755, 0.856, 0.086, 0.11},  {1.76, 0.856, 0.086, 0.11},   {1.765, 0.855, 0.086, 0.11},
           {1.77, 0.855, 0.086, 0.11},   {1.775, 0.855, 0.087, 0.109}, {1.78, 0.854, 0.086, 0.11},
           {1.785, 0.853, 0.086, 0.109}, {1.79, 0.852, 0.085, 0.109},  {1.795, 0.852, 0.084, 0.11},
           {1.8, 0.852, 0.084, 0.11},    {1.805, 0.853, 0.084, 0.11},  {1.81, 0.852, 0.084, 0.11},
           {1.815, 0.852, 0.085, 0.11},  {1.82, 0.853, 0.086, 0.11},   {1.825, 0.852, 0.085, 0.109},
           {1.83, 0.852, 0.085, 0.108},  {1.835, 0.853, 0.084, 0.108}, {1.84, 0.853, 0.084, 0.107},
           {1.845, 0.852, 0.084, 0.105}, {1.85, 0.853, 0.085, 0.106},  {1.855, 0.853, 0.086, 0.107},
           {1.86, 0.852, 0.085, 0.109},  {1.865, 0.851, 0.085, 0.106}, {1.87, 0.851, 0.087, 0.107},
           {1.875, 0.851, 0.087, 0.108}, {1.88, 0.851, 0.085, 0.109},  {1.885, 0.851, 0.085, 0.107},
           {1.89, 0.852, 0.085, 0.106},  {1.895, 0.849, 0.083, 0.108}, {1.9, 0.851, 0.085, 0.108},
           {1.905, 0.853, 0.087, 0.111}, {1.91, 0.851, 0.087, 0.108},  {1.915, 0.851, 0.087, 0.107},
           {1.92, 0.85, 0.085, 0.109},   {1.925, 0.851, 0.085, 0.109}, {1.93, 0.848, 0.087, 0.109},
           {1.935, 0.849, 0.088, 0.108}, {1.94, 0.849, 0.088, 0.108},  {1.945, 0.85, 0.089, 0.108},
           {1.95, 0.85, 0.09, 0.108},    {1.955, 0.851, 0.089, 0.109}, {1.96, 0.85, 0.088, 0.11},
           {1.965, 0.851, 0.09, 0.108},  {1.97, 0.85, 0.09, 0.107},    {1.975, 0.85, 0.089, 0.107},
           {1.98, 0.85, 0.089, 0.108},   {1.985, 0.851, 0.089, 0.109}, {1.99, 0.851, 0.089, 0.107},
           {1.995, 0.852, 0.089, 0.108}, {2.0, 0.852, 0.09, 0.107},    {2.005, 0.851, 0.088, 0.107},
           {2.01, 0.852, 0.088, 0.107},  {2.015, 0.851, 0.086, 0.108}, {2.02, 0.852, 0.085, 0.107},
           {2.025, 0.85, 0.087, 0.105},  {2.03, 0.85, 0.085, 0.108},   {2.035, 0.85, 0.084, 0.112},
           {2.04, 0.851, 0.084, 0.109},  {2.045, 0.851, 0.087, 0.106}, {2.05, 0.849, 0.084, 0.109},
           {2.055, 0.851, 0.084, 0.108}, {2.06, 0.85, 0.085, 0.107},   {2.065, 0.846, 0.084, 0.111},
           {2.07, 0.847, 0.082, 0.109},  {2.075, 0.846, 0.082, 0.113}, {2.08, 0.845, 0.083, 0.11},
           {2.085, 0.848, 0.087, 0.107}, {2.09, 0.845, 0.084, 0.101},  {2.095, 0.843, 0.085, 0.107},
           {2.1, 0.845, 0.081, 0.102},   {2.105, 0.844, 0.085, 0.102}, {2.11, 0.843, 0.083, 0.106},
           {2.115, 0.848, 0.083, 0.107}, {2.12, 0.846, 0.082, 0.108},  {2.125, 0.844, 0.085, 0.104},
           {2.13, 0.842, 0.083, 0.103},  {2.135, 0.832, 0.079, 0.106}, {2.14, 0.833, 0.082, 0.103},
           {2.145, 0.837, 0.085, 0.108}, {2.15, 0.835, 0.083, 0.109},  {2.155, 0.832, 0.082, 0.11},
           {2.16, 0.833, 0.084, 0.11},   {2.165, 0.832, 0.081, 0.104}, {2.17, 0.833, 0.084, 0.115},
           {2.175, 0.832, 0.083, 0.106}, {2.18, 0.831, 0.082, 0.105},  {2.185, 0.837, 0.083, 0.114},
           {2.19, 0.827, 0.083, 0.112},  {2.195, 0.825, 0.088, 0.104}, {2.2, 0.821, 0.087, 0.112},
           {2.205, 0.824, 0.084, 0.109}, {2.21, 0.825, 0.082, 0.107},  {2.215, 0.828, 0.084, 0.101},
           {2.22, 0.822, 0.08, 0.103},   {2.225, 0.819, 0.076, 0.103}, {2.23, 0.825, 0.084, 0.094},
           {2.235, 0.821, 0.085, 0.106}, {2.24, 0.824, 0.081, 0.103},  {2.245, 0.823, 0.086, 0.113},
           {2.25, 0.828, 0.087, 0.108},  {2.255, 0.819, 0.084, 0.109}, {2.26, 0.823, 0.084, 0.105},
           {2.265, 0.816, 0.081, 0.106}, {2.27, 0.816, 0.076, 0.103},  {2.275, 0.816, 0.074, 0.113},
           {2.28, 0.824, 0.085, 0.1},    {2.285, 0.826, 0.085, 0.1},   {2.29, 0.83, 0.079, 0.109},
           {2.295, 0.831, 0.081, 0.105}, {2.3, 0.826, 0.078, 0.094},   {2.305, 0.833, 0.087, 0.096},
           {2.31, 0.83, 0.09, 0.117},    {2.315, 0.824, 0.08, 0.112},  {2.32, 0.826, 0.08, 0.1},
           {2.325, 0.821, 0.08, 0.109},  {2.33, 0.826, 0.092, 0.096},  {2.335, 0.823, 0.094, 0.115},
           {2.34, 0.824, 0.083, 0.113},  {2.345, 0.832, 0.087, 0.111}, {2.35, 0.822, 0.08, 0.111},
           {2.355, 0.817, 0.08, 0.107},  {2.36, 0.822, 0.084, 0.11},   {2.365, 0.825, 0.077, 0.093},
           {2.37, 0.824, 0.075, 0.11},   {2.375, 0.811, 0.075, 0.106}, {2.38, 0.83, 0.087, 0.105},
           {2.385, 0.829, 0.084, 0.108}, {2.39, 0.807, 0.083, 0.109},  {2.395, 0.825, 0.083, 0.104},
           {2.4, 0.813, 0.075, 0.096},   {2.405, 0.812, 0.084, 0.081}, {2.41, 0.806, 0.083, 0.089},
           {2.415, 0.82, 0.067, 0.098},  {2.42, 0.821, 0.093, 0.123},  {2.425, 0.815, 0.084, 0.12},
           {2.43, 0.821, 0.096, 0.09},   {2.435, 0.826, 0.101, 0.099}, {2.44, 0.819, 0.068, 0.096},
           {2.445, 0.794, 0.071, 0.125}, {2.45, 0.818, 0.081, 0.104},  {2.455, 0.809, 0.077, 0.076},
           {2.46, 0.795, 0.072, 0.093},  {2.465, 0.792, 0.063, 0.106}, {2.47, 0.801, 0.076, 0.11},
           {2.475, 0.816, 0.086, 0.118}, {2.48, 0.809, 0.07, 0.099},   {2.485, 0.783, 0.057, 0.112},
           {2.49, 0.801, 0.074, 0.121},  {2.495, 0.823, 0.079, 0.077}, {2.5, 0.781, 0.058, 0.1},
           {5.0, 0.0, 0.032, 0.032},     {6.0, 0.0, 0.022, 0.022},     {7.0, 0.0, 0.011, 0.011},
           {8.0, 0.0, 0.011, 0.011},     {9.0, 0.0, 0.207, 0.207},     {10.0, 0.0, 0.24, 0.24},
           {11.0, 0.0, 0.153, 0.153},    {12.0, 0.0, 0.077, 0.077},    {13.0, 0.0, 0.077, 0.077},
           {14.0, 0.0, 0.066, 0.066},    {15.0, 0.0, 0.055, 0.053},    {16.0, 0.0, 0.043, 0.043},
           {17.0, 0.0, 0.031, 0.032},    {18.0, 0.0, 0.024, 0.025},    {19.0, 0.0, 0.086, 0.087},
           {20.0, 0.0, 0.175, 0.176},    {21.0, 0.0, 0.219, 0.22},     {22.0, 0.0, 0.228, 0.229},
           {23.0, 0.0, 0.204, 0.205},    {24.0, 0.0, 0.186, 0.186},    {25.0, 0.0, 0.171, 0.169}});

        return aMeasurement;
    }

protected:
    virtual void SetUp()
    {
        const auto aSolarRadiation = loadSolarRadiationFile();

        // Wavelength data set according to NFRC 2003 standard is from solar radiation file
        const auto wl = aSolarRadiation.getXArray();

        double thickness = 3.0e-3;   // [m]
        const auto aMaterial_21467 = Material::nBandMaterial(
          loadSampleData_NFRC_21467(), thickness, MaterialType::Monolithic, WavelengthRange::Solar);

        aMaterial_21467->setBandWavelengths(wl);

        const auto layer102 = SpecularLayer::createLayer(aMaterial_21467);

        m_Layer = CMultiPaneSpecular::create({layer102}, loadSolarRadiationFile());
    }

public:
    std::shared_ptr<CMultiPaneSpecular> getLayer() const
    {
        return m_Layer;
    };
};

TEST_F(EquivalentSpecularLayer_21467, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    const double angle = 0;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T =
      aLayer.getPropertySimple(PropertySimple::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.857620, T, 1e-6);

    const double Rf =
      aLayer.getPropertySimple(PropertySimple::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.108847, Rf, 1e-6);

    const double Rb =
      aLayer.getPropertySimple(PropertySimple::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.114567, Rb, 1e-6);

    const double Abs1 =
      aLayer.getAbsorptanceLayer(1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.033532, Abs1, 1e-6);

    const double Them =
      aLayer.getPropertySimple(PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.783723, Them, 1e-6);

    const double Rfhem =
      aLayer.getPropertySimple(PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.169476, Rfhem, 1e-6);

    const double Rbhem =
      aLayer.getPropertySimple(PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.174748, Rbhem, 1e-6);
}

TEST_F(EquivalentSpecularLayer_21467, TestAngleHemispherical10)
{
    SCOPED_TRACE("Begin Test: Hemispherical to hemispherical with ten integration points.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;
    std::vector<double> aAngles{0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

    CMultiPaneSpecular aLayer = *getLayer();

    double Tfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.783723, Tfhem, 1e-6);

    double Tbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.783723, Tbhem, 1e-6);

    double Rfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.169476, Rfhem, 1e-6);

    double Rbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.174748, Rbhem, 1e-6);

    double Abs1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.036625, Abs1, 1e-6);
}
