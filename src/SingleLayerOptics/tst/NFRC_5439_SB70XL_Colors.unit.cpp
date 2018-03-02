#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCESpectralAveraging.hpp"

using FenestrationCommon::CSeries;

class TestNFRC_5439_SB70XL_Colors: public testing::Test {

private:

	std::shared_ptr< SingleLayerOptics::ColorProperties > m_Color;

	std::shared_ptr< std::vector< double > > loadWavelengths() const {
		auto aWavelengths = std::make_shared< std::vector< double > >();

		aWavelengths->push_back( 0.380 );
		aWavelengths->push_back( 0.385 );
		aWavelengths->push_back( 0.390 );
		aWavelengths->push_back( 0.395 );
		aWavelengths->push_back( 0.400 );
		aWavelengths->push_back( 0.405 );
		aWavelengths->push_back( 0.410 );
		aWavelengths->push_back( 0.415 );
		aWavelengths->push_back( 0.420 );
		aWavelengths->push_back( 0.425 );
		aWavelengths->push_back( 0.430 );
		aWavelengths->push_back( 0.435 );
		aWavelengths->push_back( 0.440 );
		aWavelengths->push_back( 0.445 );
		aWavelengths->push_back( 0.450 );
		aWavelengths->push_back( 0.455 );
		aWavelengths->push_back( 0.460 );
		aWavelengths->push_back( 0.465 );
		aWavelengths->push_back( 0.470 );
		aWavelengths->push_back( 0.475 );
		aWavelengths->push_back( 0.480 );
		aWavelengths->push_back( 0.485 );
		aWavelengths->push_back( 0.490 );
		aWavelengths->push_back( 0.495 );
		aWavelengths->push_back( 0.500 );
		aWavelengths->push_back( 0.505 );
		aWavelengths->push_back( 0.510 );
		aWavelengths->push_back( 0.515 );
		aWavelengths->push_back( 0.520 );
		aWavelengths->push_back( 0.525 );
		aWavelengths->push_back( 0.530 );
		aWavelengths->push_back( 0.535 );
		aWavelengths->push_back( 0.540 );
		aWavelengths->push_back( 0.545 );
		aWavelengths->push_back( 0.550 );
		aWavelengths->push_back( 0.555 );
		aWavelengths->push_back( 0.560 );
		aWavelengths->push_back( 0.565 );
		aWavelengths->push_back( 0.570 );
		aWavelengths->push_back( 0.575 );
		aWavelengths->push_back( 0.580 );
		aWavelengths->push_back( 0.585 );
		aWavelengths->push_back( 0.590 );
		aWavelengths->push_back( 0.595 );
		aWavelengths->push_back( 0.600 );
		aWavelengths->push_back( 0.605 );
		aWavelengths->push_back( 0.610 );
		aWavelengths->push_back( 0.615 );
		aWavelengths->push_back( 0.620 );
		aWavelengths->push_back( 0.625 );
		aWavelengths->push_back( 0.630 );
		aWavelengths->push_back( 0.635 );
		aWavelengths->push_back( 0.640 );
		aWavelengths->push_back( 0.645 );
		aWavelengths->push_back( 0.650 );
		aWavelengths->push_back( 0.655 );
		aWavelengths->push_back( 0.660 );
		aWavelengths->push_back( 0.665 );
		aWavelengths->push_back( 0.670 );
		aWavelengths->push_back( 0.675 );
		aWavelengths->push_back( 0.680 );
		aWavelengths->push_back( 0.685 );
		aWavelengths->push_back( 0.690 );
		aWavelengths->push_back( 0.695 );
		aWavelengths->push_back( 0.700 );
		aWavelengths->push_back( 0.705 );
		aWavelengths->push_back( 0.710 );
		aWavelengths->push_back( 0.715 );
		aWavelengths->push_back( 0.720 );
		aWavelengths->push_back( 0.725 );
		aWavelengths->push_back( 0.730 );
		aWavelengths->push_back( 0.735 );
		aWavelengths->push_back( 0.740 );
		aWavelengths->push_back( 0.745 );
		aWavelengths->push_back( 0.750 );
		aWavelengths->push_back( 0.755 );
		aWavelengths->push_back( 0.760 );
		aWavelengths->push_back( 0.765 );
		aWavelengths->push_back( 0.770 );
		aWavelengths->push_back( 0.775 );
		aWavelengths->push_back( 0.780 );

		return aWavelengths;
	}

