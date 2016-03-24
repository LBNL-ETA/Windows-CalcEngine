#ifndef PERFORATEDCELLDESCRIPTION_H
#define PERFORATEDCELLDESCRIPTION_H

#include "BaseCellDescription.hpp"

namespace LayerOptics {
  
  class CPerforatedCellDescription : public CBaseCellDescription {
  public:
    CPerforatedCellDescription( const double t_x, const double t_y, const double t_Thickness );

  protected:
    double m_x;
    double m_y;
    double m_Thickness;
  };

  class CCircularCellDescription : public CPerforatedCellDescription {
  public:
    CCircularCellDescription( const double t_x, const double t_y, const double t_Thickness, 
      const double t_Radius );

    double T_dir_dir( const FenestrationCommon::Side t_Side, std::shared_ptr< const CBeamDirection > t_Direction );

  private:
    double visibleAhole( std::shared_ptr< const CBeamDirection > t_Direction ) const;
		double visibleAcell( std::shared_ptr< const CBeamDirection > t_Direction ) const;
    double m_Radius;
  };

  class CRectangularCellDescription : public CPerforatedCellDescription {
  public:
    CRectangularCellDescription( const double t_x, const double t_y, const double t_Thickness, 
      const double t_XHole, const double t_YHole );

    double T_dir_dir( const FenestrationCommon::Side t_Side, std::shared_ptr< const CBeamDirection > t_Direction );

  private:
    double TransmittanceV( std::shared_ptr< const CBeamDirection > t_Direction ) const;
		double TransmittanceH( std::shared_ptr< const CBeamDirection > t_Direction ) const;

    double m_XHole;
    double m_YHole;
  };

}

#endif