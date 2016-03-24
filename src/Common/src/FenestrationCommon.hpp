#ifndef FENESTRATIONCOMMON_H
#define FENESTRATIONCOMMON_H

namespace FenestrationCommon {

  template< typename T >
  class Enum {
  public:
     class Iterator
     {
     public:
        Iterator( int value ) :
           m_value( value )
        { }
  
        T operator*( void ) const
        {
           return ( T )m_value;
        }
  
        void operator++( void )
        {
           ++m_value;
        }
  
        bool operator!=( Iterator rhs )
        {
           return m_value != rhs.m_value;
        }
  
     private:
        int m_value;
     };
  
  };
  
  template< typename T >
  typename Enum< T >::Iterator begin( Enum< T > ) {
     return typename Enum< T >::Iterator( ( int )T::Front );
  }
  
  template< typename T >
  typename Enum<T>::Iterator end( Enum< T > ) {
     return typename Enum< T >::Iterator( ( ( int )T::Back) + 1 );
  }
  
  enum class Side { Front, Back };
  enum class Property { T, R, Abs };

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