	std::shared_ptr< CSeries > loadSolarRadiationFile() const {

		auto solarRadiation = std::make_shared< CSeries >();

		// Full CIE Illuminant D651 nm ssp table (used for PHOTOPIC properties)
		solarRadiation->addProperty( 0.300, 0.0341000 );
		solarRadiation->addProperty( 0.301, 0.3601400 );
		solarRadiation->addProperty( 0.302, 0.6861800 );
		solarRadiation->addProperty( 0.303, 1.0122200 );
		solarRadiation->addProperty( 0.304, 1.3382600 );
		solarRadiation->addProperty( 0.305, 1.6643000 );
		solarRadiation->addProperty( 0.306, 1.9903400 );
		solarRadiation->addProperty( 0.307, 2.3163800 );
		solarRadiation->addProperty( 0.308, 2.6424200 );
		solarRadiation->addProperty( 0.309, 2.9684600 );
		solarRadiation->addProperty( 0.310, 3.2945000 );
		solarRadiation->addProperty( 0.311, 4.9886500 );
		solarRadiation->addProperty( 0.312, 6.6828000 );
		solarRadiation->addProperty( 0.313, 8.3769500 );
		solarRadiation->addProperty( 0.314, 10.071100 );
		solarRadiation->addProperty( 0.315, 11.765200 );
		solarRadiation->addProperty( 0.316, 13.459400 );
		solarRadiation->addProperty( 0.317, 15.153500 );
		solarRadiation->addProperty( 0.318, 16.847700 );
		solarRadiation->addProperty( 0.319, 18.541800 );
		solarRadiation->addProperty( 0.320, 20.236000 );
		solarRadiation->addProperty( 0.321, 21.917700 );
		solarRadiation->addProperty( 0.322, 23.599500 );
		solarRadiation->addProperty( 0.323, 25.281200 );
		solarRadiation->addProperty( 0.324, 26.963000 );
		solarRadiation->addProperty( 0.325, 28.644700 );
		solarRadiation->addProperty( 0.326, 30.326500 );
		solarRadiation->addProperty( 0.327, 32.008200 );
		solarRadiation->addProperty( 0.328, 33.690000 );
		solarRadiation->addProperty( 0.329, 35.371700 );
		solarRadiation->addProperty( 0.330, 37.053500 );
		solarRadiation->addProperty( 0.331, 37.343000 );
		solarRadiation->addProperty( 0.332, 37.632600 );
		solarRadiation->addProperty( 0.333, 37.922100 );
		solarRadiation->addProperty( 0.334, 38.211600 );
		solarRadiation->addProperty( 0.335, 38.501100 );
		solarRadiation->addProperty( 0.336, 38.790700 );
		solarRadiation->addProperty( 0.337, 39.080200 );
		solarRadiation->addProperty( 0.338, 39.369700 );
		solarRadiation->addProperty( 0.339, 39.659300 );
		solarRadiation->addProperty( 0.340, 39.948800 );
		solarRadiation->addProperty( 0.341, 40.445100 );
		solarRadiation->addProperty( 0.342, 40.941400 );
		solarRadiation->addProperty( 0.343, 41.437700 );
		solarRadiation->addProperty( 0.344, 41.934000 );
		solarRadiation->addProperty( 0.345, 42.430200 );
		solarRadiation->addProperty( 0.346, 42.926500 );
		solarRadiation->addProperty( 0.347, 43.422800 );
		solarRadiation->addProperty( 0.348, 43.919100 );
		solarRadiation->addProperty( 0.349, 44.415400 );
		solarRadiation->addProperty( 0.350, 44.911700 );
		solarRadiation->addProperty( 0.351, 45.084400 );
		solarRadiation->addProperty( 0.352, 45.257000 );
		solarRadiation->addProperty( 0.353, 45.429700 );
		solarRadiation->addProperty( 0.354, 45.602300 );
		solarRadiation->addProperty( 0.355, 45.775000 );
		solarRadiation->addProperty( 0.356, 45.947700 );
		solarRadiation->addProperty( 0.357, 46.120300 );
		solarRadiation->addProperty( 0.358, 46.293000 );
		solarRadiation->addProperty( 0.359, 46.465600 );
		solarRadiation->addProperty( 0.360, 46.638300 );
		solarRadiation->addProperty( 0.361, 47.183400 );
		solarRadiation->addProperty( 0.362, 47.728500 );
		solarRadiation->addProperty( 0.363, 48.273500 );
		solarRadiation->addProperty( 0.364, 48.818600 );
		solarRadiation->addProperty( 0.365, 49.363700 );
		solarRadiation->addProperty( 0.366, 49.908800 );
		solarRadiation->addProperty( 0.367, 50.453900 );
		solarRadiation->addProperty( 0.368, 50.998900 );
		solarRadiation->addProperty( 0.369, 51.544000 );
		solarRadiation->addProperty( 0.370, 52.089100 );
		solarRadiation->addProperty( 0.371, 51.877700 );
		solarRadiation->addProperty( 0.372, 51.666400 );
		solarRadiation->addProperty( 0.373, 51.455000 );
		solarRadiation->addProperty( 0.374, 51.243700 );
		solarRadiation->addProperty( 0.375, 51.032300 );
		solarRadiation->addProperty( 0.376, 50.820900 );
		solarRadiation->addProperty( 0.377, 50.609600 );
		solarRadiation->addProperty( 0.378, 50.398200 );
		solarRadiation->addProperty( 0.379, 50.186900 );
		solarRadiation->addProperty( 0.380, 49.975500 );
		solarRadiation->addProperty( 0.381, 50.442800 );
		solarRadiation->addProperty( 0.382, 50.910000 );
		solarRadiation->addProperty( 0.383, 51.377300 );
		solarRadiation->addProperty( 0.384, 51.844600 );
		solarRadiation->addProperty( 0.385, 52.311800 );
		solarRadiation->addProperty( 0.386, 52.779100 );
		solarRadiation->addProperty( 0.387, 53.246400 );
		solarRadiation->addProperty( 0.388, 53.713700 );
		solarRadiation->addProperty( 0.389, 54.180900 );
		solarRadiation->addProperty( 0.390, 54.648200 );
		solarRadiation->addProperty( 0.391, 57.458900 );
		solarRadiation->addProperty( 0.392, 60.269500 );
		solarRadiation->addProperty( 0.393, 63.080200 );
		solarRadiation->addProperty( 0.394, 65.890900 );
		solarRadiation->addProperty( 0.395, 68.701500 );
		solarRadiation->addProperty( 0.396, 71.512200 );
		solarRadiation->addProperty( 0.397, 74.322900 );
		solarRadiation->addProperty( 0.398, 77.133600 );
		solarRadiation->addProperty( 0.399, 79.944200 );
		solarRadiation->addProperty( 0.400, 82.754900 );
		solarRadiation->addProperty( 0.401, 83.628000 );
		solarRadiation->addProperty( 0.402, 84.501100 );
		solarRadiation->addProperty( 0.403, 85.374200 );
		solarRadiation->addProperty( 0.404, 86.247300 );
		solarRadiation->addProperty( 0.405, 87.120400 );
		solarRadiation->addProperty( 0.406, 87.993600 );
		solarRadiation->addProperty( 0.407, 88.866700 );
		solarRadiation->addProperty( 0.408, 89.739800 );
		solarRadiation->addProperty( 0.409, 90.612900 );
		solarRadiation->addProperty( 0.410, 91.486000 );
		solarRadiation->addProperty( 0.411, 91.680600 );
		solarRadiation->addProperty( 0.412, 91.875200 );
		solarRadiation->addProperty( 0.413, 92.069700 );
		solarRadiation->addProperty( 0.414, 92.264300 );
		solarRadiation->addProperty( 0.415, 92.458900 );
		solarRadiation->addProperty( 0.416, 92.653500 );
		solarRadiation->addProperty( 0.417, 92.848100 );
		solarRadiation->addProperty( 0.418, 93.042600 );
		solarRadiation->addProperty( 0.419, 93.237200 );
		solarRadiation->addProperty( 0.420, 93.431800 );
		solarRadiation->addProperty( 0.421, 92.756800 );
		solarRadiation->addProperty( 0.422, 92.081900 );
		solarRadiation->addProperty( 0.423, 91.406900 );
		solarRadiation->addProperty( 0.424, 90.732000 );
		solarRadiation->addProperty( 0.425, 90.057000 );
		solarRadiation->addProperty( 0.426, 89.382100 );
		solarRadiation->addProperty( 0.427, 88.707100 );
		solarRadiation->addProperty( 0.428, 88.032200 );
		solarRadiation->addProperty( 0.429, 87.357200 );
		solarRadiation->addProperty( 0.430, 86.682300 );
		solarRadiation->addProperty( 0.431, 88.500600 );
		solarRadiation->addProperty( 0.432, 90.318800 );
		solarRadiation->addProperty( 0.433, 92.137100 );
		solarRadiation->addProperty( 0.434, 93.955400 );
		solarRadiation->addProperty( 0.435, 95.773600 );
		solarRadiation->addProperty( 0.436, 97.591900 );
		solarRadiation->addProperty( 0.437, 99.410200 );
		solarRadiation->addProperty( 0.438, 101.22800 );
		solarRadiation->addProperty( 0.439, 103.04700 );
		solarRadiation->addProperty( 0.440, 104.86500 );
		solarRadiation->addProperty( 0.441, 106.07900 );
		solarRadiation->addProperty( 0.442, 107.29400 );
		solarRadiation->addProperty( 0.443, 108.50800 );
		solarRadiation->addProperty( 0.444, 109.72200 );
		solarRadiation->addProperty( 0.445, 110.93600 );
		solarRadiation->addProperty( 0.446, 112.15100 );
		solarRadiation->addProperty( 0.447, 113.36500 );
		solarRadiation->addProperty( 0.448, 114.57900 );
		solarRadiation->addProperty( 0.449, 115.79400 );
		solarRadiation->addProperty( 0.450, 117.00800 );
		solarRadiation->addProperty( 0.451, 117.08800 );
		solarRadiation->addProperty( 0.452, 117.16900 );
		solarRadiation->addProperty( 0.453, 117.24900 );
		solarRadiation->addProperty( 0.454, 117.33000 );
		solarRadiation->addProperty( 0.455, 117.41000 );
		solarRadiation->addProperty( 0.456, 117.49000 );
		solarRadiation->addProperty( 0.457, 117.57100 );
		solarRadiation->addProperty( 0.458, 117.65100 );
		solarRadiation->addProperty( 0.459, 117.73200 );
		solarRadiation->addProperty( 0.460, 117.81200 );
		solarRadiation->addProperty( 0.461, 117.51700 );
		solarRadiation->addProperty( 0.462, 117.22200 );
		solarRadiation->addProperty( 0.463, 116.92700 );
		solarRadiation->addProperty( 0.464, 116.63200 );
		solarRadiation->addProperty( 0.465, 116.33600 );
		solarRadiation->addProperty( 0.466, 116.04100 );
		solarRadiation->addProperty( 0.467, 115.74600 );
		solarRadiation->addProperty( 0.468, 115.45100 );
		solarRadiation->addProperty( 0.469, 115.15600 );
		solarRadiation->addProperty( 0.470, 114.86100 );
		solarRadiation->addProperty( 0.471, 114.96700 );
		solarRadiation->addProperty( 0.472, 115.07300 );
		solarRadiation->addProperty( 0.473, 115.18000 );
		solarRadiation->addProperty( 0.474, 115.28600 );
		solarRadiation->addProperty( 0.475, 115.39200 );
		solarRadiation->addProperty( 0.476, 115.49800 );
		solarRadiation->addProperty( 0.477, 115.60400 );
		solarRadiation->addProperty( 0.478, 115.71100 );
		solarRadiation->addProperty( 0.479, 115.81700 );
		solarRadiation->addProperty( 0.480, 115.92300 );
		solarRadiation->addProperty( 0.481, 115.21200 );
		solarRadiation->addProperty( 0.482, 114.50100 );
		solarRadiation->addProperty( 0.483, 113.78900 );
		solarRadiation->addProperty( 0.484, 113.07800 );
		solarRadiation->addProperty( 0.485, 112.36700 );
		solarRadiation->addProperty( 0.486, 111.65600 );
		solarRadiation->addProperty( 0.487, 110.94500 );
		solarRadiation->addProperty( 0.488, 110.23300 );
		solarRadiation->addProperty( 0.489, 109.52200 );
		solarRadiation->addProperty( 0.490, 108.81100 );
		solarRadiation->addProperty( 0.491, 108.86500 );
		solarRadiation->addProperty( 0.492, 108.92000 );
		solarRadiation->addProperty( 0.493, 108.97400 );
		solarRadiation->addProperty( 0.494, 109.02800 );
		solarRadiation->addProperty( 0.495, 109.08200 );
		solarRadiation->addProperty( 0.496, 109.13700 );
		solarRadiation->addProperty( 0.497, 109.19100 );
		solarRadiation->addProperty( 0.498, 109.24500 );
		solarRadiation->addProperty( 0.499, 109.30000 );
		solarRadiation->addProperty( 0.500, 109.35400 );
		solarRadiation->addProperty( 0.501, 109.19900 );
		solarRadiation->addProperty( 0.502, 109.04400 );
		solarRadiation->addProperty( 0.503, 108.88800 );
		solarRadiation->addProperty( 0.504, 108.73300 );
		solarRadiation->addProperty( 0.505, 108.57800 );
		solarRadiation->addProperty( 0.506, 108.42300 );
		solarRadiation->addProperty( 0.507, 108.26800 );
		solarRadiation->addProperty( 0.508, 108.11200 );
		solarRadiation->addProperty( 0.509, 107.95700 );
		solarRadiation->addProperty( 0.510, 107.80200 );
		solarRadiation->addProperty( 0.511, 107.50100 );
		solarRadiation->addProperty( 0.512, 107.20000 );
		solarRadiation->addProperty( 0.513, 106.89800 );
		solarRadiation->addProperty( 0.514, 106.59700 );
		solarRadiation->addProperty( 0.515, 106.29600 );
		solarRadiation->addProperty( 0.516, 105.99500 );
		solarRadiation->addProperty( 0.517, 105.69400 );
		solarRadiation->addProperty( 0.518, 105.39200 );
		solarRadiation->addProperty( 0.519, 105.09100 );
		solarRadiation->addProperty( 0.520, 104.79000 );
		solarRadiation->addProperty( 0.521, 105.08000 );
		solarRadiation->addProperty( 0.522, 105.37000 );
		solarRadiation->addProperty( 0.523, 105.66000 );
		solarRadiation->addProperty( 0.524, 105.95000 );
		solarRadiation->addProperty( 0.525, 106.23900 );
		solarRadiation->addProperty( 0.526, 106.52900 );
		solarRadiation->addProperty( 0.527, 106.81900 );
		solarRadiation->addProperty( 0.528, 107.10900 );
		solarRadiation->addProperty( 0.529, 107.39900 );
		solarRadiation->addProperty( 0.530, 107.68900 );
		solarRadiation->addProperty( 0.531, 107.36100 );
		solarRadiation->addProperty( 0.532, 107.03200 );
		solarRadiation->addProperty( 0.533, 106.70400 );
		solarRadiation->addProperty( 0.534, 106.37500 );
		solarRadiation->addProperty( 0.535, 106.04700 );
		solarRadiation->addProperty( 0.536, 105.71900 );
		solarRadiation->addProperty( 0.537, 105.39000 );
		solarRadiation->addProperty( 0.538, 105.06200 );
		solarRadiation->addProperty( 0.539, 104.73300 );
		solarRadiation->addProperty( 0.540, 104.40500 );
		solarRadiation->addProperty( 0.541, 104.36900 );
		solarRadiation->addProperty( 0.542, 104.33300 );
		solarRadiation->addProperty( 0.543, 104.29700 );
		solarRadiation->addProperty( 0.544, 104.26100 );
		solarRadiation->addProperty( 0.545, 104.22500 );
		solarRadiation->addProperty( 0.546, 104.19000 );
		solarRadiation->addProperty( 0.547, 104.15400 );
		solarRadiation->addProperty( 0.548, 104.11800 );
		solarRadiation->addProperty( 0.549, 104.08200 );
		solarRadiation->addProperty( 0.550, 104.04600 );
		solarRadiation->addProperty( 0.551, 103.64100 );
		solarRadiation->addProperty( 0.552, 103.23700 );
		solarRadiation->addProperty( 0.553, 102.83200 );
		solarRadiation->addProperty( 0.554, 102.42800 );
		solarRadiation->addProperty( 0.555, 102.02300 );
		solarRadiation->addProperty( 0.556, 101.61800 );
		solarRadiation->addProperty( 0.557, 101.21400 );
		solarRadiation->addProperty( 0.558, 100.80900 );
		solarRadiation->addProperty( 0.559, 100.40500 );
		solarRadiation->addProperty( 0.560, 100.00000 );
		solarRadiation->addProperty( 0.561, 99.633400 );
		solarRadiation->addProperty( 0.562, 99.266800 );
		solarRadiation->addProperty( 0.563, 98.900300 );
		solarRadiation->addProperty( 0.564, 98.533700 );
		solarRadiation->addProperty( 0.565, 98.167100 );
		solarRadiation->addProperty( 0.566, 97.800500 );
		solarRadiation->addProperty( 0.567, 97.433900 );
		solarRadiation->addProperty( 0.568, 97.067400 );
		solarRadiation->addProperty( 0.569, 96.700800 );
		solarRadiation->addProperty( 0.570, 96.334200 );
		solarRadiation->addProperty( 0.571, 96.279600 );
		solarRadiation->addProperty( 0.572, 96.225000 );
		solarRadiation->addProperty( 0.573, 96.170300 );
		solarRadiation->addProperty( 0.574, 96.115700 );
		solarRadiation->addProperty( 0.575, 96.061100 );
		solarRadiation->addProperty( 0.576, 96.006500 );
		solarRadiation->addProperty( 0.577, 95.951900 );
		solarRadiation->addProperty( 0.578, 95.897200 );
		solarRadiation->addProperty( 0.579, 95.842600 );
		solarRadiation->addProperty( 0.580, 95.788000 );
		solarRadiation->addProperty( 0.581, 95.077800 );
		solarRadiation->addProperty( 0.582, 94.367500 );
		solarRadiation->addProperty( 0.583, 93.657300 );
		solarRadiation->addProperty( 0.584, 92.947000 );
		solarRadiation->addProperty( 0.585, 92.236800 );
		solarRadiation->addProperty( 0.586, 91.526600 );
		solarRadiation->addProperty( 0.587, 90.816300 );
		solarRadiation->addProperty( 0.588, 90.106100 );
		solarRadiation->addProperty( 0.589, 89.395800 );
		solarRadiation->addProperty( 0.590, 88.685600 );
		solarRadiation->addProperty( 0.591, 88.817700 );
		solarRadiation->addProperty( 0.592, 88.949700 );
		solarRadiation->addProperty( 0.593, 89.081800 );
		solarRadiation->addProperty( 0.594, 89.213800 );
		solarRadiation->addProperty( 0.595, 89.345900 );
		solarRadiation->addProperty( 0.596, 89.478000 );
		solarRadiation->addProperty( 0.597, 89.610000 );
		solarRadiation->addProperty( 0.598, 89.742100 );
		solarRadiation->addProperty( 0.599, 89.874100 );
		solarRadiation->addProperty( 0.600, 90.006200 );
		solarRadiation->addProperty( 0.601, 89.965500 );
		solarRadiation->addProperty( 0.602, 89.924800 );
		solarRadiation->addProperty( 0.603, 89.884100 );
		solarRadiation->addProperty( 0.604, 89.843400 );
		solarRadiation->addProperty( 0.605, 89.802600 );
		solarRadiation->addProperty( 0.606, 89.761900 );
		solarRadiation->addProperty( 0.607, 89.721200 );
		solarRadiation->addProperty( 0.608, 89.680500 );
		solarRadiation->addProperty( 0.609, 89.639800 );
		solarRadiation->addProperty( 0.610, 89.599100 );
		solarRadiation->addProperty( 0.611, 89.409100 );
		solarRadiation->addProperty( 0.612, 89.219000 );
		solarRadiation->addProperty( 0.613, 89.029000 );
		solarRadiation->addProperty( 0.614, 88.838900 );
		solarRadiation->addProperty( 0.615, 88.648900 );
		solarRadiation->addProperty( 0.616, 88.458900 );
		solarRadiation->addProperty( 0.617, 88.268800 );
		solarRadiation->addProperty( 0.618, 88.078800 );
		solarRadiation->addProperty( 0.619, 87.888700 );
		solarRadiation->addProperty( 0.620, 87.698700 );
		solarRadiation->addProperty( 0.621, 87.257700 );
		solarRadiation->addProperty( 0.622, 86.816700 );
		solarRadiation->addProperty( 0.623, 86.375700 );
		solarRadiation->addProperty( 0.624, 85.934700 );
		solarRadiation->addProperty( 0.625, 85.493600 );
		solarRadiation->addProperty( 0.626, 85.052600 );
		solarRadiation->addProperty( 0.627, 84.611600 );
		solarRadiation->addProperty( 0.628, 84.170600 );
		solarRadiation->addProperty( 0.629, 83.729600 );
		solarRadiation->addProperty( 0.630, 83.288600 );
		solarRadiation->addProperty( 0.631, 83.329700 );
		solarRadiation->addProperty( 0.632, 83.370700 );
		solarRadiation->addProperty( 0.633, 83.411800 );
		solarRadiation->addProperty( 0.634, 83.452800 );
		solarRadiation->addProperty( 0.635, 83.493900 );
		solarRadiation->addProperty( 0.636, 83.535000 );
		solarRadiation->addProperty( 0.637, 83.576000 );
		solarRadiation->addProperty( 0.638, 83.617100 );
		solarRadiation->addProperty( 0.639, 83.658100 );
		solarRadiation->addProperty( 0.640, 83.699200 );
		solarRadiation->addProperty( 0.641, 83.332000 );
		solarRadiation->addProperty( 0.642, 82.964700 );
		solarRadiation->addProperty( 0.643, 82.597500 );
		solarRadiation->addProperty( 0.644, 82.230200 );
		solarRadiation->addProperty( 0.645, 81.863000 );
		solarRadiation->addProperty( 0.646, 81.495800 );
		solarRadiation->addProperty( 0.647, 81.128500 );
		solarRadiation->addProperty( 0.648, 80.761300 );
		solarRadiation->addProperty( 0.649, 80.394000 );
		solarRadiation->addProperty( 0.650, 80.026800 );
		solarRadiation->addProperty( 0.651, 80.045600 );
		solarRadiation->addProperty( 0.652, 80.064400 );
		solarRadiation->addProperty( 0.653, 80.083100 );
		solarRadiation->addProperty( 0.654, 80.101900 );
		solarRadiation->addProperty( 0.655, 80.120700 );
		solarRadiation->addProperty( 0.656, 80.139500 );
		solarRadiation->addProperty( 0.657, 80.158300 );
		solarRadiation->addProperty( 0.658, 80.177000 );
		solarRadiation->addProperty( 0.659, 80.195800 );
		solarRadiation->addProperty( 0.660, 80.214600 );
		solarRadiation->addProperty( 0.661, 80.420900 );
		solarRadiation->addProperty( 0.662, 80.627200 );
		solarRadiation->addProperty( 0.663, 80.833600 );
		solarRadiation->addProperty( 0.664, 81.039900 );
		solarRadiation->addProperty( 0.665, 81.246200 );
		solarRadiation->addProperty( 0.666, 81.452500 );
		solarRadiation->addProperty( 0.667, 81.658800 );
		solarRadiation->addProperty( 0.668, 81.865200 );
		solarRadiation->addProperty( 0.669, 82.071500 );
		solarRadiation->addProperty( 0.670, 82.277800 );
		solarRadiation->addProperty( 0.671, 81.878400 );
		solarRadiation->addProperty( 0.672, 81.479100 );
		solarRadiation->addProperty( 0.673, 81.079700 );
		solarRadiation->addProperty( 0.674, 80.680400 );
		solarRadiation->addProperty( 0.675, 80.281000 );
		solarRadiation->addProperty( 0.676, 79.881600 );
		solarRadiation->addProperty( 0.677, 79.482300 );
		solarRadiation->addProperty( 0.678, 79.082900 );
		solarRadiation->addProperty( 0.679, 78.683600 );
		solarRadiation->addProperty( 0.680, 78.284200 );
		solarRadiation->addProperty( 0.681, 77.427900 );
		solarRadiation->addProperty( 0.682, 76.571600 );
		solarRadiation->addProperty( 0.683, 75.715300 );
		solarRadiation->addProperty( 0.684, 74.859000 );
		solarRadiation->addProperty( 0.685, 74.002700 );
		solarRadiation->addProperty( 0.686, 73.146500 );
		solarRadiation->addProperty( 0.687, 72.290200 );
		solarRadiation->addProperty( 0.688, 71.433900 );
		solarRadiation->addProperty( 0.689, 70.577600 );
		solarRadiation->addProperty( 0.690, 69.721300 );
		solarRadiation->addProperty( 0.691, 69.910100 );
		solarRadiation->addProperty( 0.692, 70.098900 );
		solarRadiation->addProperty( 0.693, 70.287600 );
		solarRadiation->addProperty( 0.694, 70.476400 );
		solarRadiation->addProperty( 0.695, 70.665200 );
		solarRadiation->addProperty( 0.696, 70.854000 );
		solarRadiation->addProperty( 0.697, 71.042800 );
		solarRadiation->addProperty( 0.698, 71.231500 );
		solarRadiation->addProperty( 0.699, 71.420300 );
		solarRadiation->addProperty( 0.700, 71.609100 );
		solarRadiation->addProperty( 0.701, 71.883100 );
		solarRadiation->addProperty( 0.702, 72.157100 );
		solarRadiation->addProperty( 0.703, 72.431100 );
		solarRadiation->addProperty( 0.704, 72.705100 );
		solarRadiation->addProperty( 0.705, 72.979000 );
		solarRadiation->addProperty( 0.706, 73.253000 );
		solarRadiation->addProperty( 0.707, 73.527000 );
		solarRadiation->addProperty( 0.708, 73.801000 );
		solarRadiation->addProperty( 0.709, 74.075000 );
		solarRadiation->addProperty( 0.710, 74.349000 );
		solarRadiation->addProperty( 0.711, 73.074500 );
		solarRadiation->addProperty( 0.712, 71.800000 );
		solarRadiation->addProperty( 0.713, 70.525500 );
		solarRadiation->addProperty( 0.714, 69.251000 );
		solarRadiation->addProperty( 0.715, 67.976500 );
		solarRadiation->addProperty( 0.716, 66.702000 );
		solarRadiation->addProperty( 0.717, 65.427500 );
		solarRadiation->addProperty( 0.718, 64.153000 );
		solarRadiation->addProperty( 0.719, 62.878500 );
		solarRadiation->addProperty( 0.720, 61.604000 );
		solarRadiation->addProperty( 0.721, 62.432200 );
		solarRadiation->addProperty( 0.722, 63.260300 );
		solarRadiation->addProperty( 0.723, 64.088500 );
		solarRadiation->addProperty( 0.724, 64.916600 );
		solarRadiation->addProperty( 0.725, 65.744800 );
		solarRadiation->addProperty( 0.726, 66.573000 );
		solarRadiation->addProperty( 0.727, 67.401100 );
		solarRadiation->addProperty( 0.728, 68.229300 );
		solarRadiation->addProperty( 0.729, 69.057400 );
		solarRadiation->addProperty( 0.730, 69.885600 );
		solarRadiation->addProperty( 0.731, 70.405700 );
		solarRadiation->addProperty( 0.732, 70.925900 );
		solarRadiation->addProperty( 0.733, 71.446000 );
		solarRadiation->addProperty( 0.734, 71.966200 );
		solarRadiation->addProperty( 0.735, 72.486300 );
		solarRadiation->addProperty( 0.736, 73.006400 );
		solarRadiation->addProperty( 0.737, 73.526600 );
		solarRadiation->addProperty( 0.738, 74.046700 );
		solarRadiation->addProperty( 0.739, 74.566900 );
		solarRadiation->addProperty( 0.740, 75.087000 );
		solarRadiation->addProperty( 0.741, 73.937600 );
		solarRadiation->addProperty( 0.742, 72.788100 );
		solarRadiation->addProperty( 0.743, 71.638700 );
		solarRadiation->addProperty( 0.744, 70.489300 );
		solarRadiation->addProperty( 0.745, 69.339800 );
		solarRadiation->addProperty( 0.746, 68.190400 );
		solarRadiation->addProperty( 0.747, 67.041000 );
		solarRadiation->addProperty( 0.748, 65.891600 );
		solarRadiation->addProperty( 0.749, 64.742100 );
		solarRadiation->addProperty( 0.750, 63.592700 );
		solarRadiation->addProperty( 0.751, 61.875200 );
		solarRadiation->addProperty( 0.752, 60.157800 );
		solarRadiation->addProperty( 0.753, 58.440300 );
		solarRadiation->addProperty( 0.754, 56.722900 );
		solarRadiation->addProperty( 0.755, 55.005400 );
		solarRadiation->addProperty( 0.756, 53.288000 );
		solarRadiation->addProperty( 0.757, 51.570500 );
		solarRadiation->addProperty( 0.758, 49.853100 );
		solarRadiation->addProperty( 0.759, 48.135600 );
		solarRadiation->addProperty( 0.760, 46.418200 );
		solarRadiation->addProperty( 0.761, 48.456900 );
		solarRadiation->addProperty( 0.762, 50.495600 );
		solarRadiation->addProperty( 0.763, 52.534400 );
		solarRadiation->addProperty( 0.764, 54.573100 );
		solarRadiation->addProperty( 0.765, 56.611800 );
		solarRadiation->addProperty( 0.766, 58.650500 );
		solarRadiation->addProperty( 0.767, 60.689200 );
		solarRadiation->addProperty( 0.768, 62.728000 );
		solarRadiation->addProperty( 0.769, 64.766700 );
		solarRadiation->addProperty( 0.770, 66.805400 );
		solarRadiation->addProperty( 0.771, 66.463100 );
		solarRadiation->addProperty( 0.772, 66.120900 );
		solarRadiation->addProperty( 0.773, 65.778600 );
		solarRadiation->addProperty( 0.774, 65.436400 );
		solarRadiation->addProperty( 0.775, 65.094100 );
		solarRadiation->addProperty( 0.776, 64.751800 );
		solarRadiation->addProperty( 0.777, 64.409600 );
		solarRadiation->addProperty( 0.778, 64.067300 );
		solarRadiation->addProperty( 0.779, 63.725100 );
		solarRadiation->addProperty( 0.780, 63.382800 );
		solarRadiation->addProperty( 0.781, 63.474900 );
		solarRadiation->addProperty( 0.782, 63.567000 );
		solarRadiation->addProperty( 0.783, 63.659200 );
		solarRadiation->addProperty( 0.784, 63.751300 );
		solarRadiation->addProperty( 0.785, 63.843400 );
		solarRadiation->addProperty( 0.786, 63.935500 );
		solarRadiation->addProperty( 0.787, 64.027600 );
		solarRadiation->addProperty( 0.788, 64.119800 );
		solarRadiation->addProperty( 0.789, 64.211900 );
		solarRadiation->addProperty( 0.790, 64.304000 );
		solarRadiation->addProperty( 0.791, 63.818800 );
		solarRadiation->addProperty( 0.792, 63.333600 );
		solarRadiation->addProperty( 0.793, 62.848400 );
		solarRadiation->addProperty( 0.794, 62.363200 );
		solarRadiation->addProperty( 0.795, 61.877900 );
		solarRadiation->addProperty( 0.796, 61.392700 );
		solarRadiation->addProperty( 0.797, 60.907500 );
		solarRadiation->addProperty( 0.798, 60.422300 );
		solarRadiation->addProperty( 0.799, 59.937100 );
		solarRadiation->addProperty( 0.800, 59.451900 );
		solarRadiation->addProperty( 0.801, 58.702600 );
		solarRadiation->addProperty( 0.802, 57.953300 );
		solarRadiation->addProperty( 0.803, 57.204000 );
		solarRadiation->addProperty( 0.804, 56.454700 );
		solarRadiation->addProperty( 0.805, 55.705400 );
		solarRadiation->addProperty( 0.806, 54.956200 );
		solarRadiation->addProperty( 0.807, 54.206900 );
		solarRadiation->addProperty( 0.808, 53.457600 );
		solarRadiation->addProperty( 0.809, 52.708300 );
		solarRadiation->addProperty( 0.810, 51.959000 );
		solarRadiation->addProperty( 0.811, 52.507200 );
		solarRadiation->addProperty( 0.812, 53.055300 );
		solarRadiation->addProperty( 0.813, 53.603500 );
		solarRadiation->addProperty( 0.814, 54.151600 );
		solarRadiation->addProperty( 0.815, 54.699800 );
		solarRadiation->addProperty( 0.816, 55.248000 );
		solarRadiation->addProperty( 0.817, 55.796100 );
		solarRadiation->addProperty( 0.818, 56.344300 );
		solarRadiation->addProperty( 0.819, 56.892400 );
		solarRadiation->addProperty( 0.820, 57.440600 );
		solarRadiation->addProperty( 0.821, 57.727800 );
		solarRadiation->addProperty( 0.822, 58.015000 );
		solarRadiation->addProperty( 0.823, 58.302200 );
		solarRadiation->addProperty( 0.824, 58.589400 );
		solarRadiation->addProperty( 0.825, 58.876500 );
		solarRadiation->addProperty( 0.826, 59.163700 );
		solarRadiation->addProperty( 0.827, 59.450900 );
		solarRadiation->addProperty( 0.828, 59.738100 );
		solarRadiation->addProperty( 0.829, 60.025300 );
		solarRadiation->addProperty( 0.830, 60.312500 );

		return solarRadiation;
	}

