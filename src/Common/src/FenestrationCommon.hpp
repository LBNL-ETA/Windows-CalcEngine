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

  // template< typename T >
  class EnumSide : public Enum< Side > {
	
  };

  inline EnumSide::Iterator begin( EnumSide ) {
    return EnumSide::Iterator( ( int )Side::Front );
  }
  
  inline EnumSide::Iterator end( EnumSide ) {
    return typename EnumSide::Iterator( ( ( int )Side::Back ) + 1 );
  }
  
  // template< typename T >
  // typename EnumSide< T >::Iterator begin( EnumSide< T > ) {
  //   return typename EnumSide< T >::Iterator( ( int )T::Front );
  // }
  // 
  // template< typename T >
  // typename EnumSide< T >::Iterator end( EnumSide< T > ) {
  //   return typename EnumSide< T >::Iterator( ( ( int )T::Back ) + 1 );
  // }
  
  // Property enumerator

  enum class Property { T, R, Abs };

  // template< typename T >
  class EnumProperty : public Enum< Property > {
	
  };

  inline EnumProperty::Iterator begin( EnumProperty ) {
    return EnumProperty::Iterator( ( int )Property::T );
  }
  
  inline EnumProperty::Iterator end( EnumProperty ) {
    return EnumProperty::Iterator( ( ( int )Property::Abs ) + 1 );
  }
  
  // template< typename T >
  // typename EnumProperty< T >::Iterator begin( EnumProperty< T > ) {
  //   return typename EnumProperty< T >::Iterator( ( int )T::T );
  // }
  // 
  // template< typename T >
  // typename EnumProperty<T>::Iterator end( EnumProperty< T > ) {
  //   return typename EnumProperty< T >::Iterator( ( ( int )T::Abs ) + 1 );
  // }

  inline Side oppositeSide( const Side t_Side ) {
    Side aSide = Side::Front;
    if( t_Side == Side::Front ) {
      aSide = Side::Back;
    }
    return aSide;
  }

  enum class WavelengthRange { Solar, Visible, IR };
  
}

#endif