#ifndef WOVENCELLDESCRIPTION_H
#define WOVENCELLDESCRIPTION_H

#include "BaseCellDescription.hpp"

namespace LayerOptics {

  class CWovenCellDescription : public CBaseCellDescription {
  public:
    CWovenCellDescription( const double t_Diameter, const double t_Spacing );

    double gamma() const;

    double T_dir_dir( const FenestrationCommon::Side t_Side, std::shared_ptr< const CBeamDirection > t_Direction );

  private:
    double Tx( std::shared_ptr< const CBeamDirection > t_Direction );
    double Ty( std::shared_ptr< const CBeamDirection > t_Direction );

    double diameter() const;
    double spacing() const;
    double cutOffAngle() const;

    double m_Diameter;
    double m_Spacing;
  };

}

#endif