	std::shared_ptr< SpectralAveraging::CSpectralSampleData > loadSampleData_NFRC_5439() const {
		std::shared_ptr< SpectralAveraging::CSpectralSampleData > aMeasurements_5439 = std::make_shared< SpectralAveraging::CSpectralSampleData >();

		aMeasurements_5439->addRecord( 0.300, 0.0019, 0.0491, 0.2686 );
		aMeasurements_5439->addRecord( 0.305, 0.0037, 0.0885, 0.2723 );
		aMeasurements_5439->addRecord( 0.310, 0.0062, 0.1118, 0.2856 );
		aMeasurements_5439->addRecord( 0.315, 0.0114, 0.1363, 0.3101 );
		aMeasurements_5439->addRecord( 0.320, 0.0209, 0.1595, 0.3418 );
		aMeasurements_5439->addRecord( 0.325, 0.0289, 0.1802, 0.3774 );
		aMeasurements_5439->addRecord( 0.330, 0.0404, 0.1970, 0.4123 );
		aMeasurements_5439->addRecord( 0.335, 0.0605, 0.2098, 0.4415 );
		aMeasurements_5439->addRecord( 0.340, 0.0653, 0.2192, 0.4636 );
		aMeasurements_5439->addRecord( 0.345, 0.0590, 0.2253, 0.4796 );
		aMeasurements_5439->addRecord( 0.350, 0.0559, 0.2295, 0.4911 );
		aMeasurements_5439->addRecord( 0.355, 0.0583, 0.2325, 0.4986 );
		aMeasurements_5439->addRecord( 0.360, 0.0657, 0.2341, 0.5029 );
		aMeasurements_5439->addRecord( 0.365, 0.0780, 0.2351, 0.5018 );
		aMeasurements_5439->addRecord( 0.370, 0.0959, 0.2374, 0.4958 );
		aMeasurements_5439->addRecord( 0.375, 0.1206, 0.2435, 0.4837 );
		aMeasurements_5439->addRecord( 0.380, 0.1534, 0.2485, 0.4627 );
		aMeasurements_5439->addRecord( 0.385, 0.1954, 0.2512, 0.4324 );
		aMeasurements_5439->addRecord( 0.390, 0.2466, 0.2485, 0.3929 );
		aMeasurements_5439->addRecord( 0.395, 0.3066, 0.2391, 0.3465 );
		aMeasurements_5439->addRecord( 0.400, 0.3718, 0.2233, 0.2960 );
		aMeasurements_5439->addRecord( 0.410, 0.5010, 0.1781, 0.1981 );
		aMeasurements_5439->addRecord( 0.420, 0.5990, 0.1340, 0.1288 );
		aMeasurements_5439->addRecord( 0.430, 0.6539, 0.1040, 0.0927 );
		aMeasurements_5439->addRecord( 0.440, 0.6761, 0.0893, 0.0782 );
		aMeasurements_5439->addRecord( 0.450, 0.6823, 0.0841, 0.0732 );
		aMeasurements_5439->addRecord( 0.460, 0.6858, 0.0833, 0.0710 );
		aMeasurements_5439->addRecord( 0.470, 0.6893, 0.0840, 0.0694 );
		aMeasurements_5439->addRecord( 0.480, 0.6948, 0.0850, 0.0677 );
		aMeasurements_5439->addRecord( 0.490, 0.7028, 0.0858, 0.0663 );
		aMeasurements_5439->addRecord( 0.500, 0.7102, 0.0859, 0.0652 );
		aMeasurements_5439->addRecord( 0.510, 0.7179, 0.0856, 0.0648 );
		aMeasurements_5439->addRecord( 0.520, 0.7232, 0.0850, 0.0647 );
		aMeasurements_5439->addRecord( 0.530, 0.7272, 0.0840, 0.0648 );
		aMeasurements_5439->addRecord( 0.540, 0.7296, 0.0829, 0.0650 );
		aMeasurements_5439->addRecord( 0.550, 0.7311, 0.0811, 0.0642 );
		aMeasurements_5439->addRecord( 0.560, 0.7315, 0.0785, 0.0624 );
		aMeasurements_5439->addRecord( 0.570, 0.7315, 0.0744, 0.0588 );
		aMeasurements_5439->addRecord( 0.580, 0.7293, 0.0686, 0.0529 );
		aMeasurements_5439->addRecord( 0.590, 0.7270, 0.0618, 0.0456 );
		aMeasurements_5439->addRecord( 0.600, 0.7224, 0.0545, 0.0377 );
		aMeasurements_5439->addRecord( 0.610, 0.7155, 0.0483, 0.0308 );
		aMeasurements_5439->addRecord( 0.620, 0.7025, 0.0456, 0.0280 );
		aMeasurements_5439->addRecord( 0.630, 0.6810, 0.0492, 0.0324 );
		aMeasurements_5439->addRecord( 0.640, 0.6507, 0.0629, 0.0487 );
		aMeasurements_5439->addRecord( 0.650, 0.6096, 0.0886, 0.0787 );
		aMeasurements_5439->addRecord( 0.660, 0.5585, 0.1279, 0.1245 );
		aMeasurements_5439->addRecord( 0.670, 0.5003, 0.1795, 0.1843 );
		aMeasurements_5439->addRecord( 0.680, 0.4374, 0.2398, 0.2544 );
		aMeasurements_5439->addRecord( 0.690, 0.3754, 0.3047, 0.3291 );
		aMeasurements_5439->addRecord( 0.700, 0.3171, 0.3709, 0.4036 );
		aMeasurements_5439->addRecord( 0.710, 0.2651, 0.4348, 0.4750 );
		aMeasurements_5439->addRecord( 0.720, 0.2194, 0.4917, 0.5401 );
		aMeasurements_5439->addRecord( 0.730, 0.1807, 0.5413, 0.5955 );
		aMeasurements_5439->addRecord( 0.740, 0.1494, 0.5855, 0.6448 );
		aMeasurements_5439->addRecord( 0.750, 0.1233, 0.6244, 0.6877 );
		aMeasurements_5439->addRecord( 0.760, 0.1025, 0.6563, 0.7243 );
		aMeasurements_5439->addRecord( 0.770, 0.0852, 0.6826, 0.7536 );
		aMeasurements_5439->addRecord( 0.780, 0.0716, 0.7092, 0.7786 );
		aMeasurements_5439->addRecord( 0.790, 0.0602, 0.7228, 0.8010 );
		aMeasurements_5439->addRecord( 0.800, 0.0509, 0.7419, 0.8199 );
		aMeasurements_5439->addRecord( 0.810, 0.0436, 0.7579, 0.8362 );
		aMeasurements_5439->addRecord( 0.820, 0.0373, 0.7693, 0.8507 );
		aMeasurements_5439->addRecord( 0.830, 0.0320, 0.7820, 0.8617 );
		aMeasurements_5439->addRecord( 0.840, 0.0280, 0.7918, 0.8711 );
		aMeasurements_5439->addRecord( 0.850, 0.0234, 0.7952, 0.8792 );
		aMeasurements_5439->addRecord( 0.860, 0.0206, 0.8083, 0.8928 );
		aMeasurements_5439->addRecord( 0.870, 0.0184, 0.8075, 0.8935 );
		aMeasurements_5439->addRecord( 0.880, 0.0161, 0.8138, 0.9007 );
		aMeasurements_5439->addRecord( 0.890, 0.0151, 0.8183, 0.9063 );
		aMeasurements_5439->addRecord( 0.900, 0.0131, 0.8220, 0.9101 );
		aMeasurements_5439->addRecord( 0.910, 0.0117, 0.8248, 0.9148 );
		aMeasurements_5439->addRecord( 0.920, 0.0108, 0.8258, 0.9189 );
		aMeasurements_5439->addRecord( 0.930, 0.0099, 0.8283, 0.9225 );
		aMeasurements_5439->addRecord( 0.940, 0.0089, 0.8305, 0.9256 );
		aMeasurements_5439->addRecord( 0.950, 0.0080, 0.8321, 0.9283 );
		aMeasurements_5439->addRecord( 0.960, 0.0075, 0.8337, 0.9310 );
		aMeasurements_5439->addRecord( 0.970, 0.0067, 0.8355, 0.9338 );
		aMeasurements_5439->addRecord( 0.980, 0.0061, 0.8373, 0.9360 );
		aMeasurements_5439->addRecord( 0.990, 0.0058, 0.8391, 0.9380 );
		aMeasurements_5439->addRecord( 1.000, 0.0053, 0.8410, 0.9400 );
		aMeasurements_5439->addRecord( 1.050, 0.0036, 0.8478, 0.9475 );
		aMeasurements_5439->addRecord( 1.100, 0.0014, 0.8533, 0.9534 );
		aMeasurements_5439->addRecord( 1.150, 0.0010, 0.8604, 0.9575 );
		aMeasurements_5439->addRecord( 1.200, 0.0005, 0.8639, 0.9608 );
		aMeasurements_5439->addRecord( 1.250, 0.0003, 0.8699, 0.9634 );
		aMeasurements_5439->addRecord( 1.300, 0.0002, 0.8740, 0.9655 );
		aMeasurements_5439->addRecord( 1.350, 0.0000, 0.8747, 0.9672 );
		aMeasurements_5439->addRecord( 1.400, 0.0005, 0.8759, 0.9682 );
		aMeasurements_5439->addRecord( 1.450, 0.0004, 0.8835, 0.9692 );
		aMeasurements_5439->addRecord( 1.500, 0.0003, 0.8897, 0.9702 );
		aMeasurements_5439->addRecord( 1.550, 0.0003, 0.8951, 0.9713 );
		aMeasurements_5439->addRecord( 1.600, 0.0003, 0.8984, 0.9719 );
		aMeasurements_5439->addRecord( 1.650, 0.0003, 0.8999, 0.9727 );
		aMeasurements_5439->addRecord( 1.700, 0.0006, 0.9005, 0.9734 );
		aMeasurements_5439->addRecord( 1.750, 0.0007, 0.8974, 0.9739 );
		aMeasurements_5439->addRecord( 1.800, 0.0005, 0.8887, 0.9745 );
		aMeasurements_5439->addRecord( 1.850, 0.0005, 0.8806, 0.9751 );
		aMeasurements_5439->addRecord( 1.900, 0.0004, 0.8788, 0.9754 );
		aMeasurements_5439->addRecord( 1.950, 0.0004, 0.8757, 0.9756 );
		aMeasurements_5439->addRecord( 2.000, 0.0005, 0.8688, 0.9759 );
		aMeasurements_5439->addRecord( 2.050, 0.0005, 0.8626, 0.9764 );
		aMeasurements_5439->addRecord( 2.100, 0.0005, 0.8565, 0.9766 );
		aMeasurements_5439->addRecord( 2.150, 0.0003, 0.8407, 0.9767 );
		aMeasurements_5439->addRecord( 2.200, 0.0008, 0.7981, 0.9768 );
		aMeasurements_5439->addRecord( 2.250, 0.0001, 0.7954, 0.9771 );
		aMeasurements_5439->addRecord( 2.300, 0.0008, 0.7861, 0.9763 );
		aMeasurements_5439->addRecord( 2.350, 0.0010, 0.7911, 0.9768 );
		aMeasurements_5439->addRecord( 2.400, 0.0004, 0.7827, 0.9781 );
		aMeasurements_5439->addRecord( 2.450, 0.0013, 0.7314, 0.9773 );
		aMeasurements_5439->addRecord( 2.500, 0.0009, 0.7243, 0.9779 );
		aMeasurements_5439->addRecord( 5.000, 0.0000, 0.0346, 0.9854 );
		aMeasurements_5439->addRecord( 6.000, 0.0000, 0.0278, 0.9847 );
		aMeasurements_5439->addRecord( 7.000, 0.0000, 0.0168, 0.9850 );
		aMeasurements_5439->addRecord( 8.000, 0.0000, 0.0011, 0.9857 );
		aMeasurements_5439->addRecord( 9.000, 0.0000, 0.1913, 0.9868 );
		aMeasurements_5439->addRecord( 10.000, 0.0000, 0.2714, 0.9845 );
		aMeasurements_5439->addRecord( 11.000, 0.0000, 0.1950, 0.9841 );
		aMeasurements_5439->addRecord( 12.000, 0.0000, 0.0881, 0.9846 );
		aMeasurements_5439->addRecord( 13.000, 0.0000, 0.0775, 0.9848 );
		aMeasurements_5439->addRecord( 14.000, 0.0000, 0.0715, 0.9850 );
		aMeasurements_5439->addRecord( 15.000, 0.0000, 0.0603, 0.9852 );
		aMeasurements_5439->addRecord( 16.000, 0.0000, 0.0495, 0.9855 );
		aMeasurements_5439->addRecord( 17.000, 0.0000, 0.0392, 0.9863 );
		aMeasurements_5439->addRecord( 18.000, 0.0000, 0.0260, 0.9874 );
		aMeasurements_5439->addRecord( 19.000, 0.0000, 0.0537, 0.9883 );
		aMeasurements_5439->addRecord( 20.000, 0.0000, 0.1614, 0.9885 );
		aMeasurements_5439->addRecord( 21.000, 0.0000, 0.2107, 0.9872 );
		aMeasurements_5439->addRecord( 22.000, 0.0000, 0.2206, 0.9873 );
		aMeasurements_5439->addRecord( 23.000, 0.0000, 0.2047, 0.9868 );
		aMeasurements_5439->addRecord( 24.000, 0.0000, 0.1862, 0.9864 );
		aMeasurements_5439->addRecord( 25.000, 0.0000, 0.1683, 0.9870 );
		aMeasurements_5439->addRecord( 26.000, 0.0000, 0.1562, 0.9873 );
		aMeasurements_5439->addRecord( 27.000, 0.0000, 0.1475, 0.9868 );
		aMeasurements_5439->addRecord( 28.000, 0.0000, 0.1405, 0.9880 );
		aMeasurements_5439->addRecord( 29.000, 0.0000, 0.1357, 0.9858 );
		aMeasurements_5439->addRecord( 30.000, 0.0000, 0.1315, 0.9864 );
		aMeasurements_5439->addRecord( 31.000, 0.0000, 0.1283, 0.9890 );
		aMeasurements_5439->addRecord( 32.000, 0.0000, 0.1278, 0.9870 );
		aMeasurements_5439->addRecord( 33.000, 0.0000, 0.1284, 0.9872 );
		aMeasurements_5439->addRecord( 34.000, 0.0000, 0.1276, 0.9871 );
		aMeasurements_5439->addRecord( 35.000, 0.0000, 0.1280, 0.9905 );
		aMeasurements_5439->addRecord( 36.000, 0.0000, 0.1304, 0.9899 );
		aMeasurements_5439->addRecord( 37.000, 0.0000, 0.1331, 0.9901 );
		aMeasurements_5439->addRecord( 38.000, 0.0000, 0.1336, 0.9882 );
		aMeasurements_5439->addRecord( 39.000, 0.0000, 0.1355, 0.9917 );
		aMeasurements_5439->addRecord( 40.000, 0.0000, 0.1381, 0.9915 );

		return aMeasurements_5439;

	}

