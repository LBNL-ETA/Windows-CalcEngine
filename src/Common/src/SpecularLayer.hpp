#ifndef SPECULARLAYER_H
#define SPECULARLAYER_H

namespace FenestrationCommon {

  enum class SpecularLayerSurfaceType { None, Coated, Uncoated };

  enum class SpecularMaterialType { None, Monolithic, Coated, Film, AppliedFilm, Laminate, Interlayer, 
    Electrochromic, Thermochromic };

  SpecularLayerSurfaceType coatingType( const SpecularMaterialType t_LayerType );

}

#endif