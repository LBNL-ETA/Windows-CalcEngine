#include <assert.h>
#include "SpecularLayer.hpp"

namespace FenestrationCommon {

  SurfaceType coatingType( const MaterialType t_LayerType ) {
    SurfaceType aType = SurfaceType::None;
    switch ( t_LayerType ) {
    case MaterialType::Monolithic:
      aType = SurfaceType::Uncoated;
      break;
    case MaterialType::Coated:
      aType = SurfaceType::Coated;
      break;
    case MaterialType::Film:
      aType = SurfaceType::Coated;
      break;
    case MaterialType::AppliedFilm:
      aType = SurfaceType::Coated;
      break;
    case MaterialType::Laminate:
      aType = SurfaceType::Coated;
      break;
    case MaterialType::Interlayer:
      aType = SurfaceType::Uncoated;
      break;
    case MaterialType::Electrochromic:
      aType = SurfaceType::Coated;
      break;
    case MaterialType::Thermochromic:
      aType = SurfaceType::Coated;
      break;
    default:
      assert("Incorrect selection of layer type.");
      break;
    }

    return aType;
  }

}