	std::shared_ptr< CSeries > ASTM_E308_1964_X() const {
		auto detectorData = std::make_shared< CSeries >();

		detectorData->addProperty( 0.380, 0.0002 );
		detectorData->addProperty( 0.385, 0.0007 );
		detectorData->addProperty( 0.390, 0.0024 );
		detectorData->addProperty( 0.395, 0.0072 );
		detectorData->addProperty( 0.400, 0.0191 );
		detectorData->addProperty( 0.405, 0.0434 );
		detectorData->addProperty( 0.410, 0.0847 );
		detectorData->addProperty( 0.415, 0.1406 );
		detectorData->addProperty( 0.420, 0.2045 );
		detectorData->addProperty( 0.425, 0.2647 );
		detectorData->addProperty( 0.430, 0.3147 );
		detectorData->addProperty( 0.435, 0.3577 );
		detectorData->addProperty( 0.440, 0.3837 );
		detectorData->addProperty( 0.445, 0.3867 );
		detectorData->addProperty( 0.450, 0.3707 );
		detectorData->addProperty( 0.455, 0.3430 );
		detectorData->addProperty( 0.460, 0.3023 );
		detectorData->addProperty( 0.465, 0.2541 );
		detectorData->addProperty( 0.470, 0.1956 );
		detectorData->addProperty( 0.475, 0.1323 );
		detectorData->addProperty( 0.480, 0.0805 );
		detectorData->addProperty( 0.485, 0.0411 );
		detectorData->addProperty( 0.490, 0.0162 );
		detectorData->addProperty( 0.495, 0.0051 );
		detectorData->addProperty( 0.500, 0.0038 );
		detectorData->addProperty( 0.505, 0.0154 );
		detectorData->addProperty( 0.510, 0.0375 );
		detectorData->addProperty( 0.515, 0.0714 );
		detectorData->addProperty( 0.520, 0.1177 );
		detectorData->addProperty( 0.525, 0.1730 );
		detectorData->addProperty( 0.530, 0.2365 );
		detectorData->addProperty( 0.535, 0.3042 );
		detectorData->addProperty( 0.540, 0.3768 );
		detectorData->addProperty( 0.545, 0.4516 );
		detectorData->addProperty( 0.550, 0.5298 );
		detectorData->addProperty( 0.555, 0.6161 );
		detectorData->addProperty( 0.560, 0.7052 );
		detectorData->addProperty( 0.565, 0.7938 );
		detectorData->addProperty( 0.570, 0.8787 );
		detectorData->addProperty( 0.575, 0.9512 );
		detectorData->addProperty( 0.580, 1.0142 );
		detectorData->addProperty( 0.585, 1.0743 );
		detectorData->addProperty( 0.590, 1.1185 );
		detectorData->addProperty( 0.595, 1.1343 );
		detectorData->addProperty( 0.600, 1.1240 );
		detectorData->addProperty( 0.605, 1.0891 );
		detectorData->addProperty( 0.610, 1.0305 );
		detectorData->addProperty( 0.615, 0.9507 );
		detectorData->addProperty( 0.620, 0.8563 );
		detectorData->addProperty( 0.625, 0.7549 );
		detectorData->addProperty( 0.630, 0.6475 );
		detectorData->addProperty( 0.635, 0.5351 );
		detectorData->addProperty( 0.640, 0.4316 );
		detectorData->addProperty( 0.645, 0.3437 );
		detectorData->addProperty( 0.650, 0.2683 );
		detectorData->addProperty( 0.655, 0.2043 );
		detectorData->addProperty( 0.660, 0.1526 );
		detectorData->addProperty( 0.665, 0.1122 );
		detectorData->addProperty( 0.670, 0.0813 );
		detectorData->addProperty( 0.675, 0.0529 );
		detectorData->addProperty( 0.680, 0.0409 );
		detectorData->addProperty( 0.685, 0.0286 );
		detectorData->addProperty( 0.690, 0.0199 );
		detectorData->addProperty( 0.695, 0.0138 );
		detectorData->addProperty( 0.700, 0.0096 );
		detectorData->addProperty( 0.705, 0.0066 );
		detectorData->addProperty( 0.710, 0.0046 );
		detectorData->addProperty( 0.715, 0.0031 );
		detectorData->addProperty( 0.720, 0.0022 );
		detectorData->addProperty( 0.725, 0.0015 );
		detectorData->addProperty( 0.730, 0.0010 );
		detectorData->addProperty( 0.735, 0.0007 );
		detectorData->addProperty( 0.740, 0.0005 );
		detectorData->addProperty( 0.745, 0.0004 );
		detectorData->addProperty( 0.750, 0.0003 );
		detectorData->addProperty( 0.755, 0.0002 );
		detectorData->addProperty( 0.760, 0.0001 );
		detectorData->addProperty( 0.765, 0.0001 );
		detectorData->addProperty( 0.770, 0.0001 );
		detectorData->addProperty( 0.775, 0.0000 );
		detectorData->addProperty( 0.780, 0.0000 );

		return detectorData;
	}

