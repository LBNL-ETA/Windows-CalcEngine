#include "Material.hpp"
#include "WCECommon.hpp"
#include "MaterialDescription.hpp"
#include "WCESpectralAveraging.hpp"

using FenestrationCommon::WavelengthRange;
using FenestrationCommon::CSeries;
using SpectralAveraging::CSpectralSample;

namespace SingleLayerOptics {
	std::shared_ptr< CMaterial > Material::dualBandMaterial( const double Tfsol,
															 const double Tbsol,
															 const double Rfsol,
															 const double Rbsol,
															 const double Tfvis,
															 const double Tbvis,
															 const double Rfvis,
															 const double Rbvis ) {
		auto aSolarRangeMaterial =
			std::make_shared< CMaterialSingleBand >( Tfsol, Tbsol, Rfsol, Rbsol,
													 WavelengthRange::Solar );
		auto aVisibleRangeMaterial = std::make_shared< CMaterialSingleBand >(
			Tfvis, Tbvis, Rfvis, Rbvis, WavelengthRange::Visible );
		return std::make_shared< CMaterialDualBand >( aVisibleRangeMaterial, aSolarRangeMaterial );
	}

	std::shared_ptr< CMaterial >
	Material::singleBandMaterial( const double Tf, const double Tb, const double Rf,
								  const double Rb, const double minLambda,
								  const double maxLambda ) {
		return std::make_shared< CMaterialSingleBand >( Tf, Tb, Rf, Rb, minLambda, maxLambda );
	}

	std::shared_ptr< CMaterial >
	Material::singleBandMaterial( const double Tf, const double Tb, const double Rf,
								  const double Rb,
								  const FenestrationCommon::WavelengthRange range ) {
		return std::make_shared< CMaterialSingleBand >( Tf, Tb, Rf, Rb, range );
	}

	std::shared_ptr< CMaterial > Material::nBandMaterial(
		const std::shared_ptr< SpectralAveraging::CSpectralSampleData > & measurement,
		const double thickness, const FenestrationCommon::MaterialType materialType,
		const FenestrationCommon::WavelengthRange range,
		const std::shared_ptr<FenestrationCommon::CSeries> & detectorData) {
		auto aSample = std::make_shared< CSpectralSample >( measurement );
		if(detectorData != nullptr)
		{
			aSample->setDetectorData(detectorData);
		}
		return std::make_shared< CMaterialSample >( aSample, thickness, materialType, range );
	}

	std::shared_ptr< CMaterial > Material::nBandMaterial(
		const std::shared_ptr< SpectralAveraging::CSpectralSampleData > & measurement,
		const double thickness, const FenestrationCommon::MaterialType materialType,
		const double minLambda,
		const double maxLambda,
		const std::shared_ptr<FenestrationCommon::CSeries> & detectorData) {
		auto aSample = std::make_shared< CSpectralSample >( measurement );
		if(detectorData != nullptr)
		{
			aSample->setDetectorData(detectorData);
		}
		return std::make_shared< CMaterialSample >( aSample, thickness, materialType, minLambda,
													maxLambda );
	}
}   // namespace SingleLayerOptics