#include <assert.h>
#include "SpecularLayer.hpp"

namespace FenestrationCommon {

  SpecularLayerSurfaceType coatingType( const SpecularMaterialType t_LayerType ) {
    SpecularLayerSurfaceType aType = SpecularLayerSurfaceType::None;
    switch ( t_LayerType ) {
    case SpecularMaterialType::Monolithic:
      aType = SpecularLayerSurfaceType::Uncoated;
      break;
    case SpecularMaterialType::Coated:
      aType = SpecularLayerSurfaceType::Coated;
      break;
    case SpecularMaterialType::Film:
      aType = SpecularLayerSurfaceType::Coated;
      break;
    case SpecularMaterialType::AppliedFilm:
      aType = SpecularLayerSurfaceType::Coated;
      break;
    case SpecularMaterialType::Laminate:
      aType = SpecularLayerSurfaceType::Coated;
      break;
    case SpecularMaterialType::Interlayer:
      aType = SpecularLayerSurfaceType::Uncoated;
      break;
    case SpecularMaterialType::Electrochromic:
      aType = SpecularLayerSurfaceType::Coated;
      break;
    case SpecularMaterialType::Thermochromic:
      aType = SpecularLayerSurfaceType::Coated;
      break;
    default:
      assert("Incorrect selection of layer type.");
      break;
    }

    return aType;
  }

}