	std::shared_ptr< CSeries > ASTM_E308_1964_Y() const {
		auto detectorData = std::make_shared< CSeries >();

		detectorData->addProperty( 0.380, 0.0000 );
		detectorData->addProperty( 0.385, 0.0001 );
		detectorData->addProperty( 0.390, 0.0003 );
		detectorData->addProperty( 0.395, 0.0008 );
		detectorData->addProperty( 0.400, 0.0020 );
		detectorData->addProperty( 0.405, 0.0045 );
		detectorData->addProperty( 0.410, 0.0088 );
		detectorData->addProperty( 0.415, 0.0145 );
		detectorData->addProperty( 0.420, 0.0214 );
		detectorData->addProperty( 0.425, 0.0295 );
		detectorData->addProperty( 0.430, 0.0387 );
		detectorData->addProperty( 0.435, 0.0496 );
		detectorData->addProperty( 0.440, 0.0621 );
		detectorData->addProperty( 0.445, 0.0747 );
		detectorData->addProperty( 0.450, 0.0895 );
		detectorData->addProperty( 0.455, 0.1063 );
		detectorData->addProperty( 0.460, 0.1282 );
		detectorData->addProperty( 0.465, 0.1528 );
		detectorData->addProperty( 0.470, 0.1852 );
		detectorData->addProperty( 0.475, 0.2199 );
		detectorData->addProperty( 0.480, 0.2536 );
		detectorData->addProperty( 0.485, 0.2977 );
		detectorData->addProperty( 0.490, 0.3391 );
		detectorData->addProperty( 0.495, 0.3954 );
		detectorData->addProperty( 0.500, 0.4608 );
		detectorData->addProperty( 0.505, 0.5314 );
		detectorData->addProperty( 0.510, 0.6067 );
		detectorData->addProperty( 0.515, 0.6857 );
		detectorData->addProperty( 0.520, 0.7618 );
		detectorData->addProperty( 0.525, 0.8233 );
		detectorData->addProperty( 0.530, 0.8752 );
		detectorData->addProperty( 0.535, 0.9238 );
		detectorData->addProperty( 0.540, 0.9620 );
		detectorData->addProperty( 0.545, 0.9822 );
		detectorData->addProperty( 0.550, 0.9918 );
		detectorData->addProperty( 0.555, 0.9991 );
		detectorData->addProperty( 0.560, 0.9973 );
		detectorData->addProperty( 0.565, 0.9824 );
		detectorData->addProperty( 0.570, 0.9555 );
		detectorData->addProperty( 0.575, 0.9152 );
		detectorData->addProperty( 0.580, 0.8689 );
		detectorData->addProperty( 0.585, 0.8256 );
		detectorData->addProperty( 0.590, 0.7774 );
		detectorData->addProperty( 0.595, 0.7204 );
		detectorData->addProperty( 0.600, 0.6583 );
		detectorData->addProperty( 0.605, 0.5939 );
		detectorData->addProperty( 0.610, 0.5280 );
		detectorData->addProperty( 0.615, 0.4618 );
		detectorData->addProperty( 0.620, 0.3981 );
		detectorData->addProperty( 0.625, 0.3396 );
		detectorData->addProperty( 0.630, 0.2835 );
		detectorData->addProperty( 0.635, 0.2283 );
		detectorData->addProperty( 0.640, 0.1798 );
		detectorData->addProperty( 0.645, 0.1402 );
		detectorData->addProperty( 0.650, 0.1076 );
		detectorData->addProperty( 0.655, 0.0812 );
		detectorData->addProperty( 0.660, 0.0603 );
		detectorData->addProperty( 0.665, 0.0441 );
		detectorData->addProperty( 0.670, 0.0318 );
		detectorData->addProperty( 0.675, 0.0226 );
		detectorData->addProperty( 0.680, 0.0159 );
		detectorData->addProperty( 0.685, 0.0111 );
		detectorData->addProperty( 0.690, 0.0077 );
		detectorData->addProperty( 0.695, 0.0054 );
		detectorData->addProperty( 0.700, 0.0037 );
		detectorData->addProperty( 0.705, 0.0026 );
		detectorData->addProperty( 0.710, 0.0018 );
		detectorData->addProperty( 0.715, 0.0012 );
		detectorData->addProperty( 0.720, 0.0008 );
		detectorData->addProperty( 0.725, 0.0006 );
		detectorData->addProperty( 0.730, 0.0004 );
		detectorData->addProperty( 0.735, 0.0003 );
		detectorData->addProperty( 0.740, 0.0002 );
		detectorData->addProperty( 0.745, 0.0001 );
		detectorData->addProperty( 0.750, 0.0001 );
		detectorData->addProperty( 0.755, 0.0001 );
		detectorData->addProperty( 0.760, 0.0000 );
		detectorData->addProperty( 0.765, 0.0000 );
		detectorData->addProperty( 0.770, 0.0000 );
		detectorData->addProperty( 0.775, 0.0000 );
		detectorData->addProperty( 0.780, 0.0000 );

		return detectorData;
	}


