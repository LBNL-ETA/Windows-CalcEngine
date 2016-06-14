#ifndef FENESTRATIONCOMMON_H
#define FENESTRATIONCOMMON_H

namespace FenestrationCommon {

  template< typename T >
  class Enum {
  public:
     class Iterator {
     public:
        explicit Iterator( int value ) :
           m_value( value ) { }
  
        T operator*( void ) const {
           return ( T )m_value;
        }
  
        void operator++( void ) {
           ++m_value;
        }
  
        bool operator!=( Iterator rhs ) {
           return m_value != rhs.m_value;
        }
  
     private:
        int m_value;
     };
  
  };

  // Side enumerator

  enum class Side { Front, Back };

  class EnumSide : public Enum< Side > {
	
  };

  inline EnumSide::Iterator begin( EnumSide ) {
    return EnumSide::Iterator( ( int )Side::Front );
  }
  
  inline EnumSide::Iterator end( EnumSide ) {
    return typename EnumSide::Iterator( ( ( int )Side::Back ) + 1 );
  }
  
  // Property enumerator

  enum class Property { T, R, Abs };

  class EnumProperty : public Enum< Property > {
	
  };

  inline EnumProperty::Iterator begin( EnumProperty ) {
    return EnumProperty::Iterator( ( int )Property::T );
  }
  
  inline EnumProperty::Iterator end( EnumProperty ) {
    return EnumProperty::Iterator( ( ( int )Property::Abs ) + 1 );
  }

  inline Side oppositeSide( const Side t_Side ) {
    Side aSide = Side::Front;
    if( t_Side == Side::Front ) {
      aSide = Side::Back;
    }
    return aSide;
  }

  enum class WavelengthRange { Solar, Visible, IR };

  // Short version of enum class Property is necessary because in optical routines it is quite often the case
  // when calculations are performed only over transmittance and reflectance. It is also often the case when
  // Transmittance and Reflectance have different structure from absorptances.
  enum class PropertySimple { T, R };

  class EnumPropertySimple : public Enum< PropertySimple > {
	
  };

  inline EnumPropertySimple::Iterator begin( EnumPropertySimple ) {
    return EnumPropertySimple::Iterator( ( int )PropertySimple::T );
  }
  
  inline EnumPropertySimple::Iterator end( EnumPropertySimple ) {
    return EnumPropertySimple::Iterator( ( ( int )PropertySimple::R ) + 1 );
  }

  enum class Scattering { DirectDirect, DirectDiffuse, DiffuseDiffuse };
  enum class ScatteringSimple { Direct, Diffuse };
  
}

#endif