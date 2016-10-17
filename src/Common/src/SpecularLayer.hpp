#ifndef SPECULARLAYER_H
#define SPECULARLAYER_H

namespace FenestrationCommon {

  enum class SurfaceType { None, Coated, Uncoated };

  enum class MaterialType { None, Monolithic, Coated, Film, AppliedFilm, Laminate, Interlayer, 
    Electrochromic, Thermochromic };

  SurfaceType coatingType( const MaterialType t_LayerType );

}

#endif