	std::shared_ptr< CSeries > ASTM_E308_1964_Z() const {
		auto detectorData = std::make_shared< CSeries >();

		detectorData->addProperty( 0.380, 0.0007 );
		detectorData->addProperty( 0.385, 0.0029 );
		detectorData->addProperty( 0.390, 0.0105 );
		detectorData->addProperty( 0.395, 0.0323 );
		detectorData->addProperty( 0.400, 0.0860 );
		detectorData->addProperty( 0.405, 0.1971 );
		detectorData->addProperty( 0.410, 0.3894 );
		detectorData->addProperty( 0.415, 0.6568 );
		detectorData->addProperty( 0.420, 0.9725 );
		detectorData->addProperty( 0.425, 1.2825 );
		detectorData->addProperty( 0.430, 1.5535 );
		detectorData->addProperty( 0.435, 1.7985 );
		detectorData->addProperty( 0.440, 1.9673 );
		detectorData->addProperty( 0.445, 2.0273 );
		detectorData->addProperty( 0.450, 1.9948 );
		detectorData->addProperty( 0.455, 1.9007 );
		detectorData->addProperty( 0.460, 1.7454 );
		detectorData->addProperty( 0.465, 1.5549 );
		detectorData->addProperty( 0.470, 1.3176 );
		detectorData->addProperty( 0.475, 1.0302 );
		detectorData->addProperty( 0.480, 0.7721 );
		detectorData->addProperty( 0.485, 0.5701 );
		detectorData->addProperty( 0.490, 0.4153 );
		detectorData->addProperty( 0.495, 0.3024 );
		detectorData->addProperty( 0.500, 0.2185 );
		detectorData->addProperty( 0.505, 0.1592 );
		detectorData->addProperty( 0.510, 0.1120 );
		detectorData->addProperty( 0.515, 0.0822 );
		detectorData->addProperty( 0.520, 0.0607 );
		detectorData->addProperty( 0.525, 0.0431 );
		detectorData->addProperty( 0.530, 0.0305 );
		detectorData->addProperty( 0.535, 0.0206 );
		detectorData->addProperty( 0.540, 0.0137 );
		detectorData->addProperty( 0.545, 0.0079 );
		detectorData->addProperty( 0.550, 0.0040 );
		detectorData->addProperty( 0.555, 0.0011 );
		detectorData->addProperty( 0.560, 0.0000 );
		detectorData->addProperty( 0.565, 0.0000 );
		detectorData->addProperty( 0.570, 0.0000 );
		detectorData->addProperty( 0.575, 0.0000 );
		detectorData->addProperty( 0.580, 0.0000 );
		detectorData->addProperty( 0.585, 0.0000 );
		detectorData->addProperty( 0.590, 0.0000 );
		detectorData->addProperty( 0.595, 0.0000 );
		detectorData->addProperty( 0.600, 0.0000 );
		detectorData->addProperty( 0.605, 0.0000 );
		detectorData->addProperty( 0.610, 0.0000 );
		detectorData->addProperty( 0.615, 0.0000 );
		detectorData->addProperty( 0.620, 0.0000 );
		detectorData->addProperty( 0.625, 0.0000 );
		detectorData->addProperty( 0.630, 0.0000 );
		detectorData->addProperty( 0.635, 0.0000 );
		detectorData->addProperty( 0.640, 0.0000 );
		detectorData->addProperty( 0.645, 0.0000 );
		detectorData->addProperty( 0.650, 0.0000 );
		detectorData->addProperty( 0.655, 0.0000 );
		detectorData->addProperty( 0.660, 0.0000 );
		detectorData->addProperty( 0.665, 0.0000 );
		detectorData->addProperty( 0.670, 0.0000 );
		detectorData->addProperty( 0.675, 0.0000 );
		detectorData->addProperty( 0.680, 0.0000 );
		detectorData->addProperty( 0.685, 0.0000 );
		detectorData->addProperty( 0.690, 0.0000 );
		detectorData->addProperty( 0.695, 0.0000 );
		detectorData->addProperty( 0.700, 0.0000 );
		detectorData->addProperty( 0.705, 0.0000 );
		detectorData->addProperty( 0.710, 0.0000 );
		detectorData->addProperty( 0.715, 0.0000 );
		detectorData->addProperty( 0.720, 0.0000 );
		detectorData->addProperty( 0.725, 0.0000 );
		detectorData->addProperty( 0.730, 0.0000 );
		detectorData->addProperty( 0.735, 0.0000 );
		detectorData->addProperty( 0.740, 0.0000 );
		detectorData->addProperty( 0.745, 0.0000 );
		detectorData->addProperty( 0.750, 0.0000 );
		detectorData->addProperty( 0.755, 0.0000 );
		detectorData->addProperty( 0.760, 0.0000 );
		detectorData->addProperty( 0.765, 0.0000 );
		detectorData->addProperty( 0.770, 0.0000 );
		detectorData->addProperty( 0.775, 0.0000 );
		detectorData->addProperty( 0.780, 0.0000 );

		return detectorData;
	}

