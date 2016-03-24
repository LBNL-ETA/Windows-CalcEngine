#ifndef BSDFTHETALIMITS_H
#define BSDFTHETALIMITS_H

#include <vector>
#include <memory>

namespace LayerOptics {
  
  class CThetaLimits {
  public:
    CThetaLimits( const std::vector< double >& t_ThetaAngles );
  
    std::shared_ptr< std::vector< double > > getThetaLimits() const;
  
  private:
    void createLimits( const std::vector< double >& t_ThetaAngles );
    std::shared_ptr< std::vector< double > > m_ThetaLimits;
  };

}

#endif