	std::shared_ptr< SingleLayerOptics::CScatteringLayer > createXLayer() const {
		auto aSolarRadiation = loadSolarRadiationFile();
		auto aMeasurements = loadSampleData_NFRC_5439();
		auto wavelengths = loadWavelengths();

		auto aDetectorData = ASTM_E308_1964_X();

		auto aSample = std::make_shared< SpectralAveraging::CSpectralSample >( aMeasurements,
				aSolarRadiation );

		aSample->setDetectorData( aDetectorData );
		aSample->setWavelengths( SpectralAveraging::WavelengthSet::Custom, wavelengths );

		double thickness = 3.048e-3; // [m]
		std::shared_ptr< SingleLayerOptics::CMaterial > aMaterial = std::make_shared< SingleLayerOptics::CMaterialSample >(
				aSample, thickness, FenestrationCommon::MaterialType::Monolithic,
				FenestrationCommon::WavelengthRange::Visible );

		return std::make_shared< SingleLayerOptics::CScatteringLayer >( aMaterial );
	}

	std::shared_ptr< SingleLayerOptics::CScatteringLayer > createYLayer() const {
		auto aSolarRadiation = loadSolarRadiationFile();
		auto aMeasurements = loadSampleData_NFRC_5439();
		auto wavelengths = loadWavelengths();

		auto aDetectorData = ASTM_E308_1964_Y();

		auto aSample = std::make_shared< SpectralAveraging::CSpectralSample >( aMeasurements,
				aSolarRadiation );

		aSample->setDetectorData( aDetectorData );
		aSample->setWavelengths( SpectralAveraging::WavelengthSet::Custom, wavelengths );

		double thickness = 3.048e-3; // [m]
		std::shared_ptr< SingleLayerOptics::CMaterial > aMaterial = std::make_shared< SingleLayerOptics::CMaterialSample >(
				aSample, thickness, FenestrationCommon::MaterialType::Monolithic,
				FenestrationCommon::WavelengthRange::Visible );

		return std::make_shared< SingleLayerOptics::CScatteringLayer >( aMaterial );
	}

	std::shared_ptr< SingleLayerOptics::CScatteringLayer > createZLayer() const {
		auto aSolarRadiation = loadSolarRadiationFile();
		auto aMeasurements = loadSampleData_NFRC_5439();
		auto wavelengths = loadWavelengths();

		auto aDetectorData = ASTM_E308_1964_Z();

		auto aSample = std::make_shared< SpectralAveraging::CSpectralSample >( aMeasurements,
				aSolarRadiation );

		aSample->setDetectorData( aDetectorData );
		aSample->setWavelengths( SpectralAveraging::WavelengthSet::Custom, wavelengths );

		double thickness = 3.048e-3; // [m]
		std::shared_ptr< SingleLayerOptics::CMaterial > aMaterial = std::make_shared< SingleLayerOptics::CMaterialSample >(
				aSample, thickness, FenestrationCommon::MaterialType::Monolithic,
				FenestrationCommon::WavelengthRange::Visible );

		return std::make_shared< SingleLayerOptics::CScatteringLayer >( aMaterial );
	}

protected:
	void SetUp() override {

		std::shared_ptr< SingleLayerOptics::IScatteringLayer > LayerX = createXLayer();
		std::shared_ptr< SingleLayerOptics::IScatteringLayer > LayerY = createYLayer();
		std::shared_ptr< SingleLayerOptics::IScatteringLayer > LayerZ = createZLayer();

		CSeries DX = *ASTM_E308_1964_X();
		CSeries DY = *ASTM_E308_1964_Y();
		CSeries DZ = *ASTM_E308_1964_Z();

		CSeries solarRadiation = *loadSolarRadiationFile();

		auto wl = loadWavelengths();

		m_Color = std::make_shared< SingleLayerOptics::ColorProperties >( LayerX, LayerY, LayerZ,
				solarRadiation, DX, DY, DZ, wl );

	}

public:

	std::shared_ptr< SingleLayerOptics::ColorProperties > getLayer() const {
		return m_Color;
	}

};

TEST_F( TestNFRC_5439_SB70XL_Colors, TestTrichromatic ) {
	SCOPED_TRACE( "Begin Test: Trichromatic." );

	std::shared_ptr< SingleLayerOptics::ColorProperties > aLayer = getLayer();

	FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

	SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
			FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect );
	EXPECT_NEAR( 66.393144, T.X, 1e-6 );
	EXPECT_NEAR( 71.662457, T.Y, 1e-6 );
	EXPECT_NEAR( 71.768345, T.Z, 1e-6 );

}

TEST_F( TestNFRC_5439_SB70XL_Colors, TestRGB ) {
	SCOPED_TRACE( "Begin Test: RGB." );

	std::shared_ptr< SingleLayerOptics::ColorProperties > aLayer = getLayer();

	FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

	auto rgb = aLayer->getRGB( FenestrationCommon::PropertySimple::T, aSide,
			FenestrationCommon::Scattering::DirectDirect );
	EXPECT_EQ( 239, rgb.R );
	EXPECT_EQ( 245, rgb.G );
	EXPECT_EQ( 233, rgb.B );

}

TEST_F( TestNFRC_5439_SB70XL_Colors, TestCIE_LAB ) {
	SCOPED_TRACE( "Begin Test: CIE_LAB." );

	std::shared_ptr< SingleLayerOptics::ColorProperties > aLayer = getLayer();

	FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

	auto cie = aLayer->getCIE_Lab( FenestrationCommon::PropertySimple::T, aSide,
			FenestrationCommon::Scattering::DirectDirect );
	EXPECT_NEAR( 87.805864, cie.L, 1e-6 );
	EXPECT_NEAR( -3.402796, cie.a, 1e-6 );
	EXPECT_NEAR( 4.081155, cie.b, 1e-6 );

}
